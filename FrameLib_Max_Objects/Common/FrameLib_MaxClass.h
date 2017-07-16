
#include "MaxClass_Base.h"

#include "FrameLib_Multichannel.h"
#include "FrameLib_DSP.h"
#include "FrameLib_Global.h"
#include "FrameLib_Context.h"

#include <vector>

// FIX - improve reporting of extra connections + look into feedback detection...
// FIX - think about adding assist helpers for this later...

//////////////////////////////////////////////////////////////////////////
///////////////////////////// Sync Check Class ///////////////////////////
//////////////////////////////////////////////////////////////////////////

class SyncCheck
{
    enum TraversalMode {kDownOnly, kDown, kAcross};
    
public:
    
    enum SyncMode { kSyncComplete, kSync, kAttachAndSync };

    SyncCheck() : mObject(NULL), mTime(-1), mMode(kDownOnly) {}
    
    SyncMode operator()(void *object, bool handlesAudio, bool isOutput)
    {
        SyncCheck *info = *syncInfo();
        
        if (info && (info->mTime != mTime || info->mObject != mObject))
        {
            *this = SyncCheck(info->mObject, info->mTime, info->mMode);
            return handlesAudio && object != mObject && (mMode != kAcross ||isOutput) ? kAttachAndSync : kSync;
        }
        else if (info && mMode == kAcross && info->mMode == kDown)
        {
            mMode = kDown;
            return handlesAudio && object != mObject && !isOutput ? kAttachAndSync : kSync;
        }

        return kSyncComplete;
    }
    
    void syncSet(void *object = NULL, long time = -1)
    {
        *this = SyncCheck(object, time, object && object_method(object, gensym("is_output")) ? kDownOnly : kDown);
        *syncInfo() = (object ? this : NULL);
    }

    bool startAcross()    { return setMode(*syncInfo(), kAcross); }
    void restoreMode()    { setMode(*syncInfo(), mMode); }
    
private:
    
    SyncCheck(void *object, long time, TraversalMode mode) : mObject(object), mTime(time), mMode(mode) {}

    SyncCheck **syncInfo()              { return (SyncCheck **) &gensym("__FrameLib__SYNC__")->s_thing; }
    bool setMode(SyncCheck *info, TraversalMode mode) { return info && info->mMode != kDownOnly && ((info->mMode = mode) == mode); }
    
    void *mObject;
    long mTime;
    TraversalMode mMode;
};

//////////////////////////////////////////////////////////////////////////
////////////////////// Mutator for Synchronisation ///////////////////////
//////////////////////////////////////////////////////////////////////////

class Mutator : public MaxClass_Base
{
    
public:
    
    Mutator(t_symbol *sym, long ac, t_atom *av) : mObject(ac ? atom_getobj(av) : NULL) {}
    
    static void classInit(t_class *c, t_symbol *nameSpace, const char *classname)
    {
        addMethod<Mutator, &Mutator::mutate>(c, "signal");
    }
    
    void mutate(t_symbol *sym, long ac, t_atom *av)
    {
        mSyncChecker.syncSet(mObject, gettime());
        object_method(mObject, gensym("sync"));
        mSyncChecker.syncSet();
    }
    
private:
    
    SyncCheck mSyncChecker;    
    void *mObject;
};

//////////////////////////////////////////////////////////////////////////
////////////////////// Wrapper for Synchronisation ///////////////////////
//////////////////////////////////////////////////////////////////////////

template <class T> class Wrapper : public MaxClass_Base
{
    
public:
    
    // Initialise Class
    
    static void classInit(t_class *c, t_symbol *nameSpace, const char *classname)
    {
        addMethod<Wrapper<T>, &Wrapper<T>::anything>(c, "anything");
        addMethod<Wrapper<T>, &Wrapper<T>::subpatcher>(c, "subpatcher");
        addMethod<Wrapper<T>, &Wrapper<T>::assist>(c, "assist");
        addMethod<Wrapper<T>, &Wrapper<T>::sync>(c, "sync");
        addMethod(c, (method) &externalPatchLineUpdate, "patchlineupdate");
        addMethod(c, (method) &externalConnectionAccept, "connectionaccept");
        addMethod(c, (method) &externalWrapperInternalObject, "wrapper_internal_object");
        
        // N.B. MUST add signal handling after dspInit to override the builtin responses
        
        dspInit(c);
        addMethod<Wrapper<T>, &Wrapper<T>::anything>(c, "signal");
        
        // Make sure the mutator class exists
        
        const char mutatorClassName[] = "__fl.signal.mutator";
        
        if (!class_findbyname(CLASS_NOBOX, gensym(mutatorClassName)))
            Mutator::makeClass<Mutator>(CLASS_NOBOX, mutatorClassName);
    }

    // Constructor and Destructor
    
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
        
        t_object *box = NULL;
        const char *text = NULL;
        
        object_obex_lookup(this, gensym("#B"), &box);
        t_object * textfield = jbox_get_textfield(box);
        
        std::string newObjectText = accessClassName<Wrapper>()->c_str();
        
        // Get box text and strip object name from the top (relace with stored name in case the object name is an alias)
        
        if (textfield)
        {
            text = (char *)object_method(textfield, gensym("getptr"));
            text = strchr(text, ' ');
            
            if (text)
                newObjectText += text;
        }
        
        // Make Internal Object

        mObject = jbox_get_object((t_object *) newobject_sprintf(mPatch, "@maxclass newobj @text \"unsynced.%s\" @patching_rect 0 0 30 10", newObjectText.c_str()));
        
        // Make Mutator (with argument referencing the internal object)
        
        atom_setobj(&a, mObject);
        mMutator = (t_object *) object_new_typed(CLASS_NOBOX, gensym("__fl.signal.mutator"), 1, &a);
        
        // Free resources we no longer need
    
        freeobject((t_object *)d);
        
        // Get the object itself (typed)
        
        T *internal = internalObject();
        
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
        
        // Connect first signal outlet to the mutator
        
        outlet_add(outlet_nth(mObject, 0), inlet_nth(mMutator, 0));
        
        // Connect inlets (all types)
        
        for (long i = 0; i < numAudioIns + numIns - 1; i++)
            outlet_add(mInOutlets[i], inlet_nth(mObject, i + 1));
        
        // Connect other outlets (audio then frame and sync message outlets)
        
        for (long i = 0; i < numAudioOuts - 1; i++)
            outlet_add(outlet_nth(mObject, i + 1), mAudioOuts[i]);
        
        for (long i = 0; i < numOuts; i++)
            outlet_add(outlet_nth(mObject, i + numAudioOuts), mOuts[i]);
    }
    
    ~Wrapper()
    {
        // Delete ins and proxies
        
        for (std::vector <t_object *>::iterator it = mProxyIns.begin(); it != mProxyIns.end(); it++)
            object_free(*it);
        
        for (std::vector <t_object *>::iterator it = mInOutlets.begin(); it != mInOutlets.end(); it++)
            object_free(*it);
        
        // Free objects - N.B. - free the patch, but not the object within it (which will be freed by deleting the patch)
        
        object_free(mMutator);
        object_free(mPatch);
    }
    
    // Standard methods
    
    void assist(void *b, long m, long a, char *s)
    {
        internalObject()->assist(b, m, a + 1, s);
    }
    
    void *subpatcher(long index, void *arg)
    {
        if ((t_ptr_uint) arg <= 1 || NOGOOD(arg))
            return NULL;
        
        return (index == 0) ? (void *) mPatch : NULL;
    }
    
    void anything(t_symbol *sym, long ac, t_atom *av)
    {
        long inlet = getInlet();
        
        outlet_anything(mInOutlets[inlet], sym, ac, av);
    }
    
    void sync()
    {
        internalObject()->sync();
    }
    
    // External methods (A_CANT)
    
    static t_max_err externalPatchLineUpdate(Wrapper *x, t_object *patchline, long updatetype, t_object *src, long srcout, t_object *dst, long dstin)
    {
        // Only handle destinations and account for internal sync connections
        
        if ((t_object *) x == dst)
            return T::externalPatchLineUpdate(x->internalObject(), patchline, updatetype, src, srcout, x->mObject, dstin + 1);
        
        return MAX_ERR_NONE;
    }
    
    static long externalConnectionAccept(Wrapper *src, t_object *dst, long srcout, long dstin, t_object *outlet, t_object *inlet)
    {
        // Only handle sources and account for internal sync connections

        return T::externalConnectionAccept(src->internalObject(), dst, srcout + 1, dstin, outlet, inlet);
    }
    
    static void *externalWrapperInternalObject(Wrapper *x)
    {
        return x->mObject;
    }
    
private:
    
    T *internalObject() { return (T *) mObject; }
    
    // Objects (need freeing except the internal object which is owned by the patch)
    
    t_object *mPatch;
    t_object *mObject;
    t_object *mMutator;
    
    // Inlets (must be freed)
    
    std::vector <t_object *> mInOutlets;
    std::vector <t_object *> mProxyIns;
    
    // Outlets (don't need to free)
    
    std::vector <t_object *> mAudioOuts;
    std::vector <t_object *> mOuts;
    
    // Dummy for stuffloc on proxies
    
    long mProxyNum;
};

//////////////////////////////////////////////////////////////////////////
/////////////////////// FrameLib Max Object Class ////////////////////////
//////////////////////////////////////////////////////////////////////////

template <class T, bool argsSetAllInputs = false> class FrameLib_MaxClass : public MaxClass_Base
{
    // Connection Mode Enum
    
    enum ConnectMode { kConnect, kConfirm, kDoubleCheck };
    
    struct ConnectionInfo
    {
        ConnectionInfo(t_object *object, unsigned long index, t_object *topLevelPatch, ConnectMode mode) :
        mObject(object), mIndex(index), mTopLevelPatch(topLevelPatch), mMode(mode) {}
        
        t_object *mObject;
        unsigned long mIndex;
        t_object *mTopLevelPatch;
        ConnectMode mMode;
        
    };

    struct Input
    {
        Input() : mProxy(NULL), mObject(NULL), mIndex(0) {}
        Input(t_object *proxy, t_object *object, unsigned long index) : mProxy(proxy), mObject(object), mIndex(index) {}
        
        t_object *mProxy;
        t_object *mObject;
        unsigned long mIndex;
    };
    
public:
    
    // Class Initialisation (must explicitly give U for classes that inherit from FrameLib_MaxClass<>)
    
    template <class U = FrameLib_MaxClass<T, argsSetAllInputs> > static void makeClass(t_symbol *nameSpace, const char *className)
    {
        // If handles audio/scheduler then make wrapper class and name the inner object differently..
        
        std::string internalClassName = className;
        
        if (T::handlesAudio())
        {
            Wrapper<U>:: template makeClass<Wrapper<U> >(CLASS_BOX, className);
            internalClassName.insert(0, "unsynced.");
        }
        
        MaxClass_Base::makeClass<U>(nameSpace, internalClassName.c_str());
    }
    
    static void classInit(t_class *c, t_symbol *nameSpace, const char *classname)
    {
        addMethod(c, (method) &externalPatchLineUpdate, "patchlineupdate");
        addMethod(c, (method) &externalConnectionAccept, "connectionaccept");
        addMethod(c, (method) &externalConnectionConfirm, "connection_confirm");
        addMethod(c, (method) &externalGetInternalObject, "get_internal_object");
        addMethod(c, (method) &externalIsOutput, "is_output");
        addMethod(c, (method) &externalIsInput, "is_input");
        addMethod(c, (method) &externalGetNumAudioIns, "get_num_audio_ins");
        addMethod(c, (method) &externalGetNumAudioOuts, "get_num_audio_outs");
        addMethod<FrameLib_MaxClass<T>, &FrameLib_MaxClass<T>::assist>(c, "assist");
        addMethod<FrameLib_MaxClass<T>, &FrameLib_MaxClass<T>::frame>(c, "frame");
        addMethod<FrameLib_MaxClass<T>, &FrameLib_MaxClass<T>::sync>(c, "sync");
        addMethod<FrameLib_MaxClass<T>, &FrameLib_MaxClass<T>::dsp>(c);
        
        dspInit(c);
    }

    // Constructor and Destructor
    
    FrameLib_MaxClass(t_symbol *s, long argc, t_atom *argv) : mConfirmIndex(-1), mConfirm(false), mSyncIn(NULL)
    {
        // Init
        
        FrameLib_Parameters::AutoSerial serialisedParameters;
        FrameLib_Context context = getContext();
        mUserObject = (t_object *)this;

        // Object creation with parameters and arguments (N.B. the object is not a member due to size restrictions)
    
        parseParameters(serialisedParameters, argc, argv);

        mObject = new T(context, &serialisedParameters, this);
        
        parseInputs(argc, argv);
        
        mInputs.resize(getNumIns());
        mOutputs.resize(getNumOuts());
        
        for (long i = getNumIns() - 1; i >= 0; i--)
        {
            // N.B. - we create a proxy if the inlet is not the first inlet (not the first frame input or the object handles audio)
            
            t_object *proxy = (t_object *) ((i || T::handlesAudio()) ? proxy_new(this, getNumAudioIns() + i, &mProxyNum) : NULL);
            
            mInputs[i] = Input(proxy, NULL, 0);
        }
        
        for (unsigned long i = getNumOuts(); i > 0; i--)
            mOutputs[i - 1] = outlet_new(this, NULL);
        
        // Setup for audio, even if the object doesn't handle it, so that dsp recompile works correctly
        
        dspSetup(getNumAudioIns());
 
        for (unsigned long i = 0; i < getNumAudioOuts(); i++)
            outlet_new(this, "signal");
        
        // Add a sync outlet if we need to handle audio
        
        if (T::handlesAudio())
        {
            mSyncIn = (t_object *) outlet_new(NULL, NULL);
            outlet_add(mSyncIn, inlet_nth(*this, 0));
        }
    }

    ~FrameLib_MaxClass()
    {
        dspFree();

        for (typename std::vector <Input>::iterator it = mInputs.begin(); it != mInputs.end(); it++)
            object_free(it->mProxy);

        delete mObject;
        
        object_free(mSyncIn);

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

    // IO Helpers
    
    long getNumAudioIns()   { return (long) mObject->getNumAudioIns() + (T::handlesAudio() ? 1 : 0); }
    long getNumAudioOuts()  { return (long) mObject->getNumAudioOuts() + (T::handlesAudio() ? 1 : 0); }
    long getNumIns()        { return (long) mObject->getNumIns(); }
    long getNumOuts()       { return (long) mObject->getNumOuts(); }
    
    // Perform and DSP

    void perform(t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
    {
        // N.B. Plus one due to sync inputs
        
        mObject->blockUpdate(ins + 1, outs + 1, vec_size);
    }

    void dsp(t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
    {
        // Set sampling rate
        
        mObject->setSamplingRate(samplerate);
        
        // Confirm input connections
        
        for (unsigned long i = 0; i < getNumIns(); i++)
            confirmConnection(i, kConfirm);
        
        // Make output connections
        
        for (unsigned long i = getNumOuts(); i > 0; i--)
            makeConnection(i - 1, kConnect);
        
        // Reset DSP
        
        mObject->reset();
        
        // Add a perform routine to the chain if the object handles audio
        
        if (T::handlesAudio())
            addPerform<FrameLib_MaxClass, &FrameLib_MaxClass<T>::perform>(dsp64);
    }

    // Audio Synchronisation
    
    void sync()
    {
        SyncCheck::SyncMode mode = mSyncChecker(this, T::handlesAudio(), externalIsOutput(this));
        
        if (mode == SyncCheck::kAttachAndSync)
            outlet_anything(mSyncIn, gensym("signal"), 0, NULL);
        
        if (mode != SyncCheck::kSyncComplete)
        {
            for (unsigned long i = getNumOuts(); i > 0; i--)
                outlet_anything(mOutputs[i - 1], gensym("sync"), 0, NULL);
            
            if (mSyncChecker.startAcross())
            {
                for (unsigned long i = 0; i < getNumIns(); i++)
                    if (mInputs[i].mObject)
                        object_method(mInputs[i].mObject, gensym("sync"));
                mSyncChecker.restoreMode();
            }
        }
    }
    
    // Connection Routines
    
    ConnectionInfo* getConnectionInfo()                     { return *frameConnectionInfo(); }
    void setConnectionInfo(ConnectionInfo *info = NULL)     { *frameConnectionInfo() = info; }
   
    void frame()
    {
        ConnectionInfo *info = getConnectionInfo();
        long index = getInlet() - getNumAudioIns();
        
        if (!info)
            return;
        
        switch (info->mMode)
        {
            // FIX - need better confirmation here that things are working correctly...
                
            case kConnect:

                if (info->mObject == *this)
                {
                    object_error(mUserObject, "direct feedback loop detected");
                    return;
                }
                
                if (info->mTopLevelPatch != mTopLevelPatch)
                {
                    object_error(mUserObject, "cannot connect objects from different top level patchers");
                    return;
                }
                
                connect(info->mObject, info->mIndex, index);
                break;
                
            case kConfirm:
            case kDoubleCheck:

                if (index == mConfirmIndex && mInputs[index].mObject == info->mObject && mInputs[index].mIndex == info->mIndex)
                {
                    mConfirm = true;
                    if (info->mMode == kDoubleCheck)
                        object_error(mUserObject, "extra connection to input %ld", index + 1);
                }
                break;
        }
    }

    // External methods (A_CANT)
    
    static void externalConnectionConfirm(FrameLib_MaxClass *x, unsigned long index, ConnectMode mode)
    {
        x->makeConnection(index, mode);
    }
    
    static FrameLib_MultiChannel *externalGetInternalObject(FrameLib_MaxClass *x)
    {
        return x->mObject;
    }
    
    static bool externalIsInput(FrameLib_MaxClass *x)
    {
        return T::handlesAudio() && (x->getNumAudioOuts() == 1);
    }
    
    static bool externalIsOutput(FrameLib_MaxClass *x)
    {
        return T::handlesAudio() && (x->getNumAudioOuts() > 1);
    }
    
    static long externalGetNumAudioIns(FrameLib_MaxClass *x)
    {
        return x->getNumAudioIns();
    }
    
    static long externalGetNumAudioOuts(FrameLib_MaxClass *x)
    {
        return x->getNumAudioOuts();
    }
    
    static long externalConnectionAccept(FrameLib_MaxClass *src, t_object *dst, long srcout, long dstin, t_object *outlet, t_object *inlet)
    {
        return src->connectionAccept(dst, srcout, dstin, outlet, inlet);
    }
    
    static t_max_err externalPatchLineUpdate(FrameLib_MaxClass *x, t_object *patchline, long updatetype, t_object *src, long srcout, t_object *dst, long dstin)
    {
        return x->patchLineUpdate(patchline, updatetype, src, srcout, dst, dstin);
    }

private:
    
    // Globals
    
    FrameLib_Global **globalHandle()        { return (FrameLib_Global **) &gensym("__FrameLib__Global__")->s_thing; }
    ConnectionInfo **frameConnectionInfo()  { return (ConnectionInfo **) &gensym("__frame__connection__info__")->s_thing; }

    FrameLib_Context getContext()
    {
        mTopLevelPatch = jpatcher_get_toppatcher(gensym("#P")->s_thing);
        
        return FrameLib_Context(FrameLib_Global::get(globalHandle()), mTopLevelPatch);
    }
    
    // Call to get the context increments the global counter, so it needs relasing when we are done
    
    void releaseGlobal()                    { FrameLib_Global::release(globalHandle()); }
    
    // Unwrapping Connections
    
    void unwrapConnection(t_object *& object, long& connection)
    {
        t_object *wrapped = (t_object *) object_method(object, gensym("wrapper_internal_object"));
        
        if (wrapped)
        {
            object = wrapped;
            connection++;
        }
    }
    
    // Get an internal object from a generic pointer safely
    
    FrameLib_MultiChannel *getInternalObject(t_object *x)
    {
        return (FrameLib_MultiChannel *) object_method(x, gensym("get_internal_object"));
    }
    
    // Private connection methods
    
    void makeConnection(unsigned long index, ConnectMode mode)
    {
        ConnectionInfo info(*this, index, mTopLevelPatch, mode);
        
        setConnectionInfo(&info);
        outlet_anything(mOutputs[index], gensym("frame"), 0, NULL);
        setConnectionInfo();
    }
    
    bool confirmConnection(unsigned long inputIndex, ConnectMode mode)
    {
        mConfirm = false;
        mConfirmIndex = inputIndex;
        
        // Check for connection *only* if the internal object is connected (otherwise assume the previously connected object has been deleted)
        
        if (mInputs[inputIndex].mObject && mObject->isConnected(inputIndex))
        {
            object_method(mInputs[inputIndex].mObject, gensym("connection_confirm"), mInputs[inputIndex].mIndex, mode);
            
            if (!mConfirm)
                disconnect(mInputs[inputIndex].mObject, mInputs[inputIndex].mIndex, inputIndex);
        }
        
        return mConfirm;
    }
    
    bool inRange(long index, long numFrameConnectors) { return index >= 0 && index < numFrameConnectors; }
    
    void connect(t_object *src, long outIdx, long inIdx)
    {
        FrameLib_MultiChannel *internalObject = getInternalObject(src);
        bool connectionChange = (mInputs[inIdx].mObject != src || mInputs[inIdx].mIndex != outIdx);
        
        if (!inRange(inIdx, getNumIns()) || !internalObject || !inRange(outIdx, internalObject->getNumOuts()) || !connectionChange || (connectionChange && confirmConnection(inIdx, kDoubleCheck)))
            return;
        
        // Change the connection if needed
        
        mInputs[inIdx].mObject = src;
        mInputs[inIdx].mIndex = outIdx;
        
        mObject->addConnection(internalObject, outIdx, inIdx);

        dspchain_setbroken(dspchain_fromobject(*this));
    }
    
    void disconnect(t_object *src, long outIdx, long inIdx)
    {
        if (!inRange(inIdx, getNumIns()) || !mInputs[inIdx].mObject || mInputs[inIdx].mObject != src || mInputs[inIdx].mIndex != outIdx)
            return;
        
        mInputs[inIdx].mObject = NULL;
        mInputs[inIdx].mIndex = 0;
        
        if (mObject->isConnected(inIdx))
            mObject->deleteConnection(inIdx);
        
        dspchain_setbroken(dspchain_fromobject(*this));
    }

    // Patchline connections
    
    t_max_err patchLineUpdate(t_object *patchline, long updatetype, t_object *src, long srcout, t_object *dst, long dstin)
    {
        if (*this == dst)
        {
            unwrapConnection(src, srcout);
            
            if (getInternalObject(src))
            {
                srcout -= (long) object_method(src, gensym("get_num_audio_outs"));
                dstin -= getNumAudioIns();
                
                switch (updatetype)
                {
                    case JPATCHLINE_CONNECT:        connect(src, srcout, dstin);        break;
                    case JPATCHLINE_DISCONNECT:     disconnect(src, srcout, dstin);     break;
                    case JPATCHLINE_ORDER:                                              break;
                }
            }
        }
        
        return MAX_ERR_NONE;
    }
    
    long connectionAccept(t_object *dst, long srcout, long dstin, t_object *outlet, t_object *inlet)
    {
        if (!inRange(srcout - getNumAudioOuts(), getNumOuts()))
            return 1;
        
        unwrapConnection(dst, dstin);
        FrameLib_MultiChannel *internal = getInternalObject(dst);
        
        if (!internal || !inRange(dstin - (long) object_method(dst, gensym("get_num_audio_ins")), internal->getNumIns()))
            return 0;
        
        // FIX - make wrapper work, and also make inlets/outlets work...
        
        return 1;
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
            
            if (!argsSetAllInputs)
            {
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
            }
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
        long i = 0;
        
        // Parse arguments if used to set inputs
        
        if (argsSetAllInputs)
        {
            i = parseNumericalList(values, argv, argc, 0);
            
            for (unsigned long j = 0; i && j < mObject->getNumIns(); j++)
                mObject->setFixedInput(j, &values[0], values.size());
        }
        
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

    // Data
    
    FrameLib_MultiChannel *mObject;
    
    std::vector <Input> mInputs;
    std::vector <void *> mOutputs;

    long mProxyNum;
    long mConfirmIndex;
    bool mConfirm;
    
    t_object *mTopLevelPatch;
    t_object *mSyncIn;
    
    SyncCheck mSyncChecker;
    
public:
    
    t_object *mUserObject;
};

// Convenience for Objects Using FrameLib_Expand (use FrameLib_MaxClass_Expand<T>::makeClass() to create)

template <class T, bool argsSetAllInputs = false>
class FrameLib_MaxClass_Expand : public FrameLib_MaxClass<FrameLib_Expand<T>, argsSetAllInputs> {};
