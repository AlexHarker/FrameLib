
#include "Max_Base.h"

#include "FrameLib_Multichannel.h"
#include "FrameLib_DSP.h"
#include "FrameLib_Global.h"
#include "FrameLib_Context.h"

#include <vector>

// FIX - improve reporting of extra connections + look into feedback detection...
// FIX - think about adding assist helpers for this later...
// FIX - threadsafety??
// FIX - look at static items

t_class *objectClass = NULL;
t_class *wrapperClass = NULL;
t_class *mutatorClass = NULL;

//////////////////////////////////////////////////////////////////////////
///////////////////////////// Sync Check Class ///////////////////////////
//////////////////////////////////////////////////////////////////////////

class SyncCheck
{
    
public:
    
    SyncCheck() : mTime(-1), mObject(NULL) {}
    
    bool operator()()
    {
        SyncCheck *info = *syncInfo();
        
        if (!info || (info->mTime == mTime && info->mObject == mObject))
            return false;
        
        *this = *info;
        
        return true;
    }
    
    void syncSet(void *object, long time)
    {
        mObject = object;
        mTime = time;
        *syncInfo() = (object ? this : NULL);
    }
    
private:
    
    SyncCheck **syncInfo() { return (SyncCheck **) &gensym("__FrameLib__SYNC__")->s_thing; }
    
    long mTime;
    void *mObject;
};

//////////////////////////////////////////////////////////////////////////
////////////////////// Mutator for Synchronisation ///////////////////////
//////////////////////////////////////////////////////////////////////////

class Mutator : public MaxBase
{
    
public:
    
    Mutator(t_symbol *sym, long ac, t_atom *av)
    {
        mOutlet = outlet_new(this, "sync");
    }
    
    static void classInit(t_class *c, t_symbol *nameSpace, const char *classname)
    {
        addMethod<Mutator, &Mutator::mutate>(c, "signal");
    }
    
    void mutate(t_symbol *sym, long ac, t_atom *av)
    {
        mSyncChecker.syncSet(this, gettime());
        outlet_anything(mOutlet, gensym("sync"), 0, 0);
        mSyncChecker.syncSet(NULL, gettime());
    }
    
private:
    
    SyncCheck mSyncChecker;
    
    void *mOutlet;
};

//////////////////////////////////////////////////////////////////////////
////////////////////// Wrapper for Synchronisation ///////////////////////
//////////////////////////////////////////////////////////////////////////

template <class T> class Wrapper : public MaxBase
{
    
public:
    
    Wrapper(t_symbol *s, long argc, t_atom *argv)
    {
        // Create Patcher (you must report this as a subpatcher to get audio working)
        
        t_dictionary *d = dictionary_new();
        t_atom a;
        t_atom *av = NULL;
        long ac = 0;
        
        atom_setparse(&ac, &av, "@defrect 0 0 300 300");
        attr_args_dictionary(d, ac, av);
        atom_setobj(&a, d);
        mPatch = (t_object *)object_new_typed(CLASS_NOBOX, gensym("jpatcher"),1, &a);
        
        // Create Objects
        
        // FIX - this should not be a macro for the name (needs to be stored somehow?? with the class...
        
        char name[256];
        sprintf(name, "unsynced.%s", OBJECT_NAME);
        
        // FIX - make me better
        
        char *text = NULL;
        long textSize = 0;
        
        atom_gettext(argc, argv, &textSize, &text, 0);
        mObject = jbox_get_object((t_object *) newobject_sprintf(mPatch, "@maxclass newobj @text \"%s %s\" @patching_rect 0 0 30 10", name, text));
        mMutator = (t_object *) object_new_typed(CLASS_NOBOX, gensym("__fl.signal.mutator"), 0, NULL);
        
        // Free resources we no longer need
        
        sysmem_freeptr(av);
        freeobject((t_object *)d);
        sysmem_freeptr(text);
        
        // Get the object itself (typed)
        
        T *internal = (T *) mObject;
        
        long numIns = internal->getNumIns();
        long numOuts = internal->getNumOuts();
        long numAudioIns = internal->getNumAudioIns();
        long numAudioOuts = internal->getNumAudioOuts();
        
        internal->mUserObject = *this;
        
        // Create I/O
        
        mInOutlets.resize(numIns + numAudioIns - 1);
        mProxyIns.resize(numIns + numAudioIns - 1);
        mAudioOuts.resize(numAudioOuts - 1);
        mOuts.resize(numOuts);
        
        // Inlets for messages/signals
        
        mSyncIn = (t_object *) outlet_new(NULL, NULL);
        
        for (long i = numIns + numAudioIns - 2; i >= 0 ; i--)
        {
            mInOutlets[i] = (t_object *) outlet_new(NULL, NULL);
            mProxyIns[i] = (t_object *)  (i ? proxy_new(this, i, &mProxyNum) : NULL);
        }
        
        // Outlets for messages/signals
        
        for (long i = numOuts - 1; i >= 0 ; i--)
            mOuts[i] = (t_object *) outlet_new(this, NULL);
        for (long i = numAudioOuts - 2; i >= 0 ; i--)
            mAudioOuts[i] = (t_object *) outlet_new(this, "signal");
        
        // Connect the audio sync in to the object and through to the mutator
        
        outlet_add(mSyncIn, inlet_nth(mObject, 0));
        outlet_add(outlet_nth(mObject, 0), inlet_nth(mMutator, 0));
        
        // Connect other inlets (all types)
        
        for (long i = 0; i < numAudioIns + numIns - 1; i++)
            outlet_add(mInOutlets[i], inlet_nth(mObject, i + 1));
        
        // Connect other outlets (audio then frame and sync message outlets)
        
        for (long i = 0; i < numAudioOuts - 1; i++)
            outlet_add(outlet_nth(mObject, i + 1), mAudioOuts[i]);
        
        for (long i = 0; i < numOuts; i++)
        {
            outlet_add(outlet_nth(mObject, i + numAudioOuts), mOuts[i]);
            outlet_add(outlet_nth(mMutator, 0), mOuts[i]);
        }
    }
    
    ~Wrapper()
    {
        // Delete ins and proxies
        
        for (std::vector <t_object *>::iterator it = mProxyIns.begin(); it != mProxyIns.end(); it++)
            object_free(*it);
        
        for (std::vector <t_object *>::iterator it = mInOutlets.begin(); it != mInOutlets.end(); it++)
            object_free(*it);
        
        // Free objects - N.B. - free the patch, but not the object within it (which will be freed by deleting the patch)
        
        freeobject(mSyncIn);
        freeobject(mMutator);
        freeobject(mPatch);
    }
    
    void assist(void *b, long m, long a, char *s)
    {
        ((T *)mObject)->assist(b, m, a + 1, s);
    }
    
    void *subpatcher(long index, void *arg)
    {
        if ((t_ptr_uint) arg <= 1 || NOGOOD(arg))
            return NULL;
        
        return (index == 0) ? (void *) mPatch : NULL;
    }
    
    void sync(t_symbol *sym, long ac, t_atom *av)
    {
        if (mSyncChecker())
            outlet_anything(mSyncIn, gensym("signal"), ac, av);
    }
    
    void anything(t_symbol *sym, long ac, t_atom *av)
    {
        long inlet = getInlet();
        
        outlet_anything(mInOutlets[inlet], sym, ac, av);
    }
    
    // Initialise Class
    
    static void classInit(t_class *c, t_symbol *nameSpace, const char *classname)
    {
        addMethod<Wrapper<T>, &Wrapper<T>::sync>(c, "sync");
        addMethod<Wrapper<T>, &Wrapper<T>::anything>(c, "anything");
        addMethod<Wrapper<T>, &Wrapper<T>::subpatcher>(c, "subpatcher");
        addMethod<Wrapper<T>, &Wrapper<T>::assist>(c, "assist");
        
        // N.B. MUST add signal handling after dspInit to override the builtin responses
        
        dspInit(c);
        addMethod<Wrapper<T>, &Wrapper<T>::anything>(c, "signal");
        
        // Make sure the mutator class exists
        
        const char mutatorClassName[] = "__fl.signal.mutator";
                
        if (!class_findbyname(CLASS_NOBOX, gensym(mutatorClassName)))
            Mutator::makeClass<Mutator, &mutatorClass>(CLASS_NOBOX, mutatorClassName);
    }
    
private:
    
    // Objects (need freeing except the internal object which is owned by the patch)
    
    t_object *mPatch;
    t_object *mObject;
    t_object *mMutator;
    
    // Inlets (must be freed)
    
    std::vector <t_object *> mInOutlets;
    std::vector <t_object *> mProxyIns;
    t_object *mSyncIn;
    
    // Outlets (don't need to free - only the arrays need freeing)
    
    std::vector <t_object *> mAudioOuts;
    std::vector <t_object *> mOuts;
    
    // Sync Check
    
    SyncCheck mSyncChecker;
    
    // Dummy for stuffloc on proxies
    
    long mProxyNum;
};

//////////////////////////////////////////////////////////////////////////
////////////////// Max Object Class for Synchronisation //////////////////
//////////////////////////////////////////////////////////////////////////

template <class T> class FrameLib_MaxObj : public MaxBase
{
    // Connection Mode Enum
    
    enum ConnectMode { kConnect, kConfirm, kDoubleCheck };
    
    struct ConnectionInfo
    {
        ConnectionInfo(FrameLib_MaxObj *object, unsigned long index, t_object *topLevelPatch, ConnectMode mode) :
        mObject(object), mIndex(index), mTopLevelPatch(topLevelPatch), mMode(mode) {}
        
        FrameLib_MaxObj *mObject;
        unsigned long mIndex;
        t_object *mTopLevelPatch;
        ConnectMode mMode;
        
    };

    struct Input
    {
        Input() :
        mProxy(NULL), mObject(NULL), mIndex(0), mReportError(false) {}
        
        Input(t_object *proxy, FrameLib_MaxObj *object, unsigned long index, bool reportError) :
        mProxy(proxy), mObject(object), mIndex(index), mReportError(reportError) {}
        
        t_object *mProxy;
        FrameLib_MaxObj *mObject;
        unsigned long mIndex;
        bool mReportError;
    };
    
private:
    
    // Globals
    
    FrameLib_Global **globalHandle()
    {
        return (FrameLib_Global **) &gensym("__FrameLib__Global__")->s_thing;
    }
    
    FrameLib_Context getContext()
    {
        mTopLevelPatch = jpatcher_get_toppatcher(gensym("#P")->s_thing);
        
        return FrameLib_Context(FrameLib_Global::get(globalHandle()), mTopLevelPatch);
    }

    ConnectionInfo **frameConnectionInfo() { return (ConnectionInfo **) &gensym("__frame__connection__info__")->s_thing; }

    // Call to get the context increments the global counter, so it needs relasing when we are done
    
    void releaseGlobal()
    {
        FrameLib_Global::release(globalHandle());
    }

    // Parameter Parsing
    
    bool isParameterTag(t_symbol *sym)
    {
        return (sym && sym->s_name[0] == '#' && strlen(sym->s_name) > 1);
    }

    bool isInputTag(t_symbol *sym)
    {
        return (sym && sym->s_name[0] == '/' && strlen(sym->s_name) > 1);
    }
    
    bool isTag(t_atom *a)
    {
        t_symbol *sym = atom_getsym(a);
        return isParameterTag(sym) || isInputTag(sym);
    }
    
    long parseNumericalList(std::vector<double> &values, t_atom *argv, long argc, long idx)
    {
        values.resize(0);
        
        // Collect doubles
        
        for ( ; idx < argc; idx++)
        {
            if (isTag(argv + idx))
                break;
            
            if (atom_gettype(argv + idx) == A_SYM)
                object_error(mUserObject, "string %s in entry list where value expected", atom_getsym(argv + idx)->s_name);
            
            values.push_back(atom_getfloat(argv + idx));
        }
        
        return idx;
    }
    
    void parseParameters(FrameLib_Parameters::AutoSerial& serialisedParameters, long argc, t_atom *argv)
    {
        t_symbol *sym;
        std::vector<double> values;
        long i, j;
       
        // Parse arguments
        
        for (i = 0; i < argc; i++)
        {
            if (isTag(argv + i))
                break;
            
#ifndef OBJECT_ARGS_SET_ALL_INPUTS
            
            char argNames[64];
            
            sprintf(argNames, "%ld", i);
            sym = atom_getsym(argv + i);

            if (sym != gensym(""))
                serialisedParameters.write(argNames, sym->s_name);
            else
            {
                double value = atom_getfloat(argv + i);
                serialisedParameters.write(argNames, &value, 1);
            }
#endif
        }
        
        // Parse parameters
        
        while (i < argc)
        {
            // Strip stray items
            
            for (j = 0; i < argc; i++, j++)
            {
                if (isTag(argv + i))
                {
                    sym = atom_getsym(argv + i);
                    break;
                }
            
                if (j == 0)
                    object_error(mUserObject, "stray items after entry %s", sym->s_name);
            }
            
            // Check for lack of values or end of list
            
            if ((++i >= argc) || isTag(argv + i))
            {
                if (i < (argc + 1))
                    object_error(mUserObject, "no values given for entry %s", sym->s_name);
                continue;
            }
            
            if (isParameterTag(sym))
            {
                // Do strings or values
                
                if (atom_getsym(argv + i) != gensym(""))
                    serialisedParameters.write(sym->s_name + 1, atom_getsym(argv + i++)->s_name);
                else
                {
                    i = parseNumericalList(values, argv, argc, i);
                    serialisedParameters.write(sym->s_name + 1, &values[0], values.size());
                }
            }
        }
    }

    // Input Parsing
    
    unsigned long inputNumber(t_symbol *sym)
    {
        return atoi(sym->s_name + 1) - 1;
    }

    void parseInputs(long argc, t_atom *argv)
    {
        std::vector<double> values;
        
        // Parse arguments if used to set inputs
        
#ifdef OBJECT_ARGS_SET_ALL_INPUTS
        long i = parseNumericalList(values, argv, argc, 0);
        
        for (unsigned long j = 0; i && j < mObject->getNumIns(); j++)
            mObject->setFixedInput(j, &values[0], values.size());
#else 
        long i = 0;
#endif

        // Parse tags
        
        while (i < argc)
        {
            // Advance to next input tag
            
            for ( ; i < argc && !isInputTag(atom_getsym(argv + i)); i++);
            
            // If there are values to read then do so
                
            if ((i + 1) < argc && !isTag(argv + i + 1))
            {
                t_symbol *sym = atom_getsym(argv + i);
                i = parseNumericalList(values, argv, argc, i + 1);
                mObject->setFixedInput(inputNumber(sym), &values[0], values.size());
            }
        }
    }
    
    // IO Helpers

public:

    long getNumAudioIns()
    {
        return (long) mObject->getNumAudioIns() + (T::handlesAudio() ? 1 : 0);
    }

    long getNumAudioOuts()
    {
        return (long) mObject->getNumAudioOuts() + (T::handlesAudio() ? 1 : 0);
    }

    long getNumIns()
    {
        return (long) mObject->getNumIns();
    }

    long getNumOuts()
    {
        return (long) mObject->getNumOuts();
    }

    // Constructor and Destructor
    
public:

    FrameLib_MaxObj(t_symbol *s, long argc, t_atom *argv) : mConfirmIndex(-1), mConfirm(false)
    {
        // Init
        
        FrameLib_Parameters::AutoSerial serialisedParameters;
        FrameLib_Context context = getContext();
        mUserObject = (t_object *)this;

        // Object creation with parameters and arguments
    
        parseParameters(serialisedParameters, argc, argv);

        mObject = new T(context, &serialisedParameters, this);
        
        parseInputs(argc, argv);
        
        mInputs.resize(getNumIns());
        mOutputs.resize(getNumOuts());
        
        for (long i = getNumIns() - 1; i >= 0; i--)
        {
            // N.B. - we create a proxy if the inlet is not the first inlet (not the first frame input or the object handles audio)
            
            t_object *proxy = (t_object *) ((i || T::handlesAudio()) ? proxy_new(this, getNumAudioIns() + i, &mProxyNum) : NULL);
            
            mInputs[i] = Input(proxy, NULL, 0, true);
        }
        
        for (unsigned long i = getNumOuts(); i > 0; i--)
            mOutputs[i - 1] = outlet_new(this, NULL);
        
        // Setup for audio, even if the object doesn't handle it, so that dsp recompile works correctly
        
        dspSetup(getNumAudioIns());
 
        for (unsigned long i = 0; i < getNumAudioOuts(); i++)
            outlet_new(this, "signal");
    }

    ~FrameLib_MaxObj()
    {
        dspFree();

        for (typename std::vector <Input>::iterator it = mInputs.begin(); it != mInputs.end(); it++)
            object_free(it->mProxy);

        delete mObject;
        
        releaseGlobal();
    }

    void assist (void *b, long m, long a, char *s)
    {
        if (m == ASSIST_OUTLET)
        {
            sprintf(s,"(signal) Output %ld", a + (T::handlesAudio() ? 0 : 1));
        }
        else
        {
            sprintf(s,"(signal) Input %ld", a + (T::handlesAudio() ? 0 : 1));
        }
    }

    // Perform and DSP

    void perform(t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
    {
        // N.B. Plus one due to sync inputs
        
        mObject->blockUpdate(ins + 1, outs + 1, vec_size);
    }

    void dsp(t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
    {
        // Check / make connections
        
        connections();

        // Set sampling rate
        
        mObject->setSamplingRate(samplerate);
        
        // Add a perform routine to the chain if the object handles audio
        
        if (T::handlesAudio())
            addPerform<FrameLib_MaxObj, &FrameLib_MaxObj<T>::perform>(dsp64);
    }

    // Connection Routines
    
    ConnectionInfo* getConnectionInfo()
    {
        return *frameConnectionInfo();
    }
    
    void setConnectionInfo(ConnectionInfo *info = NULL)
    {
        *frameConnectionInfo() = info;
    }

    void checkConnection(FrameLib_MaxObj *x, unsigned long index, ConnectMode mode)
    {
        object_method(x, gensym("internal_connect"), index, mode);
    }
    
    static void connectionCall(FrameLib_MaxObj *x, unsigned long index, ConnectMode mode)
    {
        x->connect(index, mode);
    }
    
    void connect(unsigned long index, ConnectMode mode)
    {
        ConnectionInfo info(this, index, mTopLevelPatch, mode);
        
        setConnectionInfo(&info);
        outlet_anything(mOutputs[index], gensym("frame"), 0, NULL);
        setConnectionInfo();
    }

    void connections()
    {
        // Check input connections
        
        for (unsigned long i = 0; i < getNumIns(); i++)
        {
            mInputs[i].mReportError = true;
            
            if (mInputs[i].mObject)
            {
                mConfirm = false;
                mConfirmIndex = i;
                
                if (mObject->isConnected(i))
                    checkConnection(mInputs[i].mObject, mInputs[i].mIndex, kConfirm);
                
                if (!mConfirm)
                {
                    // Object is no longer connected as before
                    
                    mInputs[i].mObject = NULL;
                    mInputs[i].mIndex = 0;
                    
                    if (mObject->isConnected(i))
                        mObject->deleteConnection(i);
                }
                
                mConfirm = false;
                mConfirmIndex = -1;
            }
        }
        
        // Make output connections
        
        for (unsigned long i = getNumOuts(); i > 0; i--)
            connect(i - 1, kConnect);
        
        // Reset DSP
        
        mObject->reset();
    }

    void frame()
    {
        ConnectionInfo *info = getConnectionInfo();
        long index = getInlet() - getNumAudioIns();
        
        if (!info || index < 0)
            return;
        
        switch (info->mMode)
        {
            case kConnect:
            {
                bool connectionChange = (mInputs[index].mObject != info->mObject || mInputs[index].mIndex != info->mIndex);
                bool valid = (info->mTopLevelPatch == mTopLevelPatch && info->mObject != this);
                
                // Confirm that the object is valid
                
                if (!valid)
                {
                    if (info->mObject == this)
                        object_error(mUserObject, "direct feedback loop detected");
                    else
                        object_error(mUserObject, "cannot connect objects from different top level patchers");
                }
                
                // Check for double connection *only* if the internal object is connected (otherwise the previously connected object has been deleted)
                
                if (mInputs[index].mObject && mInputs[index].mReportError && connectionChange && mObject->isConnected(index))
                {
                    mConfirmIndex = index;
                    checkConnection(mInputs[index].mObject, mInputs[index].mIndex, kDoubleCheck);
                    mConfirmIndex = -1;
                    mInputs[index].mReportError = false;
                }
                
                // Always change the connection if the new object is valid (only way to ensure new connections work)
                
                if (connectionChange && valid)
                {
                    mInputs[index].mObject = info->mObject;
                    mInputs[index].mIndex = info->mIndex;
                    
                    mObject->addConnection(info->mObject->mObject, info->mIndex, index);
                }
            }
                break;
                
            case kConfirm:
                if (index == mConfirmIndex && mInputs[index].mObject == info->mObject && mInputs[index].mIndex == info->mIndex)
                    mConfirm = true;
                break;
                
            case kDoubleCheck:
                if (index == mConfirmIndex && mInputs[index].mObject == info->mObject && mInputs[index].mIndex == info->mIndex)
                    object_error(mUserObject, "extra connection to input %ld", index + 1);
                break;
        }
    }

    void sync()
    {
        if (mSyncChecker())
            for (unsigned long i = getNumOuts(); i > 0; i--)
                outlet_anything(mOutputs[i - 1], gensym("sync"), 0, NULL);
    }
    
    // Class Initilisation
    
    template <class U> static void makeClass(t_symbol *nameSpace, const char *className)
    {
        // If handles audio/scheduler then make wrapper class and name the inner object differently..
        
        char internalClassName[256];
        
        if (T::handlesAudio())
        {
            Wrapper<U>:: template makeClass<Wrapper<U>, &wrapperClass>(CLASS_BOX, className);
            sprintf(internalClassName, "unsynced.%s", className);
        }
        else
            strcpy(internalClassName, className);
        
        MaxBase::makeClass<U, &objectClass>(nameSpace, internalClassName);
    }
    
    static void classInit(t_class *c, t_symbol *nameSpace, const char *classname)
    {
        addMethod(c, (method) &connectionCall, "internal_connect");
        addMethod<FrameLib_MaxObj<T>, &FrameLib_MaxObj<T>::assist>(c, "assist");
        addMethod<FrameLib_MaxObj<T>, &FrameLib_MaxObj<T>::frame>(c, "frame");
        addMethod<FrameLib_MaxObj<T>, &FrameLib_MaxObj<T>::sync>(c, "sync");
        addMethod<FrameLib_MaxObj<T>, &FrameLib_MaxObj<T>::dsp>(c);
        
        dspInit(c);
    }

private:
    
    FrameLib_MultiChannel *mObject;
    
    std::vector <Input> mInputs;
    std::vector <void *> mOutputs;

    long mProxyNum;
    long mConfirmIndex;
    bool mConfirm;
    
    t_object *mTopLevelPatch;
    
     SyncCheck mSyncChecker;
    
public:
    
    t_object *mUserObject;
};

#ifndef CUSTOM_OBJECT
extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxObj<OBJECT_CLASS>::makeClass<FrameLib_MaxObj<OBJECT_CLASS> >(CLASS_BOX, OBJECT_NAME);
}
#endif
