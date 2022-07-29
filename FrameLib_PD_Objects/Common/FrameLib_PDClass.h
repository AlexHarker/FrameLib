
#ifndef FRAMELIB_PD_CLASS_H
#define FRAMELIB_PD_CLASS_H

#include "PDClass_Base.h"

#include "FrameLib_Global.h"
#include "FrameLib_Context.h"
#include "FrameLib_Parameters.h"
#include "FrameLib_DSP.h"
#include "FrameLib_Multistream.h"
#include "FrameLib_SerialiseGraph.h"

#include "g_canvas.h"

#include <string>
#include <vector>

extern "C"
{
    t_signal *signal_newfromcontext(int borrowed);
    void signal_makereusable(t_signal *sig);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////// PD Globals Class ////////////////////////////
//////////////////////////////////////////////////////////////////////////

class FrameLib_PDGlobals : public PDClass_Base
{
public:
    
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
        
        FrameLib_PDGlobals *mGlobal;
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
        
        FrameLib_PDGlobals *operator->() { return mPointer; }
        
    private:
        
        // Deleted
        
        ManagedPointer(const ManagedPointer&) = delete;
        ManagedPointer& operator=(const ManagedPointer&) = delete;

        FrameLib_PDGlobals *mPointer;
    };
    
    // Constructor and Destructor (public for the PD API, but use the ManagedPointer for use from outside this class)
    
    FrameLib_PDGlobals(t_symbol *sym, long ac, t_atom *av)
    : mGlobal(nullptr), mConnectionInfo(nullptr), mSyncCheck(nullptr) {}
    ~FrameLib_PDGlobals() { if (mGlobal) FrameLib_Global::release(&mGlobal); }

    // Getters and setters for max global items
    
    FrameLib_Global *getGlobal() const                          { return mGlobal; }
    
    const ConnectionInfo *getConnectionInfo() const             { return mConnectionInfo; }
    void setConnectionInfo(ConnectionInfo *info = nullptr)      { mConnectionInfo = info; }
    
    SyncCheck *getSyncCheck() const                             { return mSyncCheck; }
    void setSyncCheck(SyncCheck *check = nullptr)               { mSyncCheck = check; }
    
private:
    
    void retain()                                               { FrameLib_Global::get(&mGlobal, FrameLib_Thread::defaultPriorities()); }
    void release()                                              { FrameLib_Global::release(&mGlobal); }

    static FrameLib_PDGlobals **getPDGlobalsPtr()
    {
        return (FrameLib_PDGlobals **) &gensym("__fl.pd_global_items")->s_thing;
    }
    
    // Get and release the max global items (singleton)
    
    static FrameLib_PDGlobals *get()
    {
        // Make sure the max globals class exists

        FrameLib_PDGlobals *x = *getPDGlobalsPtr();
        
        if (!x)
        {
            makeClass<FrameLib_PDGlobals>("__fl.pd_global_items");
            x = (FrameLib_PDGlobals *) pd_new(*FrameLib_PDGlobals::getClassPointer<FrameLib_PDGlobals>());
            *getPDGlobalsPtr() = x;
        }
        
        if (x)
            x->retain();
        
        return x;
    }
    
    // Pointers
    
    FrameLib_Global *mGlobal;
    ConnectionInfo *mConnectionInfo;
    SyncCheck *mSyncCheck;
};

//////////////////////////////////////////////////////////////////////////
////////////////////// Mutator for Synchronisation ///////////////////////
//////////////////////////////////////////////////////////////////////////
/*
class Mutator : public PDClass_Base
{
public:
    
    Mutator(t_symbol *sym, long ac, t_atom *av)
    {
        mObject = ac ? atom_getobj(av) : nullptr;
        mMode = object_method(mObject, gensym("__fl.is_output")) ? FrameLib_PDGlobals::SyncCheck::kDownOnly : FrameLib_PDGlobals::SyncCheck::kDown;
    }
    
    static void classInit(t_class *c, const char *classname)
    {
        addMethod<Mutator, &Mutator::mutate>(c, "signal");
    }
    
    void mutate(t_symbol *sym, long ac, t_atom *av)
    {
        // FIX - clock_getlogicaltime??
        mSyncChecker.sync(mObject, gettime(), mMode);
        object_method(mObject, gensym("sync"));
        mSyncChecker.sync();
    }
    
private:
    
    FrameLib_PDGlobals::SyncCheck mSyncChecker;
    FrameLib_PDGlobals::SyncCheck::Mode mMode;
    void *mObject;
};
*/
//////////////////////////////////////////////////////////////////////////
////////////////////// Wrapper for Synchronisation ///////////////////////
//////////////////////////////////////////////////////////////////////////
/*
template <class T>
 class Wrapper : public PDClass_Base
{
public:
    
    // Initialise Class
    
    static t_method *sigMethodCache()
    {
        static t_method sigMethod;
        
        return &sigMethod;
    }
    
    static void classInit(t_class *c, const char *classname)
    {
        addMethod<Wrapper<T>, &Wrapper<T>::subpatcher>(c, "subpatcher");
        addMethod<Wrapper<T>, &Wrapper<T>::assist>(c, "assist");
        addMethod<Wrapper<T>, &Wrapper<T>::anything>(c, "anything");
        addMethod<Wrapper<T>, &Wrapper<T>::sync>(c, "sync");
        addMethod<Wrapper<T>, &Wrapper<T>::dsp>(c);
        addMethod(c, (t_method) &extPatchLineUpdate, "patchlineupdate");
        addMethod(c, (t_method) &extConnectionAccept, "connectionaccept");
        addMethod(c, (t_method) &extWrapperInternalObject, "__fl.wrapper_internal_object");
        
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
    
    Wrapper(t_symbol *s, long argc, t_atom *argv)
    {
        // Create patcher (you must report this as a subpatcher to get audio working)
        
        t_dictionary *d = dictionary_new();
        t_atom a;
        t_atom *av = nullptr;
        long ac = 0;
        
        atom_setparse(&ac, &av, "@defrect 0 0 300 300");
        attr_args_dictionary(d, ac, av);
        atom_setobj(&a, d);
        mPatch = (t_object *)object_new_typed(CLASS_NOBOX, gensym("jpatcher"),1, &a);
        
        // Get box text (and strip object name from the top - relace with stored name in case the object name is an alias)
        
        t_object *textfield = nullptr;
        const char *text = nullptr;
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
        
        long numIns = internal->getNumIns() + (internal->supportsOrderingConnections() ? 1 : 0);
        long numOuts = internal->getNumOuts();
        long numAudioIns = internal->getNumAudioIns();
        long numAudioOuts = internal->getNumAudioOuts();
        
        internal->mUserObject = *this;
        
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
    
    void *subpatcher(long index, void *arg)
    {
        return ((t_ptr_uint) arg > 1 && !NOGOOD(arg) && index == 0) ? (void *) mPatch : nullptr;
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
        
        for (long i = 0; i < internalOuts.size(); i++)
            std::copy(internalOuts[i], internalOuts[i] + vec_size, outs[i]);
    }
    
    void anything(t_symbol *sym, long ac, t_atom *av)
    {
        outlet_anything(mInOutlets[getInlet()], sym, ac, av);
    }
    
    // External methods (A_CANT)
    
    static t_max_err extPatchLineUpdate(Wrapper *x, t_object *patchline, long updatetype, t_object *src, long srcout, t_object *dst, long dstin)
    {
        if ((t_object *) x == dst)
            return T::extPatchLineUpdate(x->internalObject(), patchline, updatetype, src, srcout, x->mObject, dstin + 1);
        else
            return T::extPatchLineUpdate(x->internalObject(), patchline, updatetype, x->mObject, srcout + 1, dst, dstin);
    }
    
    static t_ptr_int extConnectionAccept(Wrapper *src, t_object *dst, long srcout, long dstin, t_object *outlet, t_object *inlet)
    {
        // Only called for sources / account for internal sync connections

        return T::extConnectionAccept(src->internalObject(), dst, srcout + 1, dstin, outlet, inlet);
    }
    
    static void *extWrapperInternalObject(Wrapper *x)
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
    
    std::vector<t_object *> mInOutlets;
    std::vector<t_object *> mProxyIns;
    
    // Outlets (don't need to free)
    
    std::vector<t_object *> mAudioOuts;
    std::vector<t_object *> mOuts;
    
    // Dummy for stuffloc on proxies
    
    long mProxyNum;
};
*/
//////////////////////////////////////////////////////////////////////////
/////////////////////// FrameLib PD Object Class /////////////////////////
//////////////////////////////////////////////////////////////////////////

enum PDObjectArgsMode { kAsParams, kAllInputs, kDistribute };

struct FrameLib_PDProxy : public virtual FrameLib_Proxy
{
    t_object *mMaxObject;
};

template <class T, PDObjectArgsMode argsMode = kAsParams>
class FrameLib_PDClass : public PDClass_Base
{
    typedef FrameLib_Object<FrameLib_Multistream>::Connection FrameLibConnection;
    typedef FrameLib_Object<t_object>::Connection PDConnection;
    typedef FrameLib_PDGlobals::ConnectionInfo ConnectionInfo;

    using ClipMode = FrameLib_Parameters::ClipMode;
    using NumericType = FrameLib_Parameters::NumericType;
    
    static t_atomtype atom_gettype(t_atom* a) { return a->a_type; }
    
    struct PDProxy : public PDClass_Base
    {
        static t_pd *create(FrameLib_PDClass *owner, int index)
        {
            t_pd *proxy = pd_new(*PDProxy::getClassPointer<PDProxy>());
           
            ((PDProxy *)proxy)->mOwner = owner;
            ((PDProxy *)proxy)->mIndex = index;

            return proxy;
        }
        
        static void classInit(t_class *c, const char *classname)
        {
            addMethod<PDProxy, &PDProxy::frame>(c, "frame");
        }
        
        PDProxy(t_symbol *sym, long ac, t_atom *av) : mOwner(nullptr), mIndex(-1) {}
        
        void frame()
        {
            mOwner->frameInlet(mIndex);
        }
        
        FrameLib_PDClass *mOwner;
        int mIndex;
    };
    
public:
    
    // Class Initialisation (must explicitly give U for classes that inherit from FrameLib_PDClass<>)
    
    template <class U = FrameLib_PDClass<T, argsMode>>
    static void makeClass(const char *className)
    {
        // If handles audio/scheduler then make wrapper class and name the inner object differently..
        
        std::string internalClassName = className;
        std::string proxyClassName;

        if (T::sHandlesAudio)
        {
            //Wrapper<U>:: template makeClass<Wrapper<U>>(CLASS_BOX, className);
            internalClassName.insert(0, "unsynced.");
        }
        
        proxyClassName.append(".proxy");
        PDClass_Base::makeClass<U>(internalClassName.c_str());
        PDClass_Base::makeClass<PDProxy>(proxyClassName.c_str());
    }
    
    static void classInit(t_class *c, const char *classname)
    {
        addMethod<FrameLib_PDClass<T>, &FrameLib_PDClass<T>::info>(c, "info");
        addMethod<FrameLib_PDClass<T>, &FrameLib_PDClass<T>::frame>(c, "frame");
        addMethod<FrameLib_PDClass<T>, &FrameLib_PDClass<T>::sync>(c, "sync");
        addMethod<FrameLib_PDClass<T>, &FrameLib_PDClass<T>::dsp>(c);
        addMethod(c, (t_method) &extResolveConnections, "__fl.resolve_connections");
        addMethod(c, (t_method) &extAutoOrdering, "__fl.auto_ordering_connections");
        addMethod(c, (t_method) &extClearAutoOrdering, "__fl.clear_auto_ordering_connections");
        addMethod(c, (t_method) &extIsConnected, "__fl.is_connected");
        addMethod(c, (t_method) &extConnectionConfirm, "__fl.connection_confirm");
        addMethod(c, (t_method) &extGetInternalObject, "__fl.get_internal_object");
        addMethod(c, (t_method) &extIsOutput, "__fl.is_output");
        addMethod(c, (t_method) &extGetNumAudioIns, "__fl.get_num_audio_ins");
        addMethod(c, (t_method) &extGetNumAudioOuts, "__fl.get_num_audio_outs");
        class_addmethod(c, (t_method) &codeexport, gensym("export"), A_SYMBOL, A_SYMBOL, 0);

        dspInit(c);
    }

    // Tag checks
    
    static bool isParameterTag(t_symbol *sym)
    {
        return strlen(sym->s_name) > 1 && sym->s_name[0] == '/';
    }
    
    static bool isInputTag(t_symbol *sym)
    {
        size_t len = strlen(sym->s_name);
        
        if (len > 2)
            return (sym->s_name[0] == '[' && sym->s_name[len - 1] == ']');
        
        return false;
    }
    
    static bool isTag(t_atom *a)
    {
        t_symbol *sym = atom_getsymbol(a);
        return atom_gettype(a) == A_SYMBOL && (isParameterTag(sym) || isInputTag(sym));
    }
    
    // Constructor and Destructor

    FrameLib_PDClass(t_symbol *s, long argc, t_atom *argv, FrameLib_PDProxy *proxy = new FrameLib_PDProxy())
    : mFrameLibProxy(proxy)
    , mConfirmObject(nullptr)
    , mConfirmInIndex(-1)
    , mConfirmOutIndex(-1)
    , mConfirm(false)
    , mCanvas(canvas_getcurrent())
    , mSyncIn(nullptr)
    , mNeedsResolve(true)
    , mUserObject(*this)
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
        mObject.reset(new T(FrameLib_Context(mGlobal->getGlobal(), mCanvas), &serialisedParameters, mFrameLibProxy.get(), nStreams));
        parseInputs(argc, argv);
        
        long numIns = getNumIns() + (supportsOrderingConnections() ? 1 : 0);

        mInputs.resize(numIns);
        mOutputs.resize(getNumOuts());
        
        dspSetup(getNumAudioIns(), getNumAudioOuts());
        
        // Create frame inlets - N.B. - we create a proxy if the inlet is not the first inlet (not the first frame input or the object handles audio)

        for (long i = 0; i < numIns; i++)
        {
            if (i || handlesAudio())
            {
                mInputs[i] = PDProxy::create(this, (int) (getNumAudioIns() + i));
                inlet_new(*this, mInputs[i], gensym("frame"), gensym("frame"));
            }
            else
                mInputs[i] = nullptr;
        }
        
        // Create frame outlets
        
        for (unsigned long i = 0; i < getNumOuts(); i++)
            mOutputs[i] = outlet_new(*this, gensym("frame"));
        
        // Add a sync outlet if we need to handle audio
        
        if (handlesAudio())
        {
            //mSyncIn = (t_object *) outlet_new(nullptr, nullptr);
            //outlet_add(mSyncIn, inlet_nth(*this, 0));
        }
    }

    ~FrameLib_PDClass()
    {
        for (auto it = mInputs.begin(); it != mInputs.end(); it++)
            if (*it)
                pd_free(*it);
      
        //object_free(mSyncIn);
    }
    
    static void codeexport(FrameLib_PDClass *x, t_symbol *className, t_symbol *path)
    {
        char conformedPath[MAXPDSTRING];

        if (strlen(path->s_name) > MAXPDSTRING)
            conformedPath[0] = 0;
        else
            sys_bashfilename(path->s_name, conformedPath);
        
        ExportError error = exportGraph(x->mObject.get(), conformedPath, className->s_name);
        
        if (error == ExportError::PathError)
            pd_error(x->mUserObject, "couldn't write to or find specified path");
        else if (error == ExportError::WriteError)
            pd_error(x->mUserObject, "couldn't write file");
    }
    
    void info(t_symbol *sym, long ac, t_atom *av)
    {
        // Determine what to post
        
        enum InfoFlags { kInfoDesciption = 0x01, kInfoInputs = 0x02, kInfoOutputs = 0x04, kInfoParameters = 0x08 };
        bool verbose = true;
        long flags = 0;
        
        while (ac--)
        {
            t_symbol *type = atom_getsymbol(av++);
            
            if (type == gensym("description"))          flags |= kInfoDesciption;
            else if (type == gensym("inputs"))          flags |= kInfoInputs;
            else if (type == gensym("outputs"))         flags |= kInfoOutputs;
            else if (type == gensym("io"))              flags |= kInfoInputs | kInfoOutputs;
            else if (type == gensym("parameters"))      flags |= kInfoParameters;
            else if (type == gensym("quick"))           verbose = false;
        }
        
        flags = !flags ? (kInfoDesciption | kInfoInputs | kInfoOutputs | kInfoParameters) : flags;
        
        // Start Tag
        
        // FIX - user object should be used here....
        
        post("********* %s *********", class_getname(*getClassPointer<FrameLib_PDClass>()));

        // Description
        
        if (flags & kInfoDesciption)
        {
            post("--- Description ---");
            postSplit(mObject->objectInfo(verbose).c_str(), "", "-");
        }
        
        // IO
        
        if (flags & kInfoInputs)
        {
            post("--- Input List ---");
            if (argsMode == kAllInputs)
                post("N.B. - arguments set the fixed array values for all inputs.");
            if (argsMode == kDistribute)
                post("N.B - arguments are distributed one per input.");
            for (long i = 0; i < mObject->getNumAudioIns(); i++)
                post("Audio Input %ld: %s", i + 1, mObject->audioInfo(i, verbose).c_str());
            for (long i = 0; i < mObject->getNumIns(); i++)
                post("Frame Input %ld [%s]: %s", i + 1, frameTypeString(mObject->inputType(i)), mObject->inputInfo(i, verbose).c_str());
            if (supportsOrderingConnections())
                post("Ordering Input [%s]: Connect to ensure ordering", frameTypeString(FrameType::Any));
        }
        
        if (flags & kInfoOutputs)
        {
            post("--- Output List ---");
            for (long i = 0; i < mObject->getNumAudioOuts(); i++)
                post("Audio Output %ld: %s", i + 1, mObject->audioInfo(i, verbose).c_str());
            for (long i = 0; i < mObject->getNumOuts(); i++)
                post("Frame Output %ld [%s]: %s", i + 1, frameTypeString(mObject->outputType(i)), mObject->outputInfo(i, verbose).c_str());
        }
        
        // Parameters
        
        if (flags & kInfoParameters)
        {
            post("--- Parameter List ---");
            
            const FrameLib_Parameters *params = mObject->getParameters();
            if (!params || !params->size()) post("< No Parameters >");
            
            // Loop over parameters
            
            for (long i = 0; params && i < params->size(); i++)
            {
                FrameLib_Parameters::Type type = params->getType(i);
                FrameLib_Parameters::NumericType numericType = params->getNumericType(i);
                std::string defaultStr = params->getDefaultString(i);

                // Name, type and default value
                
                if (defaultStr.size())
                    post("Parameter %ld: %s [%s] (default: %s)", i + 1, params->getName(i), params->getTypeString(i).c_str(), defaultStr.c_str());
                else
                    post("Parameter %ld: %s [%s]", i + 1, params->getName(i), params->getTypeString(i).c_str());

                // Verbose - arguments, range (for numeric types), enum items (for enums), array sizes (for arrays), description
                
                if (verbose)
                {
                    if (argsMode == kAsParams && params->getArgumentIdx(i) >= 0)
                        post("- Argument: %ld", params->getArgumentIdx(i) + 1);
                    if (numericType == NumericType::Integer || numericType == NumericType::Double)
                    {
                        switch (params->getClipMode(i))
                        {
                            case ClipMode::None:    break;
                            case ClipMode::Min:     post("- Min Value: %lg", params->getMin(i));                        break;
                            case ClipMode::Max:     post("- Max Value: %lg", params->getMax(i));                        break;
                            case ClipMode::Clip:    post("- Clipped: %lg-%lg", params->getMin(i), params->getMax(i));   break;
                        }
                    }
                    if (type == FrameLib_Parameters::Type::Enum)
                        for (long j = 0; j <= params->getMax(i); j++)
                            post("   [%ld] - %s", j, params->getItemString(i, j));
                    else if (type == FrameLib_Parameters::Type::Array)
                        post("- Array Size: %ld", params->getArraySize(i));
                    else if (type == FrameLib_Parameters::Type::VariableArray)
                        post("- Array Max Size: %ld", params->getArrayMaxSize(i));
                    postSplit(params->getInfo(i).c_str(), "- ", "-");
                }
            }
        }
    }

    // Helpers (IO / context / mode / streams)
    
    ObjectType getType() const                  { return mObject->getType(); }
    
    FrameLib_Context getContext() const         { return mObject->getContext(); }

    bool handlesAudio() const                   { return T::sHandlesAudio; }
    bool supportsOrderingConnections() const    { return mObject->supportsOrderingConnections(); }

    long audioIOSize(long chans) const          { return chans + (handlesAudio() ? 1 : 0); }

    long getNumIns() const                      { return static_cast<long>(mObject->getNumIns()); }
    long getNumOuts() const                     { return static_cast<long>(mObject->getNumOuts()); }
    long getNumAudioIns() const                 { return audioIOSize(mObject->getNumAudioIns()); }
    long getNumAudioOuts() const                { return audioIOSize(mObject->getNumAudioOuts()); }
    
    // Perform and DSP

    void perform(int vec_size)
    {
        // FIX - use alloca
        
        // Copy Audio In
        
        for (int i = 0; i < (getNumAudioIns() - 1); i++)
            for (int j = 0; j < vec_size; j++)
                mSigIns[i][j] = getAudioIn(i + 1)[j];
        
        // N.B. Plus one due to sync inputs
        
        mObject->blockUpdate(mSigIns.data(), mSigOuts.data(), vec_size);
        
        for (int i = 0; i < (getNumAudioOuts() - 1); i++)
            for (int j = 0; j < vec_size; j++)
                getAudioOut(i + 1)[j] = mSigOuts[i][j];
    }

    void dsp(t_signal **sp)
    {
        // Resolve connections (in case there are no schedulers left in the patch) and mark unresolved for next time
        
        iterateCanvas(mCanvas, gensym("__fl.resolve_connections"));
        //resolveConnections();
        mNeedsResolve = true;
        
        // Reset DSP
        
        t_signal *temp = signal_newfromcontext(0);
        double samplingRate = temp->s_sr;
        int vec_size = temp->s_vecsize;
        signal_makereusable(temp);
        
        mObject->reset(samplingRate, vec_size);
    
        // Add a perform routine to the chain if the object handles audio
        
        if (handlesAudio())
        {
            addPerform<FrameLib_PDClass, &FrameLib_PDClass<T>::perform>(sp);
        
            mTemp.resize(vec_size * (getNumAudioIns() + getNumAudioOuts() -2));
            mSigIns.resize(getNumAudioIns() - 1);
            mSigOuts.resize(getNumAudioOuts() - 1);
        
            double *inVecs = mTemp.data();
            double *outVecs = inVecs + ((getNumAudioIns() - 1) * vec_size);
        
            for (int i = 0; i < getNumAudioIns() - 1; i++)
                mSigIns[i] = inVecs + (i * vec_size);
            for (int i = 0; i < getNumAudioOuts() - 1; i++)
                mSigOuts[i] = outVecs + (i * vec_size);
        }
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
        FrameLib_PDGlobals::SyncCheck::Action action = mSyncChecker(this, handlesAudio(), extIsOutput(this));
       
        if (action != FrameLib_PDGlobals::SyncCheck::kSyncComplete && handlesAudio() && mNeedsResolve)
        {
            iterateCanvas(mCanvas, gensym("__fl.resolve_connections"));
            iterateCanvas(mCanvas, gensym("__fl.clear_auto_ordering_connections"));
            iterateCanvas(mCanvas, gensym("__fl.auto_ordering_connections"));
        }
        
        // FIX
        
        //if (action == FrameLib_PDGlobals::SyncCheck::kAttachAndSync)
        //    outlet_anything(mSyncIn, gensym("signal"), 0, nullptr);
        
        if (action != FrameLib_PDGlobals::SyncCheck::kSyncComplete)
        {
            for (unsigned long i = getNumOuts(); i > 0; i--)
                outlet_anything(mOutputs[i - 1], gensym("sync"), 0, nullptr);
            
            if (mSyncChecker.upwardsMode())
            {
                for (unsigned long i = 0; i < getNumIns(); i++)
                    if (isConnected(i))
                        callMethod(getConnection(i).mObject, gensym("sync"));
                
                if (supportsOrderingConnections())
                    for (unsigned long i = 0; i < getNumOrderingConnections(); i++)
                        callMethod(getOrderingConnection(i).mObject, gensym("sync"));
                
                mSyncChecker.restoreMode();
            }
        }
    }
    
    // Connection Routines

    void frame()
    {
        frameInlet(0);
    }
    
    void frameInlet(long index)
    {
        const ConnectionInfo *info = mGlobal->getConnectionInfo();
        index -= getNumAudioIns();
        
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
                        pd_error(mUserObject, "extra connection to input %ld", index + 1);
                }
                break;
        }
    }

    // External methods (A_CANT)
    
    static void extResolveConnections(FrameLib_PDClass *x)
    {
        x->resolveConnections();
    }
                               
    static void extAutoOrdering(FrameLib_PDClass *x)
    {
        x->mObject->makeAutoOrderingConnections();
    }
    
    static void extClearAutoOrdering(FrameLib_PDClass *x)
    {
        x->mObject->clearAutoOrderingConnections();
    }

    static uintptr_t extIsConnected(FrameLib_PDClass *x, unsigned long index)
    {
        return x->confirmConnection(index, ConnectionInfo::kConfirm);
    }
    
    static void extConnectionConfirm(FrameLib_PDClass *x, unsigned long index, FrameLib_PDGlobals::ConnectionInfo::Mode mode)
    {
        x->makeConnection(index, mode);
    }
    
    static FrameLib_Multistream *extGetInternalObject(FrameLib_PDClass *x)
    {
        return x->mObject.get();
    }
    
    static uintptr_t extIsOutput(FrameLib_PDClass *x)
    {
        return x->handlesAudio() && (x->getNumAudioOuts() > 1);
    }
    
    static uintptr_t extGetNumAudioIns(FrameLib_PDClass *x)
    {
        return x->getNumAudioIns();
    }
    
    static uintptr_t extGetNumAudioOuts(FrameLib_PDClass *x)
    {
        return x->getNumAudioOuts();
    }
    
    static uintptr_t intMethod(t_object *object, t_symbol *methodName)
    {
        // FIX - look at vmess
        
        typedef uintptr_t (*func)(t_object *);

        t_gotfn f = zgetfn(&object->ob_pd, methodName);
        
        if (!f)
            return 0;
        
        func f2 = (func)f;
        return f2(object);
    }
    
    static void *ptrMethod(t_object *object, t_symbol *methodName)
    {
        // FIX - look at vmess

        typedef void *(*func)(t_object *);
        
        t_gotfn f = zgetfn(&object->ob_pd, methodName);
        
        if (!f)
            return 0;
        
        func f2 = (func)f;
        return f2(object);
    }
    
    static void callMethod(t_object *object, t_symbol *methodName)
    {
        //vmess((t_pd *) g, method, "");

        // FIX - look at vmess
        
        typedef void (*func)(t_object *);
        
        t_gotfn f = zgetfn(&object->ob_pd, methodName);
        
        if (!f)
            return;
        
        func f2 = (func)f;
        return f2(object);
    }
    
    static void confirmMethod(t_object *object, t_symbol *methodName, long index, ConnectionInfo::Mode mode)
    {
        //vmess((t_pd *) g, method, "");
        
        // FIX - look at vmess
        
        typedef void (*func)(t_object *, long, ConnectionInfo::Mode);
        
        t_gotfn f = zgetfn(&object->ob_pd, methodName);
        
        if (!f)
            return;
        
        func f2 = (func)f;
        return f2(object, index, mode);
    }
    
private:
    
    // Unwrapping connections
    
    void unwrapConnection(t_object *& object, long& connection)
    {
        /*t_object *wrapped = (t_object *) object_method(object, gensym("__fl.wrapper_internal_object"));
        
        if (wrapped)
        {
            object = wrapped;
            connection++;
        }*/
    }
    
    // Get an internal object from a generic pointer safely
    
    FrameLib_Multistream *getInternalObject(t_object *x)
    {
        return (FrameLib_Multistream *) ptrMethod(x, gensym("__fl.get_internal_object"));
    }
    
    // Private connection methods
    
    void iterateCanvas(t_glist *gl, t_symbol *method)
    {        
        // Search for subpatchers, and call method on objects that don't have subpatchers
        
        for (t_gobj *g = gl->gl_list; g; g = g->g_next)
        {
            if (zgetfn((t_pd *) g, method))
                mess0((t_pd *) g, method);
        }
    }

    void resolveConnections()
    {
        if (mNeedsResolve)
        {
            // Confirm input connections
                    
            for (unsigned long i = 0; i < getNumIns(); i++)
                confirmConnection(i, ConnectionInfo::kConfirm);
            
            // Confirm ordering connections
            
            for (unsigned long i = 0; i < getNumOrderingConnections(); i++)
                confirmConnection(getOrderingConnection(i), getNumIns(), ConnectionInfo::kConfirm);
            
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
        outlet_anything(mOutputs[index], gensym("frame"), 0, nullptr);
        mGlobal->setConnectionInfo();
    }
    
    bool confirmConnection(unsigned long inIndex, ConnectionInfo::Mode mode)
    {
        if (!validInput(inIndex))
            return false;
        
        return confirmConnection(getConnection(inIndex), inIndex, mode);
    }
    
    bool confirmConnection(PDConnection connection, unsigned long inIndex, ConnectionInfo::Mode mode)
    {
        if (!validInput(inIndex))
            return false;
        
        mConfirm = false;
        mConfirmObject = connection.mObject;
        mConfirmInIndex = inIndex;
        mConfirmOutIndex = connection.mIndex;
    
        // Check for connection *only* if the internal object is connected (otherwise assume the previously connected object has been deleted)
        
        if (mConfirmObject)
            confirmMethod(mConfirmObject, gensym("__fl.connection_confirm"), mConfirmOutIndex, mode);
        
        if (mConfirmObject && !mConfirm)
            disconnect(mConfirmObject, mConfirmOutIndex, mConfirmInIndex);
        
        bool result = mConfirm;
        mConfirm = false;
        mConfirmObject = nullptr;
        mConfirmInIndex = mConfirmOutIndex = -1;
        
        return result;
    }
    
    bool validInput(long index, FrameLib_Multistream *object) const         { return object && index >= 0 && index < object->getNumIns(); }
    bool validOutput(long index, FrameLib_Multistream *object) const        { return object && index >= 0 && index < object->getNumOuts(); }
    bool isOrderingInput(long index, FrameLib_Multistream *object) const    { return object && object->supportsOrderingConnections() && index == object->getNumIns(); }
    bool validInput(long index) const                                       { return validInput(index, mObject.get()); }
    bool validOutput(long index) const                                      { return validOutput(index, mObject.get()); }
    bool isOrderingInput(long index) const                                  { return isOrderingInput(index, mObject.get()); }
    
    bool isConnected(long index) const                                      { return mObject->isConnected(index); }
    
    PDConnection getPDConnection(const FrameLibConnection& connection) const
    {
        FrameLib_PDProxy *proxy = dynamic_cast<FrameLib_PDProxy *>(connection.mObject->getProxy());
        t_object *object = proxy->mMaxObject;
        return PDConnection(object, connection.mIndex);
    }
    
    PDConnection getConnection(long index) const
    {
        if (isConnected(index))
            return getPDConnection(mObject->getConnection(index));
        else
            return PDConnection();
    }
    
    unsigned long getNumOrderingConnections() const
    {
        return mObject->getNumOrderingConnections();
    }
    
    PDConnection getOrderingConnection(long index) const
    {
        return getPDConnection(mObject->getOrderingConnection(index));
    }
    
    bool matchConnection(t_object *src, long outIdx, long inIdx) const
    {
        PDConnection connection = getConnection(inIdx);
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
            case ConnectionResult::FeedbackDetected:
                pd_error(mUserObject, "feedback loop detected");
                break;
                
            case ConnectionResult::WrongContext:
                pd_error(mUserObject, "cannot connect objects from different top-level patchers");
                break;
                
            case ConnectionResult::SelfConnection:
                pd_error(mUserObject, "direct feedback loop detected");
                break;
                
            case ConnectionResult::Success:
            case ConnectionResult::NoOrderingSupport:
            case ConnectionResult::Aliased:
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

    // Info Utilities
    
    void postSplit(const char *text, const char *firstLineTag, const char *lineTag)
    {
        std::string str(text);
        size_t prev = 0;
        
        for (size_t pos = str.find_first_of(":."); prev < str.size(); pos = str.find_first_of(":.", pos + 1))
        {
            if ((pos != std::string::npos) && (str[pos + 1] != ' '))
                continue;
            
            pos = pos == std::string::npos ? str.size() : pos;
            post("%s%s", prev ? lineTag : firstLineTag, str.substr(prev, (pos - prev) + 1).c_str());
            prev = pos + 1;
        }
    }
    
    const char *frameTypeString(FrameType type)
    {
        switch (type)
        {
            case FrameType::Vector:     return "vector";
            case FrameType::Tagged:     return "tagged";
            // kFrameAny
            default:                    return "either";
        }
    }
    
    // Parameter Parsing
    
    static unsigned long safeCount(char *str, unsigned long maxCount)
    {
        unsigned long number = std::max(1, atoi(str));
        return std::min(maxCount, number);
    }
    
    static long getStreamCount(t_atom *a)
    {
        if (atom_gettype(a) == A_SYMBOL)
        {
            t_symbol *sym = atom_getsymbol(a);
            
            if (strlen(sym->s_name) > 1 && sym->s_name[0] == '=')
                return safeCount(sym->s_name + 1, 1024);
        }
        
        return 0;
    }
    
    long parseNumericalList(std::vector<double> &values, t_atom *argv, long argc, long idx)
    {
        values.resize(0);
        
        // Collect doubles
        
        for ( ; idx < argc && !isTag(argv + idx); idx++)
        {
            values.push_back(atom_getfloat(argv + idx));

            if (atom_gettype(argv + idx) == A_SYMBOL && !values.back())
                pd_error(mUserObject, "string %s in entry list where value expected", atom_getsymbol(argv + idx)->s_name);
        }
        
        return idx;
    }
    
    void parseParameters(FrameLib_Parameters::AutoSerial& serialisedParameters, long argc, t_atom *argv)
    {
        t_symbol *sym = nullptr;
        std::vector<double> values;
        long i;
        
        // Parse arguments
        
        for (i = 0; i < argc && !isTag(argv + i); i++)
        {
            if (argsMode == kAsParams)
            {
                if (atom_gettype(argv + i) == A_SYMBOL)
                {
                    t_symbol *str = atom_getsymbol(argv + i);
                    serialisedParameters.write(std::to_string(i).c_str(), str->s_name);
                }
                else
                {
                    double value = atom_getfloat(argv + i);
                    serialisedParameters.write(std::to_string(i).c_str(), &value, 1);
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
                    sym = atom_getsymbol(argv + i);
                    break;
                }
                
                if (j == 0)
                    pd_error(mUserObject, "stray items after entry %s", sym->s_name);
            }
            
            // Check for lack of values or end of list
            
            if ((++i >= argc) || isTag(argv + i))
            {
                if (i < (argc + 1))
                    pd_error(mUserObject, "no values given for entry %s", sym->s_name);
                continue;
            }
            
            if (isParameterTag(sym))
            {
                // Do strings or values
                
                if (atom_gettype(argv + i) == A_SYMBOL && atom_getsymbol(argv + i))
                    serialisedParameters.write(sym->s_name + 1, atom_getsymbol(argv + i++)->s_name);
                else
                {
                    i = parseNumericalList(values, argv, argc, i);
                    serialisedParameters.write(sym->s_name + 1, values.data(), static_cast<unsigned long>(values.size()));
                }
            }
        }
    }
    
    // Input Parsing
    
    static unsigned long inputNumber(t_symbol *sym)
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
        
        // Parse input tags
        
        while (i < argc)
        {
            // Advance to next input tag
            
            for ( ; i < argc && !isInputTag(atom_getsymbol(argv + i)); i++);
            
            // If there are values to read then do so
            
            if ((i + 1) < argc && !isTag(argv + i + 1))
            {
                t_symbol *sym = atom_getsymbol(argv + i);
                i = parseNumericalList(values, argv, argc, i + 1);
                mObject->setFixedInput(inputNumber(sym), values.data(), values.size());
            }
        }
    }

protected:
    
    std::unique_ptr<FrameLib_PDProxy> mFrameLibProxy;
    
private:

    // Data - N.B. - the order is crucial for safe deconstruction
    
    FrameLib_PDGlobals::ManagedPointer mGlobal;
    FrameLib_PDGlobals::SyncCheck mSyncChecker;

    std::unique_ptr<FrameLib_Multistream> mObject;
    
    std::vector<t_pd *> mInputs;
    std::vector<t_outlet *> mOutputs;
    
    std::vector<double *> mSigIns;
    std::vector<double *> mSigOuts;
    std::vector<double> mTemp;
    
    long mProxyNum;
    t_object *mConfirmObject;
    long mConfirmInIndex;
    long mConfirmOutIndex;
    bool mConfirm;
    
    t_glist *mCanvas;
    t_object *mSyncIn;
    
    bool mNeedsResolve;
    
public:

    t_object *mUserObject;
};

// Convenience for Objects Using FrameLib_Expand (use FrameLib_PDClass_Expand<T>::makeClass() to create)

template <class T, PDObjectArgsMode argsMode = kAsParams>
using FrameLib_PDClass_Expand = FrameLib_PDClass<FrameLib_Expand<T>, argsMode>;

#endif
