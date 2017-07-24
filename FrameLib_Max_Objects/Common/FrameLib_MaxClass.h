
#include "MaxClass_Base.h"

#include "FrameLib_Multichannel.h"
#include "FrameLib_DSP.h"
#include "FrameLib_Global.h"
#include "FrameLib_Context.h"

#include <vector>

// FIX - think about adding assist helpers for this later...

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

        ConnectionInfo(t_object *object, unsigned long index, t_object *topLevelPatch, Mode mode) :
        mObject(object), mIndex(index), mTopLevelPatch(topLevelPatch), mMode(mode) {}
        
        t_object *mObject;
        unsigned long mIndex;
        t_object *mTopLevelPatch;
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
        addMethod<Wrapper<T>, &Wrapper<T>::assist>(c, "assist");
        addMethod<Wrapper<T>, &Wrapper<T>::subpatcher>(c, "subpatcher");
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
        // Create Patcher (you must report this as a subpatcher to get audio working)
        
        t_dictionary *d = dictionary_new();
        t_atom a;
        t_atom *av = NULL;
        long ac = 0;
        
        atom_setparse(&ac, &av, "@defrect 0 0 300 300");
        attr_args_dictionary(d, ac, av);
        atom_setobj(&a, d);
        mPatch = (t_object *)object_new_typed(CLASS_NOBOX, gensym("jpatcher"),1, &a);
        
        // Get Box Text (and strip object name from the top - relace with stored name in case the object name is an alias)
        
        t_object *box = NULL;
        const char *text = NULL;
        std::string newObjectText = accessClassName<Wrapper>()->c_str();

        object_obex_lookup(this, gensym("#B"), &box);
        t_object *textfield = jbox_get_textfield(box);
        
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
        
        // Free the Dictionary
    
        object_free(d);
        
        // Get the Object Itself (typed)
        
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
        
        // Inlets for Messages/Signals
        
        for (long i = numIns + numAudioIns - 2; i >= 0 ; i--)
        {
            mInOutlets[i] = (t_object *) outlet_new(NULL, NULL);
            mProxyIns[i] = (t_object *)  (i ? proxy_new(this, i, &mProxyNum) : NULL);
        }
        
        // Outlets for Messages/Signals
        
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
    
    void sync()
    {
        internalObject()->sync();
    }
    
    void anything(t_symbol *sym, long ac, t_atom *av)
    {
        long inlet = getInlet();
        
        outlet_anything(mInOutlets[inlet], sym, ac, av);
    }
    
    // External methods (A_CANT)
    
    static t_max_err externalPatchLineUpdate(Wrapper *x, t_object *patchline, long updatetype, t_object *src, long srcout, t_object *dst, long dstin)
    {
        // Only handle destinations and account for internal sync connections
        
        if ((t_object *) x == dst)
            return T::externalPatchLineUpdate(x->internalObject(), patchline, updatetype, src, srcout, x->mObject, dstin + 1);
        
        return MAX_ERR_NONE;
    }
    
    static t_ptr_int externalConnectionAccept(Wrapper *src, t_object *dst, long srcout, long dstin, t_object *outlet, t_object *inlet)
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
    // Input Structure
    
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
        addMethod<FrameLib_MaxClass<T>, &FrameLib_MaxClass<T>::assist>(c, "assist");
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
    
    FrameLib_MaxClass(t_symbol *s, long argc, t_atom *argv) : mConfirmIndex(-1), mConfirm(false), mSyncIn(NULL), mNeedsResolve(true)
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
        // Resolve connections and mark unresolved for next time
        
        resolveConnections();
        mNeedsResolve = true;
        
        // Reset DSP
        
        mObject->reset(samplerate);
        
        // Add a perform routine to the chain if the object handles audio
        
        if (T::handlesAudio())
            addPerform<FrameLib_MaxClass, &FrameLib_MaxClass<T>::perform>(dsp64);
    }

    // Audio Synchronisation
    
    void sync()
    {
        if (T::handlesAudio)
            traverseToResolveConnections(mTopLevelPatch);
        
        FrameLib_MaxGlobals::SyncCheck::Action action = mSyncChecker(this, T::handlesAudio(), externalIsOutput(this));
        
        if (action == FrameLib_MaxGlobals::SyncCheck::kAttachAndSync)
            outlet_anything(mSyncIn, gensym("signal"), 0, NULL);
        
        if (action != FrameLib_MaxGlobals::SyncCheck::kSyncComplete)
        {
            for (unsigned long i = getNumOuts(); i > 0; i--)
                outlet_anything(mOutputs[i - 1], gensym("sync"), 0, NULL);
            
            if (mSyncChecker.upwardsMode())
            {
                for (unsigned long i = 0; i < getNumIns(); i++)
                    if (mInputs[i].mObject)
                        object_method(mInputs[i].mObject, gensym("sync"));
                mSyncChecker.restoreMode();
            }
        }
    }
    
    // Connection Routines
    
    void frame()
    {
        const FrameLib_MaxGlobals::ConnectionInfo *info = mGlobal->getConnectionInfo();
        long index = getInlet() - getNumAudioIns();
        
        if (!info)
            return;
        
        switch (info->mMode)
        {
            case FrameLib_MaxGlobals::ConnectionInfo::kConnect:

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
                
            case FrameLib_MaxGlobals::ConnectionInfo::kConfirm:
            case FrameLib_MaxGlobals::ConnectionInfo::kDoubleCheck:

                if (index == mConfirmIndex && mInputs[index].mObject == info->mObject && mInputs[index].mIndex == info->mIndex)
                {
                    mConfirm = true;
                    if (info->mMode == FrameLib_MaxGlobals::ConnectionInfo::kDoubleCheck)
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
        return x->confirmConnection(index, FrameLib_MaxGlobals::ConnectionInfo::kConfirm);
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
    
    // Globals
    
    FrameLib_Context getContext()
    {
        mTopLevelPatch = jpatcher_get_toppatcher(gensym("#P")->s_thing);
        
        return FrameLib_Context(mGlobal->getGlobal(), mTopLevelPatch);
    }
    
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
                confirmConnection(i, FrameLib_MaxGlobals::ConnectionInfo::kConfirm);
            
            // Make output connections
            
            for (unsigned long i = getNumOuts(); i > 0; i--)
                makeConnection(i - 1, FrameLib_MaxGlobals::ConnectionInfo::kConnect);
            
            mNeedsResolve = false;
        }
    }

    void makeConnection(unsigned long index, FrameLib_MaxGlobals::ConnectionInfo::Mode mode)
    {
        FrameLib_MaxGlobals::ConnectionInfo info(*this, index, mTopLevelPatch, mode);
        
        mGlobal->setConnectionInfo(&info);
        outlet_anything(mOutputs[index], gensym("frame"), 0, NULL);
        mGlobal->setConnectionInfo();
    }
    
    bool confirmConnection(unsigned long inputIndex, FrameLib_MaxGlobals::ConnectionInfo::Mode mode)
    {
        mConfirm = false;
        mConfirmIndex = inputIndex;
        
        // Check for connection *only* if the internal object is connected (otherwise assume the previously connected object has been deleted)
        
        if (mInputs[inputIndex].mObject && mObject->isConnected(inputIndex))
            object_method(mInputs[inputIndex].mObject, gensym("__fl.connection_confirm"), mInputs[inputIndex].mIndex, mode);
        
        if (mInputs[inputIndex].mObject && !mConfirm)
            disconnect(mInputs[inputIndex].mObject, mInputs[inputIndex].mIndex, inputIndex);
        
        bool result = mConfirm;
        mConfirm = false;
        mConfirmIndex = -1;
        
        return result;
    }
    
    bool validInput(long index, FrameLib_MultiChannel *object)      { return object && index >= 0 && index < object->getNumIns(); }
    bool validOutput(long index, FrameLib_MultiChannel *object)     { return object && index >= 0 && index < object->getNumOuts(); }
    bool validInput(long index)                                     { return validInput(index, mObject); }
    bool validOutput(long index)                                    { return validOutput(index, mObject); }
    
    void connect(t_object *src, long outIdx, long inIdx)
    {
        FrameLib_MultiChannel *object = getInternalObject(src);
        bool connectionChange = (mInputs[inIdx].mObject != src || mInputs[inIdx].mIndex != outIdx);
        
        if (!validInput(inIdx) || !validOutput(outIdx, object) || !connectionChange || confirmConnection(inIdx, FrameLib_MaxGlobals::ConnectionInfo::kDoubleCheck))
            return;
        
        // Change the connection if needed
        
        mInputs[inIdx].mObject = src;
        mInputs[inIdx].mIndex = outIdx;
        
        mObject->addConnection(object, outIdx, inIdx);
    }
    
    void disconnect(t_object *src, long outIdx, long inIdx)
    {
        if (!validInput(inIdx) || mInputs[inIdx].mObject != src || mInputs[inIdx].mIndex != outIdx)
            return;
        
        mInputs[inIdx].mObject = NULL;
        mInputs[inIdx].mIndex = 0;
        
        if (mObject->isConnected(inIdx))
            mObject->deleteConnection(inIdx);
    }

    // Patchline connections
    
    t_max_err patchLineUpdate(t_object *patchline, long updatetype, t_object *src, long srcout, t_object *dst, long dstin)
    {
        if (*this == dst)
        {
            unwrapConnection(src, srcout);
            
            srcout -= (long) object_method(src, gensym("__fl.get_num_audio_outs"));
            dstin -= getNumAudioIns();
                
            switch (updatetype)
            {
                case JPATCHLINE_CONNECT:        connect(src, srcout, dstin);        break;
                case JPATCHLINE_DISCONNECT:     disconnect(src, srcout, dstin);     break;
                case JPATCHLINE_ORDER:                                              break;
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
        
        long num_audio_ins = (long) object_method(dst, gensym("__fl.get_num_audio_ins"));
        bool connected = object_method(dst, gensym("__fl.is_connected"), dstin);
        
        if (validInput(dstin - num_audio_ins, getInternalObject(dst)) && !connected)
            return 1;
        
        return 0;
    }

    // Parameter Parsing
    
    bool isParameterTag(t_symbol *sym)
    {
        if (!sym)
            return false;
        
        size_t len = strlen(sym->s_name);
        char beg = sym->s_name[0];
        char end = sym->s_name[len - 1];
        
        return (beg == '#' || (beg == '/' && end != '/'))  && len > 1;
    }
    
    bool isInputTag(t_symbol *sym)
    {
        if (!sym)
            return false;
        
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
    
    FrameLib_MaxGlobals::ManagedPointer mGlobal;
    FrameLib_MaxGlobals::SyncCheck mSyncChecker;
    
    bool mNeedsResolve;
    
public:

    t_object *mUserObject;
};

// Convenience for Objects Using FrameLib_Expand (use FrameLib_MaxClass_Expand<T>::makeClass() to create)

template <class T, bool argsSetAllInputs = false>
class FrameLib_MaxClass_Expand : public FrameLib_MaxClass<FrameLib_Expand<T>, argsSetAllInputs> {};
