
#ifndef FRAMELIB_MAX_CLASS_H
#define FRAMELIB_MAX_CLASS_H

#include "MaxClass_Base.h"

#include "FrameLib_Global.h"
#include "FrameLib_Context.h"
#include "FrameLib_Parameters.h"
#include "FrameLib_DSP.h"
#include "FrameLib_Multistream.h"
#include "FrameLib_SerialiseGraph.h"

#include <string>
#include <vector>

struct FrameLib_MaxProxy : public virtual FrameLib_Proxy
{
    t_object *mMaxObject;
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////// Max Globals Class ///////////////////////////
//////////////////////////////////////////////////////////////////////////

class FrameLib_MaxGlobals : public MaxClass_Base
{
    
public:
    
    // Error Notification Class
    
    struct ErrorNotifier : public FrameLib_ErrorReporter::HostNotifier
    {
        ErrorNotifier(FrameLib_MaxGlobals *maxGlobals)
        {
            mQelem = qelem_new(maxGlobals, (method) &errorReport);
        }
        
        ~ErrorNotifier()
        {
            qelem_free(mQelem);
        }
        
        void notify()
        {
            qelem_set(mQelem);
        }
        
        static void errorReport(FrameLib_MaxGlobals* x)
        {
            auto reports = x->mGlobal->getErrors();

            for (auto it = reports->begin(); it != reports->end(); it++)
            {
                std::string errorText;
                t_object *object = it->getReporter() ? dynamic_cast<FrameLib_MaxProxy *>(it->getReporter())->mMaxObject : nullptr;
                t_object *userObject = object ? (t_object *) object_method(object, gensym("__fl.get_user_object")) : nullptr;
                
                it->getErrorText(errorText);
                
                if (userObject)
                {
                    if (it->getSource() == kErrorDSP)
                        object_error_obtrusive(userObject, errorText.c_str());
                    else
                        object_error(userObject, errorText.c_str());
                }
                else
                    ouchstring(errorText.c_str());
            }

            if (reports->isFull())
                error("*** FrameLib - too many errors - reporting only some ***");
        }
        
        t_qelem mQelem;
    };
    
    // Sync Check Class
    
    class SyncCheck
    {
        
    public:
        
        enum Mode { kDownOnly, kDown, kAcross };
        enum Action { kSyncComplete, kSync, kAttachAndSync };
        
        SyncCheck() : mGlobal(get()), mObject(nullptr), mTime(-1), mMode(kDownOnly) {}
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
        
        void sync(void *object = nullptr, long time = -1, Mode mode = kDownOnly)
        {
            set(object, time, mode);
            mGlobal->setSyncCheck(object ? this : nullptr);
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

        ConnectionInfo(t_object *object, long index, Mode mode) : mObject(object), mIndex(index), mMode(mode) {}
        
        t_object *mObject;
        long mIndex;
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
        
        ManagedPointer(const ManagedPointer&) = delete;
        ManagedPointer& operator=(const ManagedPointer&) = delete;

        FrameLib_MaxGlobals *mPointer;
    };
    
    // Constructor and Destructor (public for the max API, but use the ManagedPointer for use from outside this class)
    
    FrameLib_MaxGlobals(t_symbol *sym, long ac, t_atom *av)
    : mNotifier(new ErrorNotifier(this)), mGlobal(nullptr), mConnectionInfo(nullptr), mSyncCheck(nullptr)
    {}

    // Getters and setters for max global items
    
    FrameLib_Global *getGlobal() const                          { return mGlobal; }
    
    const ConnectionInfo *getConnectionInfo() const             { return mConnectionInfo; }
    void setConnectionInfo(ConnectionInfo *info = nullptr)      { mConnectionInfo = info; }
    
    SyncCheck *getSyncCheck() const                             { return mSyncCheck; }
    void setSyncCheck(SyncCheck *check = nullptr)               { mSyncCheck = check; }
    
private:
    
    // Generate some relevant thread priorities
    
    static FrameLib_Thread::Priorities priorities()
    {
#ifdef __APPLE__
        if (maxversion() >= 0x800)
            return { 31, 31, 43, SCHED_RR };
#endif
        return FrameLib_Thread::defaultPriorities();
    }
    
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
            x = (FrameLib_MaxGlobals *) object_register(nameSpace, globalTag, object_new_typed(CLASS_NOBOX, gensym(maxGlobalClass), 0, nullptr));
        
        FrameLib_Global::get(&x->mGlobal, priorities(), x->mNotifier.get());
        
        return x;
    }
    
    void release()
    {
        FrameLib_Global::release(&mGlobal);
        
        if (!mGlobal)
        {
            object_unregister(this);
            object_free(this);
        }
    }
    
    // Pointers
    
    std::unique_ptr<ErrorNotifier> mNotifier;
    FrameLib_Global *mGlobal;
    ConnectionInfo *mConnectionInfo;
    SyncCheck *mSyncCheck;
};

//////////////////////////////////////////////////////////////////////////
////////////////////// Mutator for Synchronisation ///////////////////////
//////////////////////////////////////////////////////////////////////////

class Mutator : public MaxClass_Base
{
    
public:
    
    Mutator(t_symbol *sym, long ac, t_atom *av)
    {
        mObject = ac ? atom_getobj(av) : nullptr;
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

template <class T>
class Wrapper : public MaxClass_Base
{
    
public:
    
    // Initialise Class
    
    static method *sigMethodCache()
    {
        static method sigMethod;
        
        return &sigMethod;
    }
    
    static void classInit(t_class *c, t_symbol *nameSpace, const char *classname)
    {
        addMethod<Wrapper<T>, &Wrapper<T>::parentpatcher>(c, "parentpatcher");
        addMethod<Wrapper<T>, &Wrapper<T>::subpatcher>(c, "subpatcher");
        addMethod<Wrapper<T>, &Wrapper<T>::assist>(c, "assist");
        addMethod<Wrapper<T>, &Wrapper<T>::anything>(c, "anything");
        addMethod<Wrapper<T>, &Wrapper<T>::sync>(c, "sync");
        addMethod<Wrapper<T>, &Wrapper<T>::dsp>(c);
        addMethod(c, (method) &externalPatchLineUpdate, "patchlineupdate");
        addMethod(c, (method) &externalConnectionAccept, "connectionaccept");
        addMethod(c, (method) &externalWrapperInternalObject, "__fl.wrapper_internal_object");
        addMethod(c, (method) &externalWrapperIsWrapper, "__fl.wrapper_is_wrapper");

        // N.B. MUST add signal handling after dspInit to override the builtin responses
        
        dspInit(c);
        *sigMethodCache() = class_method(c, gensym("signal"));
        addMethod<Wrapper<T>, &Wrapper<T>::anything>(c, "signal");
        
        // Make sure the mutator class exists
        
        const char mutatorClassName[] = "__fl.signal.mutator";
        
        if (!class_findbyname(CLASS_NOBOX, gensym(mutatorClassName)))
            Mutator::makeClass<Mutator>(CLASS_NOBOX, mutatorClassName);
    }

    // Constructor and Destructor
    
    Wrapper(t_symbol *s, long argc, t_atom *argv) : mParentPatch(gensym("#P")->s_thing)
    {
        // Create patcher (you must report this as a subpatcher to get audio working)
        
        t_dictionary *d = dictionary_new();
        t_atom a;
        t_atom *av = nullptr;
        long ac = 0;
        
        atom_setparse(&ac, &av, "@defrect 0 0 300 300");
        attr_args_dictionary(d, static_cast<short>(ac), av);
        atom_setobj(&a, d);
        mPatch = (t_object *)object_new_typed(CLASS_NOBOX, gensym("jpatcher"),1, &a);
        
        // Get box text (and strip object name from the top - relace with stored name in case the object name is an alias)
        
        t_object *textfield = nullptr;
        const char *text = nullptr;
        std::string newObjectText = accessClassName<Wrapper>()->c_str();

        object_obex_lookup(this, gensym("#B"), &textfield);
        
        if ((textfield = jbox_get_textfield(textfield)))
        {
            text = (char *) object_method(textfield, gensym("getptr"));
            text = strchr(text, ' ');
            
            if (text)
                newObjectText += text;
        }
        
        // Set the patch association
        
        object_method(mPatch, gensym("setassoc"), this);
        
        // Make internal object and disallow editing

        mObject = jbox_get_object((t_object *) newobject_sprintf(mPatch, "@maxclass newobj @text \"unsynced.%s\" @patching_rect 0 0 30 10", newObjectText.c_str()));
        object_method(mPatch, gensym("noedit"));
        
        // Make Mutator (with argument referencing the internal object)
        
        atom_setobj(&a, mObject);
        mMutator = (t_object *) object_new_typed(CLASS_NOBOX, gensym("__fl.signal.mutator"), 1, &a);
        
        // Free the dictionary
    
        object_free(d);
        
        // Get the object itself (typed)
        
        T *internal = internalObject();
        
        long numIns = internal->getNumIns() + (internal->supportsOrderingConnections() ? 1 : 0);
        long numOuts = internal->getNumOuts();
        long numAudioIns = internal->getNumAudioIns();
        long numAudioOuts = internal->getNumAudioOuts();
        
        // Create I/O
        
        mInOutlets.resize(numIns + numAudioIns - 1);
        mProxyIns.resize(numIns + numAudioIns - 1);
        mAudioOuts.resize(numAudioOuts - 1);
        mOuts.resize(numOuts);
        
        // Inlets for messages/signals (we need one audio in for the purposes of sync)
        
        dspSetup(1);

        for (long i = numIns + numAudioIns - 2; i >= 0 ; i--)
        {
            mInOutlets[i] = (t_object *) outlet_new(nullptr, nullptr);
            mProxyIns[i] = (t_object *)  (i ? proxy_new(this, i, &mProxyNum) : nullptr);
        }
        
        // Outlets for messages/signals
        
        for (long i = numOuts - 1; i >= 0 ; i--)
            mOuts[i] = (t_object *) outlet_new(this, nullptr);
        for (long i = numAudioOuts - 2; i >= 0 ; i--)
            mAudioOuts[i] = (t_object *) outlet_new(this, "signal");
        
        // Connect first signal outlet to the mutator
        
        outlet_add(outlet_nth(mObject, 0), inlet_nth(mMutator, 0));
        
        // Connect inlets (all types)
        
        for (long i = 0; i < numAudioIns + numIns - 1; i++)
            outlet_add(mInOutlets[i], inlet_nth(mObject, i + 1));
        
        // Connect non-audio outlets
        
        for (long i = 0; i < numOuts; i++)
            outlet_add(outlet_nth(mObject, i + numAudioOuts), mOuts[i]);
    }
    
    ~Wrapper()
    {
        // Delete ins and proxies
        
        for (auto it = mProxyIns.begin(); it != mProxyIns.end(); it++)
            object_free(*it);
        
        for (auto it = mInOutlets.begin(); it != mInOutlets.end(); it++)
            object_free(*it);
        
        // Free objects - N.B. - free the patch, but not the object within it (which will be freed by deleting the patch)
        
        object_free(mMutator);
        object_free(mPatch);
    }
    
    // Standard methods
    
    void parentpatcher(t_object **parent)
    {
        *parent = mParentPatch;
    }
    
    void *subpatcher(long index, void *arg)
    {
        return (((t_ptr_uint) arg <= 1) || ((t_ptr_uint) arg > 1 && !NOGOOD(arg))) && index == 0 ? (void *) mPatch : nullptr;
    }
    
    void assist(void *b, long m, long a, char *s)
    {
        internalObject()->assist(b, m, a + 1, s);
    }
    
    void sync()
    {
        // Must set the order of the wrapper after the internal object before calling internal sync
        
        (*sigMethodCache())(this);
        
        internalObject()->sync();
    }
    
    void dsp(t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
    {
        if (internalObject()->getType() == kOutput)
            addPerform<Wrapper, &Wrapper<T>::perform>(dsp64);
    }
    
    void perform(t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
    {
        std::vector<double*> &internalOuts = internalObject()->getAudioOuts();
        
        // Copy to output
        
        for (size_t i = 0; i < internalOuts.size(); i++)
            std::copy(internalOuts[i], internalOuts[i] + vec_size, outs[i]);
    }
    
    void anything(t_symbol *sym, long ac, t_atom *av)
    {
        outlet_anything(mInOutlets[getInlet()], sym, static_cast<int>(ac), av);
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
    
    static void *externalWrapperIsWrapper(Wrapper *x)
    {
        return (void *) 1;
    }
    
private:
    
    T *internalObject() { return (T *) mObject; }

    // Owned Objects (need freeing)
    
    t_object *mPatch;
    t_object *mMutator;
    
    // Unowned objects (the internal object is owned by the patch)
    
    t_object *mParentPatch;
    t_object *mObject;
    
    // Inlets (must be freed)
    
    std::vector<t_object *> mInOutlets;
    std::vector<t_object *> mProxyIns;
    
    // Outlets (don't need to free)
    
    std::vector<t_object *> mAudioOuts;
    std::vector<t_object *> mOuts;
    
    // Dummy for stuffloc on proxies
    
    long mProxyNum;
};

//////////////////////////////////////////////////////////////////////////
/////////////////////// FrameLib Max Object Class ////////////////////////
//////////////////////////////////////////////////////////////////////////

enum MaxObjectArgsMode { kAsParams, kAllInputs, kDistribute };

template <class T, MaxObjectArgsMode argsMode = kAsParams>
class FrameLib_MaxClass : public MaxClass_Base
{
    typedef FrameLib_Object<FrameLib_Multistream>::Connection FrameLibConnection;
    typedef FrameLib_Connection<t_object, long> MaxConnection;
    typedef FrameLib_MaxGlobals::ConnectionInfo ConnectionInfo;

public:
    
    // Class Initialisation (must explicitly give U for classes that inherit from FrameLib_MaxClass<>)
    
    template <class U = FrameLib_MaxClass<T, argsMode>>
    static void makeClass(const char *className)
    {
        // If handles audio/scheduler then make wrapper class and name the inner object differently..
        
        std::string internalClassName = className;
        
        if (T::handlesAudio())
        {
            Wrapper<U>:: template makeClass<Wrapper<U>>(CLASS_BOX, className);
            internalClassName.insert(0, "unsynced.");
        }
        
        MaxClass_Base::makeClass<U>(CLASS_BOX, internalClassName.c_str());
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
        addMethod(c, (method) &externalMarkUnresolved, "__fl.mark_unresolved");
        addMethod(c, (method) &externalAutoOrderingConnections, "__fl.auto_ordering_connections");
        addMethod(c, (method) &externalClearAutoOrderingConnections, "__fl.clear_auto_ordering_connections");
        addMethod(c, (method) &externalIsConnected, "__fl.is_connected");
        addMethod(c, (method) &externalConnectionConfirm, "__fl.connection_confirm");
        addMethod(c, (method) &externalGetInternalObject, "__fl.get_internal_object");
        addMethod(c, (method) &externalGetUserObject, "__fl.get_user_object");
        addMethod(c, (method) &externalIsOutput, "__fl.is_output");
        addMethod(c, (method) &externalGetNumAudioIns, "__fl.get_num_audio_ins");
        addMethod(c, (method) &externalGetNumAudioOuts, "__fl.get_num_audio_outs");
        class_addmethod(c, (method) &codeexport, "export", A_SYM, A_SYM, 0);
        
        dspInit(c);
    }

    // Check if a patch in memory matches a symbol representing a path
    
    static bool comparePatchWithName(t_object *patch, t_symbol *name)
    {
        char fileName[MAX_FILENAME_CHARS];
        t_fourcc validTypes[TYPELIST_SIZE];
        short outvol = 0, numTypes = 0;
        t_fourcc outtype = 0;
        
        typelist_make(validTypes, TYPELIST_MAXFILES, &numTypes);
        strncpy_zero(fileName, name->s_name, MAX_FILENAME_CHARS);
        locatefile_extended(fileName, &outvol, &outtype, validTypes, numTypes);
        
        return !strcmp(jpatcher_get_filename(patch)->s_name, fileName);
    }
    
    // Find the patcher for the context

    static t_object *contextPatcher(t_object *patch)
    {
        bool traverse = true;
        
        for (t_object *parent = nullptr; traverse && (parent = jpatcher_get_parentpatcher(patch)); patch = traverse ? parent : patch)
        {
            t_object *assoc = 0;
            object_method(patch, gensym("getassoc"), &assoc);
            
            // Traverse if the patch is in a box (subpatcher or abstraction) it belongs to a wrapper
            
            traverse = jpatcher_get_box(patch) || (assoc && object_method(assoc, gensym("__fl.wrapper_is_wrapper")));
            
            if (!traverse && !assoc)
            {
                // Get text of loading object in parent if there is no association (patch is currently loading)

                char *text = nullptr;
                
                for (t_object *b = jpatcher_get_firstobject(parent); b && !text; b = jbox_get_nextobject(b))
                    if (jbox_get_maxclass(b) == gensym("newobj") && jbox_get_textfield(b))
                        text = (char *) object_method(jbox_get_textfield(b), gensym("getptr"));

                if (text)
                {
                    // Get the first item in the box as a symbol
                
                    t_atombuf *atomBuffer = (t_atombuf *) atombuf_new(0, nullptr);
                    atombuf_text(&atomBuffer, &text, static_cast<long>(strlen(text)));
                    t_symbol *objectName = atom_getsym(atomBuffer->a_argv);
                    atombuf_free(atomBuffer);
                    
                    // Check if the patch is loading in a subpatcher otherwise check if it is loading in an abstraction
                    
                    traverse = objectName == gensym("p") || objectName == gensym("patcher") || comparePatchWithName(patch, objectName);
                }
                else
                {
                    // FIX - this is not perfect for bpatchers, but it is relatively safe for now
                    
                    for (t_object *b = jpatcher_get_firstobject(parent); b && !traverse; b = jbox_get_nextobject(b))
                        if (jbox_get_maxclass(b) == gensym("bpatcher"))
                            traverse = comparePatchWithName(patch, object_attr_getsym(b, gensym("name")));
                }
            }
        }
        
        return patch;
    }
    
    // Detect the user object at load time
    
    t_object *detectUserObjectAtLoad()
    {
        t_object *assoc = 0;
        t_object *patch = gensym("#P")->s_thing;
        object_method(patch, gensym("getassoc"), &assoc);
    
        return (assoc && object_method(assoc, gensym("__fl.wrapper_is_wrapper"))) ? assoc : *this;
    }
    
    // Constructor and Destructor
    
    FrameLib_MaxClass(t_symbol *s, long argc, t_atom *argv, FrameLib_MaxProxy *proxy = new FrameLib_MaxProxy()) : mFrameLibProxy(proxy), mConfirmObject(nullptr), mConfirmInIndex(-1), mConfirmOutIndex(-1), mConfirm(false), mPatch(gensym("#P")->s_thing), mContextPatch(contextPatcher(mPatch)), mSyncIn(nullptr), mUserObject(detectUserObjectAtLoad()), mNeedsResolve(true)
    {
        // Object creation with parameters and arguments (N.B. the object is not a member due to size restrictions)
        
        unsigned long nStreams = 1;
        
        if (argc && getStreamCount(argv))
        {
            nStreams = getStreamCount(argv);
            argv++;
            argc--;
        }
        
        FrameLib_Parameters::AutoSerial serialisedParameters;
        parseParameters(serialisedParameters, argc, argv);
        mFrameLibProxy->mMaxObject = *this;
        mObject.reset(new T(FrameLib_Context(mGlobal->getGlobal(), mContextPatch), &serialisedParameters, mFrameLibProxy.get(), nStreams));
        parseInputs(argc, argv);
        
        long numIns = getNumIns() + (supportsOrderingConnections() ? 1 : 0);

        mInputs.resize(numIns);
        mOutputs.resize(getNumOuts());
        
        // Create frame inlets and outlets
        
        // N.B. - we create a proxy if the inlet is not the first inlet (not the first frame input or the object handles audio)
        
        for (long i = numIns - 1; i >= 0; i--)
            mInputs[i] = (t_object *) ((i || handlesAudio()) ? proxy_new(this, getNumAudioIns() + i, &mProxyNum) : nullptr);
        
        for (long i = getNumOuts(); i > 0; i--)
            mOutputs[i - 1] = outlet_new(this, nullptr);
        
        // Setup for audio, even if the object doesn't handle it, so that dsp recompile works correctly
        
        dspSetup(getNumAudioIns());
        
        for (long i = 0; i < getNumAudioOuts(); i++)
            outlet_new(this, "signal");
        
        // Add a sync outlet if we need to handle audio
        
        if (handlesAudio())
        {
            mSyncIn = (t_object *) outlet_new(nullptr, nullptr);
            outlet_add(mSyncIn, inlet_nth(*this, 0));
        }
    }

    ~FrameLib_MaxClass()
    {
        dspFree();

        for (auto it = mInputs.begin(); it != mInputs.end(); it++)
            object_free(*it);
        
        object_free(mSyncIn);
    }
    
    void assist(void *b, long m, long a, char *s)
    {
        if (m == ASSIST_OUTLET)
        {
            if (a == 0 && handlesAudio())
                sprintf(s,"(signal) Audio Synchronisation Output" );
            else if (a < getNumAudioOuts())
                sprintf(s,"(signal) %s", mObject->audioInfo(a - 1).c_str());
            else
                sprintf(s,"(frame) %s", mObject->outputInfo(a - getNumAudioOuts()).c_str());
        }
        else
        {
            if (a == 0 && handlesAudio())
                sprintf(s,"(signal) Audio Synchronisation Input");
            else if (a < getNumAudioIns())
                sprintf(s,"(signal) %s", mObject->audioInfo(a - 1).c_str());
            else
            {
                if (supportsOrderingConnections() && a == getNumAudioIns() + getNumIns())
                    sprintf(s,"(frame) Ordering Input");
                else
                    sprintf(s,"(frame) %s", mObject->inputInfo(a - getNumAudioIns()).c_str());
            }
        }
    }
    
    static void codeexport(FrameLib_MaxClass *x, t_symbol *className, t_symbol *path)
    {
        char conformedPath[MAX_PATH_CHARS];
        
        if (!sys_getdspobjdspstate(*x))
            x->resolveGraph(true);

        path_nameconform(path->s_name, conformedPath, PATH_STYLE_NATIVE, PATH_TYPE_BOOT);
        ExportError error = exportGraph(x->mObject.get(), conformedPath, className->s_name);
        
        if (error == kExportPathError)
            object_error(x->mUserObject, "couldn't write to or find specified path");
        else if (error == kExportWriteError)
            object_error(x->mUserObject, "couldn't write file");
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
            if (argsMode == kAllInputs)
                object_post(mUserObject, "N.B. - arguments set the fixed array values for all inputs.");
            if (argsMode == kDistribute)
                object_post(mUserObject, "N.B - arguments are distributed one per input.");
            for (long i = 0; i < (long) mObject->getNumAudioIns(); i++)
                object_post(mUserObject, "Audio Input %ld: %s", i + 1, mObject->audioInfo(i, verbose).c_str());
            for (long i = 0; i < getNumIns(); i++)
                object_post(mUserObject, "Frame Input %ld [%s]: %s", i + 1, frameTypeString(mObject->inputType(i)), mObject->inputInfo(i, verbose).c_str());
            if (supportsOrderingConnections())
                object_post(mUserObject, "Ordering Input [%s]: Connect to ensure ordering", frameTypeString(kFrameAny));
        }
        
        if (flags & kInfoOutputs)
        {
            object_post(mUserObject, "--- Output List ---");
            for (long i = 0; i < (long) mObject->getNumAudioOuts(); i++)
                object_post(mUserObject, "Audio Output %ld: %s", i + 1, mObject->audioInfo(i, verbose).c_str());
            for (long i = 0; i < getNumOuts(); i++)
                object_post(mUserObject, "Frame Output %ld [%s]: %s", i + 1, frameTypeString(mObject->outputType(i)), mObject->outputInfo(i, verbose).c_str());
        }
        
        // Parameters
        
        if (flags & kInfoParameters)
        {
            object_post(mUserObject, "--- Parameter List ---");
            
            const FrameLib_Parameters *params = mObject->getParameters();
            if (!params || !params->size()) object_post(mUserObject, "< No Parameters >");
            
            // Loop over parameters
            
            for (unsigned long i = 0; params && i < params->size(); i++)
            {
                FrameLib_Parameters::Type type = params->getType(i);
                FrameLib_Parameters::NumericType numericType = params->getNumericType(i);
                std::string defaultStr = params->getDefaultString(i);

                // Name, type and default value
                
                if (defaultStr.size())
                    object_post(mUserObject, "Parameter %lu: %s [%s] (default: %s)", i + 1, params->getName(i).c_str(), params->getTypeString(i).c_str(), defaultStr.c_str());
                else
                    object_post(mUserObject, "Parameter %lu: %s [%s]", i + 1, params->getName(i).c_str(), params->getTypeString(i).c_str());

                // Verbose - arguments, range (for numeric types), enum items (for enums), array sizes (for arrays), description
                
                if (verbose)
                {
                    if (argsMode == kAsParams && params->getArgumentIdx(i) >= 0)
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
                        for (unsigned long j = 0; j <= static_cast<unsigned long>(params->getMax(i)); j++)
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
    
    bool supportsOrderingConnections()  { return mObject->supportsOrderingConnections(); }
    
    bool handlesAudio() const           { return T::handlesAudio(); }
    
    long getNumAudioIns() const         { return (long) mObject->getNumAudioIns() + (handlesAudio() ? 1 : 0); }
    long getNumAudioOuts() const        { return (long) mObject->getNumAudioOuts() + (handlesAudio() ? 1 : 0); }
    long getNumIns() const              { return (long) mObject->getNumIns(); }
    long getNumOuts() const             { return (long) mObject->getNumOuts(); }
    
    // Perform and DSP

    void perform(t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
    {
        if (static_cast<long>(mSigOuts.size()) != (numouts - 1))
        {
            for (long i = 1; i < numouts; i++)
                mSigOuts.push_back(outs[i]);
        }
            
        // N.B. Plus one due to sync inputs
        
        mObject->blockUpdate(ins + 1, outs + 1, vec_size);
    }

    void dsp(t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
    {
        mSigOuts.clear();
        
        // Resolve connections (in case there are no schedulers left in the patch) and mark unresolved for next time
        
        resolveConnections();
        mNeedsResolve = true;
        
        // Reset DSP
        
        mObject->reset(samplerate, maxvectorsize);
        
        // Add a perform routine to the chain if the object handles audio
        
        if (handlesAudio())
            addPerform<FrameLib_MaxClass, &FrameLib_MaxClass<T>::perform>(dsp64);
    }

    // Get Audio Outputs
    
    std::vector<double *> &getAudioOuts()
    {
        return mSigOuts;
    }
    
    // Type
    
    ObjectType getType()
    {
        return mObject->getType();
    }
    
    // Audio Synchronisation
    
    void sync()
    {
        FrameLib_MaxGlobals::SyncCheck::Action action = mSyncChecker(this, handlesAudio(), externalIsOutput(this));
       
        if (action != FrameLib_MaxGlobals::SyncCheck::kSyncComplete && handlesAudio() && mNeedsResolve)
            resolveGraph(false);
        
        if (action == FrameLib_MaxGlobals::SyncCheck::kAttachAndSync)
            outlet_anything(mSyncIn, gensym("signal"), 0, nullptr);
        
        if (action != FrameLib_MaxGlobals::SyncCheck::kSyncComplete)
        {
            for (long i = getNumOuts(); i > 0; i--)
                outlet_anything(mOutputs[i - 1], gensym("sync"), 0, nullptr);
            
            if (mSyncChecker.upwardsMode())
            {
                for (long i = 0; i < getNumIns(); i++)
                    if (isConnected(i))
                        object_method(getConnection(i).mObject, gensym("sync"));
                
                if (supportsOrderingConnections())
                    for (long i = 0; i < getNumOrderingConnections(); i++)
                        object_method(getOrderingConnection(i).mObject, gensym("sync"));
                
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
    
    static void externalMarkUnresolved(FrameLib_MaxClass *x)
    {
        x->mNeedsResolve = true;
    }
    
    static void externalAutoOrderingConnections(FrameLib_MaxClass *x)
    {
        x->mObject->autoOrderingConnections();
    }
    
    static void externalClearAutoOrderingConnections(FrameLib_MaxClass *x)
    {
        x->mObject->clearAutoOrderingConnections();
    }

    static t_ptr_int externalIsConnected(FrameLib_MaxClass *x, unsigned long index)
    {
        return x->confirmConnection(index, ConnectionInfo::kConfirm);
    }
    
    static void externalConnectionConfirm(FrameLib_MaxClass *x, unsigned long index, FrameLib_MaxGlobals::ConnectionInfo::Mode mode)
    {
        x->makeConnection(index, mode);
    }
    
    static FrameLib_Multistream *externalGetInternalObject(FrameLib_MaxClass *x)
    {
        return x->mObject.get();
    }
    
    static t_object *externalGetUserObject(FrameLib_MaxClass *x)
    {
        return x->mUserObject;
    }
    
    static t_ptr_int externalIsOutput(FrameLib_MaxClass *x)
    {
        return x->handlesAudio() && (x->getNumAudioOuts() > 1);
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
    
    FrameLib_Multistream *getInternalObject(t_object *x)
    {
        return (FrameLib_Multistream *) object_method(x, gensym("__fl.get_internal_object"));
    }
    
    // Get the number of audio ins safely from a generic pointer
    
    long getNumAudioInsRemote(t_object *x)
    {
        t_ptr_int numAudioIns = reinterpret_cast<t_ptr_int>(object_method(x, gensym("__fl.get_num_audio_ins")));
        return static_cast<long>(numAudioIns);
    }
    
    // Get the number of audio outs safely from a generic pointer

    long getNumAudioOutsRemote(t_object *x)
    {
        t_ptr_int numAudioOuts = reinterpret_cast<t_ptr_int>(object_method(x, gensym("__fl.get_num_audio_outs")));
        return static_cast<long>(numAudioOuts);
    }
    
    // Private connection methods
    
    void traversePatch(t_patcher *p, t_symbol *method, t_object *contextAssoc)
    {
        t_object *assoc = 0;
        object_method(mContextPatch, gensym("getassoc"), &assoc);
        
        // Avoid recursion into a poly / pfft / etc. - If the subpatcher is a wrapper we do need to deal with it
         
        if (assoc != contextAssoc && !object_method(assoc, gensym("__fl.wrapper_is_wrapper")))
            return;
        
        // Search for subpatchers, and call method on objects that don't have subpatchers
        
        for (t_box *b = jpatcher_get_firstobject(p); b; b = jbox_get_nextobject(b))
        {
            long index = 0;
            
            while (b && (p = (t_patcher *)object_subpatcher(jbox_get_object(b), &index, this)))
                traversePatch(p, method, contextAssoc);

            object_method(jbox_get_object(b), method);
        }
    }

    void resolveGraph(bool markUnresolved)
    {
        t_object *assoc = 0;
        object_method(mContextPatch, gensym("getassoc"), &assoc);
        
        traversePatch(mContextPatch, gensym("__fl.resolve_connections"), assoc);
        traversePatch(mContextPatch, gensym("__fl.clear_auto_ordering_connections"), assoc);
        traversePatch(mContextPatch, gensym("__fl.auto_ordering_connections"), assoc);
        if (markUnresolved)
            traversePatch(mContextPatch, gensym("__fl.mark_unresolved"), assoc);
    }
    
    void resolveConnections()
    {
        if (mNeedsResolve)
        {
            // Confirm input connections
                    
            for (long i = 0; i < getNumIns(); i++)
                confirmConnection(i, ConnectionInfo::kConfirm);
            
            // Confirm ordering connections
            
            for (long i = 0; i < getNumOrderingConnections(); i++)
                confirmConnection(getOrderingConnection(i), getNumIns(), ConnectionInfo::kConfirm);
            
            // Make output connections
            
            for (long i = getNumOuts(); i > 0; i--)
                makeConnection(i - 1, ConnectionInfo::kConnect);
            
            mNeedsResolve = false;
        }
    }

    void makeConnection(unsigned long index, ConnectionInfo::Mode mode)
    {
        ConnectionInfo info(*this, index, mode);
        
        mGlobal->setConnectionInfo(&info);
        outlet_anything(mOutputs[index], gensym("frame"), 0, nullptr);
        mGlobal->setConnectionInfo();
    }
    
    bool confirmConnection(unsigned long inIndex, ConnectionInfo::Mode mode)
    {
        if (!validInput(inIndex))
            return false;
        
        return confirmConnection(getConnection(inIndex), inIndex, mode);
    }
    
    bool confirmConnection(MaxConnection connection, unsigned long inIndex, ConnectionInfo::Mode mode)
    {
        if (!validInput(inIndex))
            return false;
        
        mConfirm = false;
        mConfirmObject = connection.mObject;
        mConfirmInIndex = inIndex;
        mConfirmOutIndex = connection.mIndex;
    
        // Check for connection *only* if the internal object is connected (otherwise assume the previously connected object has been deleted)
        
        if (mConfirmObject)
            object_method(mConfirmObject, gensym("__fl.connection_confirm"), mConfirmOutIndex, mode);
        
        if (mConfirmObject && !mConfirm)
            disconnect(mConfirmObject, mConfirmOutIndex, mConfirmInIndex);
        
        bool result = mConfirm;
        mConfirm = false;
        mConfirmObject = nullptr;
        mConfirmInIndex = mConfirmOutIndex = -1;
        
        return result;
    }
    
    bool validInput(long index, FrameLib_Multistream *object) const
    {
        return object && index >= 0 && static_cast<unsigned long>(index) < object->getNumIns();
    }
    
    bool validOutput(long index, FrameLib_Multistream *object) const
    {
        return object && index >= 0 && static_cast<unsigned long>(index) < object->getNumOuts();
        
    }
    bool isOrderingInput(long index, FrameLib_Multistream *object) const
    {
        return object && object->supportsOrderingConnections() && static_cast<unsigned long>(index) == object->getNumIns();
        
    }
    
    bool validInput(long index) const                                       { return validInput(index, mObject.get()); }
    bool validOutput(long index) const                                      { return validOutput(index, mObject.get()); }
    bool isOrderingInput(long index) const                                  { return isOrderingInput(index, mObject.get()); }
    
    bool isConnected(long index) const                                      { return mObject->isConnected(index); }
    
    MaxConnection getMaxConnection(const FrameLibConnection& connection) const
    {
        FrameLib_MaxProxy *proxy = dynamic_cast<FrameLib_MaxProxy *>(connection.mObject->getProxy());
        t_object *object = proxy->mMaxObject;
        return MaxConnection(object, connection.mIndex);
    }
    
    MaxConnection getConnection(long index) const
    {
        if (isConnected(index))
            return getMaxConnection(mObject->getConnection(index));
        else
            return MaxConnection();
    }
    
    long getNumOrderingConnections() const
    {
        return (long) mObject->getNumOrderingConnections();
    }
    
    MaxConnection getOrderingConnection(long index) const
    {
        return getMaxConnection(mObject->getOrderingConnection(index));
    }
    
    bool matchConnection(t_object *src, long outIdx, long inIdx) const
    {
        MaxConnection connection = getConnection(inIdx);
        return connection.mObject == src && connection.mIndex == outIdx;
    }
    
    void connect(t_object *src, long outIdx, long inIdx)
    {
        FrameLib_Multistream *object = getInternalObject(src);
        
        if (!isOrderingInput(inIdx) && (!validInput(inIdx) || !validOutput(outIdx, object) || matchConnection(src, outIdx, inIdx) || confirmConnection(inIdx, ConnectionInfo::kDoubleCheck)))
            return;

        ConnectionResult result;
        
        if (isOrderingInput(inIdx))
            result = mObject->addOrderingConnection(FrameLibConnection(object, outIdx));
        else
            result = mObject->addConnection(FrameLibConnection(object, outIdx), inIdx);

        switch (result)
        {
            case kConnectFeedbackDetected:
                object_error(mUserObject, "feedback loop detected");
                break;
                
            case kConnectWrongContext:
                object_error(mUserObject, "cannot connect objects from different patching contexts");
                break;
                
            case kConnectSelfConnection:
                object_error(mUserObject, "direct feedback loop detected");
                break;
                
            case kConnectSuccess:
            case kConnectNoOrderingSupport:
            case kConnectAliased:
                break;
        }
    }
    
    void disconnect(t_object *src, long outIdx, long inIdx)
    {
        FrameLib_Multistream *object = getInternalObject(src);

        if (!isOrderingInput(inIdx) && (!validInput(inIdx) || !matchConnection(src, outIdx, inIdx)))
            return;
        
        if (isOrderingInput(inIdx))
            mObject->deleteOrderingConnection(FrameLibConnection(object, outIdx));
        else
            mObject->deleteConnection(inIdx);
    }

    // Patchline connections
    
    t_max_err patchLineUpdate(t_object *patchline, long updatetype, t_object *src, long srcout, t_object *dst, long dstin)
    {
        if (*this == dst)
        {
            unwrapConnection(src, srcout);
            
            srcout -= getNumAudioOutsRemote(src);
            dstin -= getNumAudioIns();
            
            // Check load update before we check the dspchain (in case we are loading in poly~ etc.)
            
            short loadupdate = dsp_setloadupdate(false);
            dsp_setloadupdate(loadupdate);

            if (loadupdate && sys_getdspobjdspstate(*this))
            {
                if ((isOrderingInput(dstin) || validInput(dstin)) && updatetype != JPATCHLINE_ORDER)
                    dspchain_setbroken(dspchain_fromobject(*this));
            }
            else
            {
                switch (updatetype)
                {
                    case JPATCHLINE_CONNECT:        connect(src, srcout, dstin);        break;
                    case JPATCHLINE_DISCONNECT:     disconnect(src, srcout, dstin);     break;
                    case JPATCHLINE_ORDER:                                              break;
                }
            }
        }
        else
        {
            /*
            srcout -= getNumAudioOuts();
            
            if (validOutput(srcout) && == JPATCHLINE_CONNECT)
            {
                double color[4] = { 0.0, 0.0, 1.0, 1.0 };
                object_attr_setdouble_array(patchline, gensym("patchlinecolor"), 4, color);
            }
             */
        }
        
        return MAX_ERR_NONE;
    }
    
    long connectionAccept(t_object *dst, long srcout, long dstin, t_object *outlet, t_object *inlet)
    {
        t_symbol *className = object_classname(dst);
        
        if (!validOutput(srcout - getNumAudioOuts()) || className == gensym("outlet") || className == gensym("jpatcher"))
            return 1;

        unwrapConnection(dst, dstin);
        dstin -= getNumAudioInsRemote(dst);
        
        if (isOrderingInput(dstin, getInternalObject(dst)) || (validInput(dstin, getInternalObject(dst)) && !object_method(dst, gensym("__fl.is_connected"), dstin)))
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
            case kFrameNormal:          return "vector";
            case kFrameTagged:          return "tagged";
            // kFrameAny
            default:                    return "either";
        }
    }
    
    // Parameter Parsing
    
    unsigned long safeCount(char *str, unsigned long maxCount)
    {
        unsigned long number = std::max(1, atoi(str));
        return std::min(maxCount, number);
    }
    
    long getStreamCount(t_atom *a)
    {
        if (atom_gettype(a) == A_SYM)
        {
            t_symbol *sym = atom_getsym(a);
            
            if (strlen(sym->s_name) > 1 && sym->s_name[0] == '=')
                return safeCount(sym->s_name + 1, 1024);
        }
        
        return 0;
    }
    
    bool isParameterTag(t_symbol *sym)
    {        
        return strlen(sym->s_name) > 1 && sym->s_name[0] == '/';
    }
    
    bool isInputTag(t_symbol *sym)
    {
        size_t len = strlen(sym->s_name);
        
        if (len > 2)
            return (sym->s_name[0] == '[' && sym->s_name[len - 1] == ']');
        
        return false;
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
            
            if (atom_gettype(argv + idx) == A_SYM && !atom_getfloat(argv + idx))
                object_error(mUserObject, "string %s in entry list where value expected", atom_getsym(argv + idx)->s_name);
            
            values.push_back(atom_getfloat(argv + idx));
        }
        
        return idx;
    }
    
    void parseParameters(FrameLib_Parameters::AutoSerial& serialisedParameters, long argc, t_atom *argv)
    {
        t_symbol *sym = nullptr;
        std::vector<double> values;
        long i;
        
        // Parse arguments
        
        for (i = 0; i < argc; i++)
        {
            if (isTag(argv + i))
                break;
            
            if (argsMode == kAsParams)
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
                    serialisedParameters.write(sym->s_name + 1, values.data(), static_cast<unsigned long>(values.size()));
                }
            }
            else
            {
                // Advance to next tag
                
                for ( ; i < argc && !isTag(argv + i); i++);
            }
                
        }
    }
    
    // Input Parsing
    
    unsigned long inputNumber(t_symbol *sym)
    {
        return safeCount(sym->s_name + 1, 16384) - 1;
    }
    
    void parseInputs(long argc, t_atom *argv)
    {
        std::vector<double> values;
        long i = 0;
        
        // Parse arguments if used to set inputs
        
        if (argsMode == kAllInputs || argsMode == kDistribute)
        {
            i = parseNumericalList(values, argv, argc, 0);
            if (argsMode == kAllInputs)
            {
                for (long j = 0; i && j < getNumIns(); j++)
                    mObject->setFixedInput(j, values.data(), static_cast<unsigned long>(values.size()));
            }
            else
            {
                for (long j = 0; j < i && (j + 1) < getNumIns(); j++)
                    mObject->setFixedInput(j + 1, &values[j], 1);
            }
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
                mObject->setFixedInput(inputNumber(sym), values.data(), static_cast<unsigned long>(values.size()));
            }
            
            if ((i + 1) >= argc)
                break;
        }
    }

protected:
   
    unsigned long getNumStreams() { return mObject->getNumStreams(); }
    
    std::unique_ptr<FrameLib_MaxProxy> mFrameLibProxy;
    
private:
    
    // Data - N.B. - the order is crucial for safe deconstruction
    
    FrameLib_MaxGlobals::ManagedPointer mGlobal;
    FrameLib_MaxGlobals::SyncCheck mSyncChecker;
    
    std::unique_ptr<FrameLib_Multistream> mObject;
    
    std::vector<t_object *> mInputs;
    std::vector<void *> mOutputs;
    std::vector<double *> mSigOuts;

    long mProxyNum;
    t_object *mConfirmObject;
    long mConfirmInIndex;
    long mConfirmOutIndex;
    bool mConfirm;
    
    t_object *mPatch;
    t_object *mContextPatch;
    t_object *mSyncIn;
    t_object *mUserObject;
    
    bool mNeedsResolve;
};

// Convenience for Objects Using FrameLib_Expand (use FrameLib_MaxClass_Expand<T>::makeClass() to create)

template <class T, MaxObjectArgsMode argsSetAllInputs = kAsParams>
using FrameLib_MaxClass_Expand = FrameLib_MaxClass<FrameLib_Expand<T>, argsSetAllInputs>;

#endif
