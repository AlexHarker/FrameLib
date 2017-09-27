
#include "MaxClass_Base.h"

#include "FrameLib_Global.h"
#include "FrameLib_Context.h"
#include "FrameLib_Parameters.h"
#include "FrameLib_DSP.h"
#include "FrameLib_Multichannel.h"

#include <string>
#include <vector>

//////////////////////////////////////////////////////////////////////////
//////////////////////////// Max Globals Class ///////////////////////////
//////////////////////////////////////////////////////////////////////////

class FrameLib_MaxGlobals : public MaxClass_Base
{
    
public:
    
    // Sync Check Class
    
    class SyncCheck
    {
        
    public:
        
        enum Mode { kDownOnly, kDown, kAcross };
        enum Action { kSyncComplete, kSync, kAttachAndSync };
        
        SyncCheck() : mGlobal(get()), mObject(NULL), mTime(-1), mMode(kDownOnly) {}
        ~SyncCheck() { mGlobal->release(); }
        
        Action operator()(void *object, bool handlesAudio, bool isOutput)
        {
            const SyncCheck *info = mGlobal->getSyncCheck();
            
            if (info && (info->mTime != mTime || info->mObject != mObject))
            {
                set(info->mObject, info->mTime, info->mMode);
                return handlesAudio && object != mObject && (mMode != kAcross || isOutput) ? kAttachAndSync : kSync;
            }
            
            if (info && mMode == kAcross && info->mMode == kDown)
            {
                mMode = kDown;
                return handlesAudio && object != mObject && !isOutput ? kAttachAndSync : kSync;
            }
            
            return kSyncComplete;
        }
        
        void sync(void *object = NULL, long time = -1, Mode mode = kDownOnly)
        {
            set(object, time, mode);
            mGlobal->setSyncCheck(object ? this : NULL);
        }
        
        bool upwardsMode()  { return setMode(mGlobal->getSyncCheck(), kAcross); }
        void restoreMode()  { setMode(mGlobal->getSyncCheck(), mMode); }
        
    private:
        
        void set(void *object, long time, Mode mode)
        {
            mObject = object;
            mTime = time;
            mMode = mode;
        }
    
        bool setMode(SyncCheck *info, Mode mode)    { return info && info->mMode != kDownOnly && ((info->mMode = mode) == mode); }
        
        FrameLib_MaxGlobals *mGlobal;
        void *mObject;
        long mTime;
        Mode mMode;
    };

    // ConnectionInfo Struct
    
    struct ConnectionInfo
    {
        enum Mode { kConnect, kConfirm, kDoubleCheck };

        ConnectionInfo(t_object *object, unsigned long index, Mode mode) : mObject(object), mIndex(index), mMode(mode) {}
        
        t_object *mObject;
        unsigned long mIndex;
        Mode mMode;
        
    };

    // Convenience Pointer for automatic deletion and RAII
    
    struct ManagedPointer
    {
        ManagedPointer() : mPointer(get()) {}
        ~ManagedPointer() { mPointer->release(); }
        
        FrameLib_MaxGlobals *operator->() { return mPointer; }
        
    private:
        
        // Deleted
        
        ManagedPointer(const ManagedPointer&);
        ManagedPointer& operator=(const ManagedPointer&);

        FrameLib_MaxGlobals *mPointer;
    };
    
    // Constructor and Destructor (public for the max API, but use the ManagedPointer for use from outside this class)
    
    FrameLib_MaxGlobals(t_symbol *sym, long ac, t_atom *av)
    : mGlobal(NULL), mConnectionInfo(NULL), mSyncCheck(NULL), mCount(0) { FrameLib_Global::get(&mGlobal); }
    ~FrameLib_MaxGlobals() { FrameLib_Global::release(&mGlobal); }

    // Getters and setters for max global items
    
    FrameLib_Global *getGlobal() const                      { return mGlobal; }
    
    const ConnectionInfo *getConnectionInfo() const         { return mConnectionInfo; }
    void setConnectionInfo(ConnectionInfo *info = NULL)     { mConnectionInfo = info; }
    
    SyncCheck *getSyncCheck() const                         { return mSyncCheck; }
    void setSyncCheck(SyncCheck *check = NULL)              { mSyncCheck = check; }
    
private:
    
    // Get and release the max global items (singleton)
    
    static FrameLib_MaxGlobals *get()
    {
        const char maxGlobalClass[] = "__fl.max_global_items";
        t_symbol *nameSpace = gensym("__fl.framelib_private");
        t_symbol *globalTag = gensym("__fl.max_global_tag");
     
        // Make sure the max globals class exists

        if (!class_findbyname(CLASS_NOBOX, gensym(maxGlobalClass)))
            makeClass<FrameLib_MaxGlobals>(CLASS_NOBOX, maxGlobalClass);
        
        // See if an object is registered (otherwise make object and register it...)
        
        FrameLib_MaxGlobals *x = (FrameLib_MaxGlobals *) object_findregistered(nameSpace, globalTag);
        
        if (!x)
            x = (FrameLib_MaxGlobals *) object_register(nameSpace, globalTag, object_new_typed(CLASS_NOBOX, gensym(maxGlobalClass), 0, NULL));
            
        x->mCount++;
        
        return x;
    }
    
    void release()
    {
        if (--mCount == 0)
        {
            object_unregister(this);
            object_free(this);
        }
    }
    
    // Pointers
    
    FrameLib_Global *mGlobal;
    ConnectionInfo *mConnectionInfo;
    SyncCheck *mSyncCheck;
    long mCount;
};

//////////////////////////////////////////////////////////////////////////
////////////////////// Mutator for Synchronisation ///////////////////////
//////////////////////////////////////////////////////////////////////////

class Mutator : public MaxClass_Base
{
    
public:
    
    Mutator(t_symbol *sym, long ac, t_atom *av)
    {
        mObject = ac ? atom_getobj(av) : NULL;
        mMode = object_method(mObject, gensym("__fl.is_output")) ? FrameLib_MaxGlobals::SyncCheck::kDownOnly : FrameLib_MaxGlobals::SyncCheck::kDown;
    }
    
    static void classInit(t_class *c, t_symbol *nameSpace, const char *classname)
    {
        addMethod<Mutator, &Mutator::mutate>(c, "signal");
    }
    
    void mutate(t_symbol *sym, long ac, t_atom *av)
    {
        mSyncChecker.sync(mObject, gettime(), mMode);
        object_method(mObject, gensym("sync"));
        mSyncChecker.sync();
    }
    
private:
    
    FrameLib_MaxGlobals::SyncCheck mSyncChecker;
    FrameLib_MaxGlobals::SyncCheck::Mode mMode;
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
        addMethod<Wrapper<T>, &Wrapper<T>::subpatcher>(c, "subpatcher");
        addMethod<Wrapper<T>, &Wrapper<T>::assist>(c, "assist");
        addMethod<Wrapper<T>, &Wrapper<T>::anything>(c, "anything");
        addMethod<Wrapper<T>, &Wrapper<T>::sync>(c, "sync");
        addMethod(c, (method) &externalPatchLineUpdate, "patchlineupdate");
        addMethod(c, (method) &externalConnectionAccept, "connectionaccept");
        addMethod(c, (method) &externalWrapperInternalObject, "__fl.wrapper_internal_object");
        
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
        // Create patcher (you must report this as a subpatcher to get audio working)
        
        t_dictionary *d = dictionary_new();
        t_atom a;
        t_atom *av = NULL;
        long ac = 0;
        
        atom_setparse(&ac, &av, "@defrect 0 0 300 300");
        attr_args_dictionary(d, ac, av);
        atom_setobj(&a, d);
        mPatch = (t_object *)object_new_typed(CLASS_NOBOX, gensym("jpatcher"),1, &a);
        
        // Get box text (and strip object name from the top - relace with stored name in case the object name is an alias)
        
        t_object *textfield = NULL;
        const char *text = NULL;
        std::string newObjectText = accessClassName<Wrapper>()->c_str();

        object_obex_lookup(this, gensym("#B"), &textfield);
        
        if ((textfield = jbox_get_textfield(textfield)))
        {
            text = (char *)object_method(textfield, gensym("getptr"));
            text = strchr(text, ' ');
            
            if (text)
                newObjectText += text;
        }
        
        // Make internal object

        mObject = jbox_get_object((t_object *) newobject_sprintf(mPatch, "@maxclass newobj @text \"unsynced.%s\" @patching_rect 0 0 30 10", newObjectText.c_str()));
        
        // Make Mutator (with argument referencing the internal object)
        
        atom_setobj(&a, mObject);
        mMutator = (t_object *) object_new_typed(CLASS_NOBOX, gensym("__fl.signal.mutator"), 1, &a);
        
        // Free the dictionary
    
        object_free(d);
        
        // Get the object itself (typed)
        
        T *internal = internalObject();
        
        long numIns = internal->getNumIns() + (internal->supportsDependencyConnections() ? 1 : 0);
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
    
    void *subpatcher(long index, void *arg)
    {
        return ((t_ptr_uint) arg > 1 && !NOGOOD(arg) && index == 0) ? (void *) mPatch : NULL;
    }
    
    void assist(void *b, long m, long a, char *s)
    {
        internalObject()->assist(b, m, a + 1, s);
    }
    
    void sync()
    {
        internalObject()->sync();
    }
    
    void anything(t_symbol *sym, long ac, t_atom *av)
    {
        outlet_anything(mInOutlets[getInlet()], sym, ac, av);
    }
    
    // External methods (A_CANT)
    
    static t_max_err externalPatchLineUpdate(Wrapper *x, t_object *patchline, long updatetype, t_object *src, long srcout, t_object *dst, long dstin)
    {
        if ((t_object *) x == dst)
            return T::externalPatchLineUpdate(x->internalObject(), patchline, updatetype, src, srcout, x->mObject, dstin + 1);
        else
            return T::externalPatchLineUpdate(x->internalObject(), patchline, updatetype, x->mObject, srcout + 1, dst, dstin);
    }
    
    static t_ptr_int externalConnectionAccept(Wrapper *src, t_object *dst, long srcout, long dstin, t_object *outlet, t_object *inlet)
    {
        // Only called for sources / account for internal sync connections

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
        addMethod<FrameLib_MaxClass<T>, &FrameLib_MaxClass<T>::assist>(c, "assist");
        addMethod<FrameLib_MaxClass<T>, &FrameLib_MaxClass<T>::info>(c, "info");
        addMethod<FrameLib_MaxClass<T>, &FrameLib_MaxClass<T>::frame>(c, "frame");
        addMethod<FrameLib_MaxClass<T>, &FrameLib_MaxClass<T>::sync>(c, "sync");
        addMethod<FrameLib_MaxClass<T>, &FrameLib_MaxClass<T>::dsp>(c);
        addMethod(c, (method) &externalPatchLineUpdate, "patchlineupdate");
        addMethod(c, (method) &externalConnectionAccept, "connectionaccept");
        addMethod(c, (method) &externalResolveConnections, "__fl.resolve_connections");
        addMethod(c, (method) &externalIsConnected, "__fl.is_connected");
        addMethod(c, (method) &externalConnectionConfirm, "__fl.connection_confirm");
        addMethod(c, (method) &externalGetInternalObject, "__fl.get_internal_object");
        addMethod(c, (method) &externalIsOutput, "__fl.is_output");
        addMethod(c, (method) &externalGetNumAudioIns, "__fl.get_num_audio_ins");
        addMethod(c, (method) &externalGetNumAudioOuts, "__fl.get_num_audio_outs");
        
        dspInit(c);
    }

    // Constructor and Destructor
    
    FrameLib_MaxClass(t_symbol *s, long argc, t_atom *argv) : mConfirmObject(NULL), mConfirmInIndex(-1), mConfirmOutIndex(-1), mConfirm(false), mTopLevelPatch(jpatcher_get_toppatcher(gensym("#P")->s_thing)), mSyncIn(NULL), mNeedsResolve(true), mUserObject(*this)
    {
        // Object creation with parameters and arguments (N.B. the object is not a member due to size restrictions)
        
        FrameLib_Parameters::AutoSerial serialisedParameters;
        parseParameters(serialisedParameters, argc, argv);
        mObject = new T(FrameLib_Context(mGlobal->getGlobal(), mTopLevelPatch), &serialisedParameters, this);
        parseInputs(argc, argv);
        
        long numIns = getNumIns() + (supportsDependencyConnections() ? 1 : 0);

        mInputs.resize(numIns);
        mOutputs.resize(getNumOuts());
        
        // Create frame inlets and outlets
        
        // N.B. - we create a proxy if the inlet is not the first inlet (not the first frame input or the object handles audio)
        
        for (long i = numIns - 1; i >= 0; i--)
            mInputs[i] = (t_object *) ((i || T::handlesAudio()) ? proxy_new(this, getNumAudioIns() + i, &mProxyNum) : NULL);
        
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

        delete mObject;

        for (typename std::vector <t_object *>::iterator it = mInputs.begin(); it != mInputs.end(); it++)
            object_free(*it);
        
        object_free(mSyncIn);
    }

    void assist(void *b, long m, long a, char *s)
    {
        if (m == ASSIST_OUTLET)
        {
            if (a == 0 && T::handlesAudio())
                 sprintf(s,"(signal) Audio Synchronisation Output" );
            else if (a < getNumAudioOuts())
                sprintf(s,"(signal) %s", mObject->audioInfo(a - 1).c_str());
            else
                sprintf(s,"(frame) %s", mObject->outputInfo(a - getNumAudioOuts()).c_str());
        }
        else
        {
            if (a == 0 && T::handlesAudio())
                sprintf(s,"(signal) Audio Synchronisation Input");
            else if (a < getNumAudioIns())
                sprintf(s,"(signal) %s", mObject->audioInfo(a - 1).c_str());
            else
                sprintf(s,"(frame) %s", mObject->inputInfo(a - getNumAudioIns()).c_str());
        }
    }
    
    void info(t_symbol *sym, long ac, t_atom *av)
    {
        // Determine what to post
        
        enum InfoFlags { kInfoDesciption = 0x01, kInfoInputs = 0x02, kInfoOutputs = 0x04, kInfoParameters = 0x08 };
        bool verbose = true;
        long flags = 0;
        
        while (ac--)
        {
            t_symbol *type = atom_getsym(av++);
            
            if (type == gensym("description"))          flags |= kInfoDesciption;
            else if (type == gensym("inputs"))          flags |= kInfoInputs;
            else if (type == gensym("outputs"))         flags |= kInfoOutputs;
            else if (type == gensym("io"))              flags |= kInfoInputs | kInfoOutputs;
            else if (type == gensym("parameters"))      flags |= kInfoParameters;
            else if (type == gensym("quick"))           verbose = false;
        }
        
        flags = !flags ? (kInfoDesciption | kInfoInputs | kInfoOutputs | kInfoParameters) : flags;
        
        // Start Tag
        
        object_post(mUserObject, "********* %s *********", object_classname(mUserObject)->s_name);

        // Description
        
        if (flags & kInfoDesciption)
        {
            object_post(mUserObject, "--- Description ---");
            postSplit(mObject->objectInfo(verbose).c_str(), "", "-");
        }
        
        // IO
        
        if (flags & kInfoInputs)
        {
            object_post(mUserObject, "--- Input List ---");
            if (argsSetAllInputs)
                object_post(mUserObject, "N.B. - arguments set the fixed array values for all inputs.");
            for (long i = 0; i < mObject->getNumAudioIns(); i++)
                object_post(mUserObject, "Audio Input %ld: %s", i + 1, mObject->audioInfo(i, verbose).c_str());
            for (long i = 0; i < mObject->getNumIns(); i++)
                object_post(mUserObject, "Frame Input %ld [%s]: %s", i + 1, frameTypeString(mObject->inputType(i)), mObject->inputInfo(i, verbose).c_str());
            if (supportsDependencyConnections())
                object_post(mUserObject, "Dependency Input [Any]: Connect to ensure ordering");
        }
        
        if (flags & kInfoOutputs)
        {
            object_post(mUserObject, "--- Output List ---");
            for (long i = 0; i < mObject->getNumAudioOuts(); i++)
                object_post(mUserObject, "Audio Output %ld: %s", i + 1, mObject->audioInfo(i, verbose).c_str());
            for (long i = 0; i < mObject->getNumOuts(); i++)
                object_post(mUserObject, "Frame Output %ld [%s]: %s", i + 1, frameTypeString(mObject->outputType(i)), mObject->outputInfo(i, verbose).c_str());
        }
        
        // Parameters
        
        if (flags & kInfoParameters)
        {
            object_post(mUserObject, "--- Parameter List ---");
            
            const FrameLib_Parameters *params = mObject->getParameters();
            if (!params || !params->size()) object_post(mUserObject, "< No Parameters >");
            
            // Loop over parameters
            
            for (long i = 0; params && i < params->size(); i++)
            {
                FrameLib_Parameters::Type type = params->getType(i);
                FrameLib_Parameters::NumericType numericType = params->getNumericType(i);
                std::string defaultStr = params->getDefaultString(i);

                // Name, type and default value
                
                if (defaultStr.size())
                    object_post(mUserObject, "Parameter %ld: %s [%s] (default: %s)", i + 1, params->getName(i).c_str(), params->getTypeString(i).c_str(), defaultStr.c_str());
                else
                    object_post(mUserObject, "Parameter %ld: %s [%s]", i + 1, params->getName(i).c_str(), params->getTypeString(i).c_str());

                // Verbose - arguments, range (for numeric types), enum items (for enums), array sizes (for arrays), description
                
                if (verbose)
                {
                    if (!argsSetAllInputs && params->getArgumentIdx(i) >= 0)
                        object_post(mUserObject, "- Argument: %ld", params->getArgumentIdx(i) + 1);
                    if (numericType == FrameLib_Parameters::kNumericInteger || numericType == FrameLib_Parameters::kNumericDouble)
                    {
                        switch (params->getClipMode(i))
                        {
                            case FrameLib_Parameters::kNone:    break;
                            case FrameLib_Parameters::kMin:     object_post(mUserObject, "- Min Value: %lg", params->getMin(i));                        break;
                            case FrameLib_Parameters::kMax:     object_post(mUserObject, "- Max Value: %lg", params->getMax(i));                        break;
                            case FrameLib_Parameters::kClip:    object_post(mUserObject, "- Clipped: %lg-%lg", params->getMin(i), params->getMax(i));   break;
                        }
                    }
                    if (type == FrameLib_Parameters::kEnum)
                        for (long j = 0; j <= params->getMax(i); j++)
                            object_post(mUserObject, "   [%ld] - %s", j, params->getItemString(i, j).c_str());
                    else if (type == FrameLib_Parameters::kArray)
                        object_post(mUserObject, "- Array Size: %ld", params->getArraySize(i));
                    else if (type == FrameLib_Parameters::kVariableArray)
                        object_post(mUserObject, "- Array Max Size: %ld", params->getArrayMaxSize(i));
                    postSplit(params->getInfo(i).c_str(), "- ", "-");
                }
            }
        }
    }

    // IO Helpers
    
    bool supportsDependencyConnections()    { return mObject->supportsDependencyConnections(); }
    
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
        // Resolve connections and mark unresolved for next time
        
        // FIX - is this necessary?
        //resolveConnections();
        mNeedsResolve = true;
        
        // Reset DSP
        
        mObject->reset(samplerate, maxvectorsize);
        
        // Add a perform routine to the chain if the object handles audio
        
        if (T::handlesAudio())
            addPerform<FrameLib_MaxClass, &FrameLib_MaxClass<T>::perform>(dsp64);
    }

    // Audio Synchronisation
    
    void sync()
    {
        FrameLib_MaxGlobals::SyncCheck::Action action = mSyncChecker(this, T::handlesAudio(), externalIsOutput(this));
       
        if (action != FrameLib_MaxGlobals::SyncCheck::kSyncComplete && T::handlesAudio && mNeedsResolve)
            traverseToResolveConnections(mTopLevelPatch);
        
        if (action == FrameLib_MaxGlobals::SyncCheck::kAttachAndSync)
            outlet_anything(mSyncIn, gensym("signal"), 0, NULL);
        
        if (action != FrameLib_MaxGlobals::SyncCheck::kSyncComplete)
        {
            for (unsigned long i = getNumOuts(); i > 0; i--)
                outlet_anything(mOutputs[i - 1], gensym("sync"), 0, NULL);
            
            if (mSyncChecker.upwardsMode())
            {
                for (unsigned long i = 0; i < getNumIns(); i++)
                    if (mObject->isConnected(i))
                        object_method(mObject->getInputConnection(i)->getOwner(), gensym("sync"));
                
                if (supportsDependencyConnections())
                    for (unsigned long i = 0; i < mObject->getNumDependencyConnections(); i++)
                        object_method(mObject->getDependencyConnection(i)->getOwner(), gensym("sync"));
                
                mSyncChecker.restoreMode();
            }
        }
    }
    
    // Connection Routines
    
    void frame()
    {
        const ConnectionInfo *info = mGlobal->getConnectionInfo();
        long index = getInlet() - getNumAudioIns();
        
        if (!info)
            return;
        
        switch (info->mMode)
        {
            case ConnectionInfo::kConnect:
                connect(info->mObject, info->mIndex, index);
                break;
                
            case ConnectionInfo::kConfirm:
            case ConnectionInfo::kDoubleCheck:

                if (index == mConfirmInIndex && mConfirmObject == info->mObject && mConfirmOutIndex == info->mIndex)
                {
                    mConfirm = true;
                    if (info->mMode == ConnectionInfo::kDoubleCheck)
                        object_error(mUserObject, "extra connection to input %ld", index + 1);
                }
                break;
        }
    }

    // External methods (A_CANT)
    
    static t_ptr_int externalConnectionAccept(FrameLib_MaxClass *src, t_object *dst, long srcout, long dstin, t_object *outlet, t_object *inlet)
    {
        return src->connectionAccept(dst, srcout, dstin, outlet, inlet);
    }
    
    static t_max_err externalPatchLineUpdate(FrameLib_MaxClass *x, t_object *patchline, long updatetype, t_object *src, long srcout, t_object *dst, long dstin)
    {
        return x->patchLineUpdate(patchline, updatetype, src, srcout, dst, dstin);
    }

    static void externalResolveConnections(FrameLib_MaxClass *x)
    {
        x->resolveConnections();
    }
    
    static t_ptr_int externalIsConnected(FrameLib_MaxClass *x, unsigned long index)
    {
        return x->confirmConnection(index, ConnectionInfo::kConfirm);
    }
    
    static void externalConnectionConfirm(FrameLib_MaxClass *x, unsigned long index, FrameLib_MaxGlobals::ConnectionInfo::Mode mode)
    {
        x->makeConnection(index, mode);
    }
    
    static FrameLib_MultiChannel *externalGetInternalObject(FrameLib_MaxClass *x)
    {
        return x->mObject;
    }
    
    static t_ptr_int externalIsOutput(FrameLib_MaxClass *x)
    {
        return T::handlesAudio() && (x->getNumAudioOuts() > 1);
    }
    
    static t_ptr_int externalGetNumAudioIns(FrameLib_MaxClass *x)
    {
        return x->getNumAudioIns();
    }
    
    static t_ptr_int externalGetNumAudioOuts(FrameLib_MaxClass *x)
    {
        return x->getNumAudioOuts();
    }

private:
    
    typedef FrameLib_MaxGlobals::ConnectionInfo ConnectionInfo;
    
    // Unwrapping connections
    
    void unwrapConnection(t_object *& object, long& connection)
    {
        t_object *wrapped = (t_object *) object_method(object, gensym("__fl.wrapper_internal_object"));
        
        if (wrapped)
        {
            object = wrapped;
            connection++;
        }
    }
    
    // Get an internal object from a generic pointer safely
    
    FrameLib_MultiChannel *getInternalObject(t_object *x)
    {
        return (FrameLib_MultiChannel *) object_method(x, gensym("__fl.get_internal_object"));
    }
    
    // Private connection methods
    
    void traverseToResolveConnections(t_patcher *p)
    {
        // Avoid recursion into a poly / pfft / etc.
        
        t_object *assoc = 0;
        object_method(p, gensym("getassoc"), &assoc);
        if (assoc)
            return;
        
        // Search for subpatchers, and call method on objects that don't have subpatchers
        
        for (t_box *b = jpatcher_get_firstobject(p); b; b = jbox_get_nextobject(b))
        {
            long index = 0;
            
            while (b && (p = (t_patcher *)object_subpatcher(jbox_get_object(b), &index, this)))
                traverseToResolveConnections(p);
            
            object_method(jbox_get_object(b), gensym("__fl.resolve_connections"));
        }
    }

    void resolveConnections()
    {
        if (mNeedsResolve)
        {
            // Confirm input connections
                    
            for (unsigned long i = 0; i < getNumIns(); i++)
                confirmConnection(i, ConnectionInfo::kConfirm);
            
            // Confirm dependency connections
            
            for (unsigned long i = 0; i < mObject->getNumDependencyConnections(); i++)
                confirmConnection(mObject->getDependencyConnection(i), mObject->getDependencyConnectionIdx(i), getNumIns(), ConnectionInfo::kConfirm);
            
            // Make output connections
            
            for (unsigned long i = getNumOuts(); i > 0; i--)
                makeConnection(i - 1, ConnectionInfo::kConnect);
            
            mNeedsResolve = false;
        }
    }

    void makeConnection(unsigned long index, ConnectionInfo::Mode mode)
    {
        ConnectionInfo info(*this, index, mode);
        
        mGlobal->setConnectionInfo(&info);
        outlet_anything(mOutputs[index], gensym("frame"), 0, NULL);
        mGlobal->setConnectionInfo();
    }
    
    bool confirmConnection(unsigned long inIndex, ConnectionInfo::Mode mode)
    {
        if (!validInput(inIndex))
            return false;
        
        return confirmConnection(getInputConnection(inIndex), getInputConnectionIdx(inIndex), inIndex, mode);
    }
    
    bool confirmConnection(void *object, unsigned long outIndex, unsigned long inIndex, ConnectionInfo::Mode mode)
    {
        if (!validInput(inIndex))
            return false;
        
        mConfirm = false;
        mConfirmObject = (t_object *) object;
        mConfirmInIndex = inIndex;
        mConfirmOutIndex = outIndex;
    
        // Check for connection *only* if the internal object is connected (otherwise assume the previously connected object has been deleted)
        
        if (mConfirmObject)
            object_method(mConfirmObject, gensym("__fl.connection_confirm"), mConfirmOutIndex, mode);
        
        if (mConfirmObject && !mConfirm)
            disconnect(mConfirmObject, mConfirmOutIndex, mConfirmInIndex);
        
        bool result = mConfirm;
        mConfirm = false;
        mConfirmObject = NULL;
        mConfirmInIndex = mConfirmOutIndex = -1;
        
        return result;
    }
    
    bool validInput(long index, FrameLib_MultiChannel *object)      { return object && index >= 0 && index < object->getNumIns(); }
    bool validOutput(long index, FrameLib_MultiChannel *object)     { return object && index >= 0 && index < object->getNumOuts(); }
    bool dependencyInput(long index, FrameLib_MultiChannel *object) { return object && object->supportsDependencyConnections() && index == object->getNumIns(); }
    bool validInput(long index)                                     { return validInput(index, mObject); }
    bool validOutput(long index)                                    { return validOutput(index, mObject); }
    bool dependencyInput(long index)                                { return dependencyInput(index, mObject); }
    
    t_object *getInputConnection(long index)                        { return (t_object *) (mObject->isConnected(index) ? mObject->getInputConnection(index)->getOwner() : NULL); }
    unsigned long getInputConnectionIdx(long index)                 { return mObject->getInputConnectionIdx(index); }
    bool matchConnection(t_object *src, long outIdx, long inIdx)    { return getInputConnection(inIdx) == src && getInputConnectionIdx(inIdx) == outIdx; }
    
    void connect(t_object *src, long outIdx, long inIdx)
    {
        FrameLib_MultiChannel *object = getInternalObject(src);
        
        if (!dependencyInput(inIdx) && (!validInput(inIdx) || !validOutput(outIdx, object) || matchConnection(src, outIdx, inIdx) || confirmConnection(inIdx, ConnectionInfo::kDoubleCheck)))
            return;

        ConnectionResult result;
        
        if (dependencyInput(inIdx))
            result = mObject->addDependencyConnection(object, outIdx);
        else
            result = mObject->addConnection(object, outIdx, inIdx);

        switch (result)
        {
            case kConnectFeedbackDetected:
                object_error(mUserObject, "feedback loop detected");
                break;
                
            case kConnectWrongContext:
                object_error(mUserObject, "cannot connect objects from different top-level patchers");
                break;
                
            case kConnectSelfConnection:
                object_error(mUserObject, "direct feedback loop detected");
                break;
                
            case kConnectSuccess:
            case kConnectNoDependencySupport:
                break;
        }
    }
    
    void disconnect(t_object *src, long outIdx, long inIdx)
    {
        FrameLib_MultiChannel *object = getInternalObject(src);

        if (!dependencyInput(inIdx) && (!validInput(inIdx) || !matchConnection(src, outIdx, inIdx)))
            return;
        
        if (dependencyInput(inIdx))
            mObject->deleteDependencyConnection(object, outIdx);
        else
            mObject->deleteConnection(inIdx);
    }

    // Patchcord Colour
    
    void patchLineColor(t_object *patchline)
    {
        //double color[4] = { 0.0, 0.0, 1.0, 1.0 };
        //object_attr_setdouble_array(patchline, gensym("patchlinecolor"), 4, color);
    }
    
    // Patchline connections
    
    t_max_err patchLineUpdate(t_object *patchline, long updatetype, t_object *src, long srcout, t_object *dst, long dstin)
    {
        if (*this == dst)
        {
            unwrapConnection(src, srcout);
            srcout -= (long) object_method(src, gensym("__fl.get_num_audio_outs"));
            dstin -= getNumAudioIns();
            
            if ((dependencyInput(dstin) || validInput(dstin)) && updatetype != JPATCHLINE_ORDER)
                dspchain_setbroken(dspchain_fromobject(*this));
            
            switch (updatetype)
            {
                case JPATCHLINE_CONNECT:        connect(src, srcout, dstin);        break;
                case JPATCHLINE_DISCONNECT:     disconnect(src, srcout, dstin);     break;
                case JPATCHLINE_ORDER:                                              break;
            }
        }
        else
        {
            if (validOutput(srcout))
            {
                if (updatetype != JPATCHLINE_ORDER)
                    dspchain_setbroken(dspchain_fromobject(*this));
                if (updatetype == JPATCHLINE_CONNECT)
                    patchLineColor(patchline);
            }
        }
        
        return MAX_ERR_NONE;
    }
    
    long connectionAccept(t_object *dst, long srcout, long dstin, t_object *outlet, t_object *inlet)
    {
        t_symbol *className = object_classname(dst);
        
        if (!validOutput(srcout - getNumAudioOuts()) || className == gensym("outlet") || className == gensym("jpatcher"))
            return 1;

        unwrapConnection(dst, dstin);
        dstin -= (long) object_method(dst, gensym("__fl.get_num_audio_ins"));
        
        if (dependencyInput(dstin, getInternalObject(dst)) || (validInput(dstin, getInternalObject(dst)) && !object_method(dst, gensym("__fl.is_connected"), dstin)))
            return 1;
        
        return 0;
    }

    // Info Utilities
    
    void postSplit(const char *text, const char *firstLineTag, const char *lineTag)
    {
        std::string str(text);
        size_t oldPos, pos;
        
        for (oldPos = 0, pos = str.find_first_of(":."); oldPos < str.size(); pos = str.find_first_of(":.", pos + 1))
        {
            pos = pos == std::string::npos ? str.size() : pos;
            object_post(mUserObject, "%s%s", oldPos ? lineTag : firstLineTag, str.substr(oldPos, (pos - oldPos) + 1).c_str());
            oldPos = pos + 1;
        }
    }
    
    const char *frameTypeString(FrameType type)
    {
        switch (type)
        {
            case kFrameAny:     return "either";
            case kFrameNormal:  return "vector";
            case kFrameTagged:  return "tagged";
        }
    }
    
    // Parameter Parsing
    
    bool isParameterTag(t_symbol *sym)
    {
        size_t len = strlen(sym->s_name);
        char beg = sym->s_name[0];
        char end = sym->s_name[len - 1];
        
        return (beg == '#' || (beg == '/' && end != '/'))  && len > 1;
    }
    
    bool isInputTag(t_symbol *sym)
    {
        size_t len = strlen(sym->s_name);
        char beg = sym->s_name[0];
        char end = sym->s_name[len - 1];
        
        return (((beg == '/' && end == '/') || (beg == '<' && end == '>') || (beg == '[' && end == ']'))  && len > 2) || (beg == '~' && len >1);
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
        long i;
        
        // Parse arguments
        
        for (i = 0; i < argc; i++)
        {
            if (isTag(argv + i))
                break;
            
            if (!argsSetAllInputs)
            {
                char argNames[64];
                sprintf(argNames, "%ld", i);
                
                if (atom_gettype(argv + i) == A_SYM)
                {
                    t_symbol *str = atom_getsym(argv + i);
                    serialisedParameters.write(argNames, str->s_name);
                }
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
            
            for (long j = 0; i < argc; i++, j++)
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
            
            for (unsigned long j = 0; i && j < getNumIns(); j++)
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
    
    std::vector <t_object *> mInputs;
    std::vector <void *> mOutputs;

    long mProxyNum;
    t_object *mConfirmObject;
    long mConfirmInIndex;
    long mConfirmOutIndex;
    bool mConfirm;
    
    t_object *mTopLevelPatch;
    t_object *mSyncIn;
    
    FrameLib_MaxGlobals::ManagedPointer mGlobal;
    FrameLib_MaxGlobals::SyncCheck mSyncChecker;
    
    bool mNeedsResolve;
    
public:

    t_object *mUserObject;
};

// Convenience for Objects Using FrameLib_Expand (use FrameLib_MaxClass_Expand<T>::makeClass() to create)

template <class T, bool argsSetAllInputs = false>
class FrameLib_MaxClass_Expand : public FrameLib_MaxClass<FrameLib_Expand<T>, argsSetAllInputs> {};
