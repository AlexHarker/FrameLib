
#ifndef FRAMELIB_PD_CLASS_H
#define FRAMELIB_PD_CLASS_H

#include "PDClass_Base.h"

#include "g_canvas.h"

#include "FrameLib_Global.h"
#include "FrameLib_Context.h"
#include "FrameLib_Parameters.h"
#include "FrameLib_DSP.h"
#include "FrameLib_Multistream.h"
#include "FrameLib_SerialiseGraph.h"

#include <algorithm>
#include <cctype>
#include <deque>
#include <queue>
#include <string>
#include <tuple>
#include <vector>
#include <unordered_map>

extern "C"
{
    t_signal *signal_newfromcontext(int borrowed);
    void signal_makereusable(t_signal *sig);
}

// Deal with private strings and versioning

struct FrameLib_PDPrivate
{
    // A constant representing the pd wrapper version.
    // The version is stored as a decimal with major version changes as multiples of 1000
    
    static inline constexpr uint32_t pdWrapperVersion()
    {
        return 1000;
    }
    
    // A constant representing the max wrapper and framework versions.
    // The versions are combined in a manner that they are both easily readable when viewed in decimal format
    
    static inline constexpr uint64_t version()
    {
        return static_cast<uint64_t>(FrameLib_FrameworkVersion) * 1000000000 + static_cast<uint64_t>(pdWrapperVersion());
    }
    
    struct VersionString
    {
        VersionString(const char *str) : mString(str)
        {
            mString.append(".v");
            mString.append(std::to_string(version()));
        }
        
        // Non-copyable but moveable
        
        VersionString(const VersionString&) = delete;
        VersionString& operator=(const VersionString&) = delete;
        VersionString(VersionString&&) = default;
        VersionString& operator=(VersionString&&) = default;
        
        operator const char *() { return mString.c_str(); }
        
        std::string mString;
    };
    
    static inline t_symbol *FLNamespace()                   { return gensym("__fl.framelib_private"); }
    static inline t_symbol *globalTag()                     { return gensym(VersionString("__fl.pd_global_tag")); }
    
    static inline VersionString objectGlobal()              { return "__fl.pd_global_items"; }
    static inline VersionString objectMessageHandler()      { return "__fl.message.handler"; }
    static inline VersionString objectMutator()             { return "__fl.signal.mutator"; }
    
    static inline const char *messageGetUserObject()        { return "__fl.get_user_object"; }
    static inline const char *messageUnwrap()               { return "__fl.unwrap"; }
    static inline const char *messageIsWrapper()            { return "__fl.is_wrapper"; }
    static inline const char *messageFindAudioObjects()     { return "__fl.find_audio_objects"; }
    static inline const char *messageResolveContext()       { return "__fl.resolve_context"; }
    static inline const char *messageResolveConnections()   { return "__fl.resolve_connections"; }
    static inline const char *messageMarkUnresolved()       { return "__fl.mark_unresolved"; }
    static inline const char *messageMakeAutoOrdering()     { return "__fl.make_auto_ordering"; }
    static inline const char *messageClearAutoOrdering()    { return "__fl.clear_auto_ordering"; }
    static inline const char *messageReset()                { return "__fl.reset"; }
    static inline const char *messageIsDirectlyConnected()  { return "__fl.is_directly_connected"; }
    static inline const char *messageConnectionConfirm()    { return "__fl.connection_confirm"; }
    static inline const char *messageConnectionUpdate()     { return "__fl.connection_update"; }
    static inline const char *messageGetFrameLibObject()    { return "__fl.get_framelib_object"; }
    static inline const char *messageGetNumAudioIns()       { return "__fl.get_num_audio_ins"; }
    static inline const char *messageGetNumAudioOuts()      { return "__fl.get_num_audio_outs"; }
    
    static FrameLib_Multistream *toFrameLibObject(t_object *object, uint64_t& versionCheck)
    {
        if (!object)
            return nullptr;
        
        return PDClass_Base::objectMethod<FrameLib_Multistream *>(object, messageGetFrameLibObject(), &versionCheck);
    }
    
    static FrameLib_Multistream *toFrameLibObject(t_object *object)
    {
        uint64_t versionCheck = 0;
        FrameLib_Multistream *internalObject = toFrameLibObject(object, versionCheck);
        return versionCheck == version() ? internalObject : nullptr;
    }
    
    static bool versionMismatch(t_object *object)
    {
        uint64_t versionCheck = 0;
        toFrameLibObject(object, versionCheck);
        return versionCheck && versionCheck != version();
    }
};

// Other pd-specific structures

struct FrameLib_PDProxy : public virtual FrameLib_Proxy
{
    FrameLib_PDProxy(t_object *x)
    {
        setOwner(x);
    }
    
    // Override for object proxies that need to know about the patch hierarchy
    
    //virtual void contextPatchUpdated(t_object *patch, unsigned long depth) {}
};

struct FrameLib_PDMessageProxy : FrameLib_PDProxy
{
    FrameLib_PDMessageProxy(t_object *x) : FrameLib_PDProxy(x) {}
    
    // Override for objects that require max messages to be sent from framelib
    
    virtual void sendMessage(unsigned long stream, t_symbol *s, short ac, t_atom *av) {}
    
    // Store context patch info
    
    //void contextPatchUpdated(t_object *patch, unsigned long depth) override { mDepth = depth; }
};

struct FrameLib_PDNRTAudio
{
    FrameLib_Multistream *mObject;
    t_symbol *mBuffer;
};

struct FrameLib_PDContext
{
    bool mRealtime;
    t_object *mPatch;
    t_symbol *mName;
    
    bool operator == (const FrameLib_PDContext& b) const
    {
        return mRealtime == b.mRealtime && mPatch == b.mPatch && mName == b.mName;
    }
};

//////////////////////////////////////////////////////////////////////////
/////////////////////////// Messaging Classes ////////////////////////////
//////////////////////////////////////////////////////////////////////////
/*
struct MessageInfo
{
    MessageInfo(FrameLib_PDMessageProxy* proxy, FrameLib_TimeFormat time, unsigned long stream)
    : mProxy(proxy), mTime(time), mStream(stream) {}
    
    FrameLib_PDMessageProxy* mProxy;
    FrameLib_TimeFormat mTime;
    unsigned long mStream;
};

struct Message
{
    Message(const MessageInfo& info, const double *values, unsigned long N)
    : mInfo(info), mType(FrameType::Vector), mVector(values, values + N) {}
    
    Message(const MessageInfo& info, const FrameLib_Parameters::Serial *serial)
    : mInfo(info), mType(FrameType::Tagged), mSerial(*serial) {}
    
    Message(const Message&) = delete;
    Message& operator=(const Message&) = delete;
    Message(Message&&) = default;
    Message& operator=(Message&&) = default;
    
    MessageInfo mInfo;
    FrameType mType;
    std::vector<double> mVector;
    FrameLib_Parameters::AutoSerial mSerial;
};

class MessageHandler : public PDClass_Base
{
    struct MessageCompare
    {
        bool operator()(const Message& a, const Message& b) const
        {
            // FIX - ordering comparison
            
            // Use time
            
            if (a.mInfo.mTime != b.mInfo.mTime)
                return a.mInfo.mTime > b.mInfo.mTime;
            
            // Use streams - N.B. Streams run in reverse order
            
            if (a.mInfo.mProxy == b.mInfo.mProxy)
                return a.mInfo.mStream < b.mInfo.mStream;
            
            return a.mInfo.mProxy < b.mInfo.mProxy;
        }
    };
    
    struct MessageBlock
    {
        class Queue : public std::priority_queue<Message, std::vector<Message>, MessageCompare>
        {
            friend MessageBlock;
            std::vector<Message>& container() { return c; }
        };
        
        Queue mQueue;
        unsigned long mMaxSize = 1;
        
        void flush(FrameLib_PDProxy *proxy)
        {
            for (auto it = mQueue.container().begin(); it != mQueue.container().end(); it++)
                it->mInfo.mProxy = it->mInfo.mProxy == proxy ? nullptr : it->mInfo.mProxy;
        }
    };
    
public:
    
    MessageHandler(t_object *x, t_symbol *sym, long ac, t_atom *av) {}
    
    void add(const MessageInfo& info, const double *values, unsigned long N)
    {
        // Lock, get vector size and copy
        
        FrameLib_LockHolder lock(&mLock);
        mData.mMaxSize = std::max(limit(N), mData.mMaxSize);
        mData.mQueue.emplace(info, values, N);
    }
    
    void add(const MessageInfo& info, const FrameLib_Parameters::Serial *serial)
    {
        // Lock, determine maximum vector size and copy
        
        FrameLib_LockHolder lock(&mLock);
        
        for (auto it = serial->begin(); it != serial->end(); it++)
        {
            if (it.getType() == DataType::Vector)
            {
                unsigned long size = it.getVectorSize();
                mData.mMaxSize = std::max(limit(size), mData.mMaxSize);
            }
        }
        
        mData.mQueue.emplace(info, serial);
    }
    
    void ready()
    {
        // Lock and copy onto the output queue
        
        FrameLib_LockHolder lock(&mLock);
        
        if (!mData.mQueue.size())
            return;
        
        mOutput.emplace_back();
        std::swap(mData, mOutput.back());
        lock.destroy();
    }
    
    void schedule()
    {
        ready();
        schedule_delay(*this, (t_method) &service, 0, nullptr, 0, nullptr);
    }
    
    void flush(FrameLib_PDProxy *proxy)
    {
        FrameLib_LockHolder flushLock(&mFlushLock);
        FrameLib_LockHolder lock(&mLock);
        
        mData.flush(proxy);
        
        for (auto it = mOutput.begin(); it != mOutput.end(); it++)
            it->flush(proxy);
    }
    
    static void service(MessageHandler* handler, t_symbol *s, short ac, t_atom *av)
    {
        MessageBlock messages;
        
        // Swap data
        
        FrameLib_LockHolder flushLock(&handler->mFlushLock);
        FrameLib_LockHolder lock(&handler->mLock);
        
        if (!handler->mOutput.empty())
        {
            std::swap(handler->mOutput.front(), messages);
            handler->mOutput.pop_front();
        }
        
        lock.destroy();
        flushLock.destroy();
        
        // Allocate temporary t_atoms and output
        
        std::vector<t_atom> out(messages.mMaxSize);
        
        while (!messages.mQueue.empty())
        {
            output(messages.mQueue.top(), out.data());
            messages.mQueue.pop();
        }
    }
    
private:
    
    static void output(const Message& message, t_atom *data)
    {
        FrameLib_PDMessageProxy *proxy = message.mInfo.mProxy;
        
        if (!proxy)
            return;
        
        if (message.mType == FrameType::Vector)
        {
            unsigned long N = limit(static_cast<unsigned long>(message.mVector.size()));
            
            for (unsigned long i = 0; i < N; i++)
                atom_setfloat(data + i, message.mVector[i]);
            
            proxy->sendMessage(message.mInfo.mStream, nullptr, static_cast<short>(N), data);
        }
        else
        {
            // Iterate over tags
            
            for (auto it = message.mSerial.begin(); it != message.mSerial.end(); it++)
            {
                t_symbol *tag = gensym(it.getTag());
                unsigned long size = 0;
                
                if (it.getType() == DataType::Vector)
                {
                    const double *vector = it.getVector(&size);
                    size = limit(size);
                    
                    for (unsigned long i = 0; i < size; i++)
                        atom_setfloat(data + i, vector[i]);
                }
                else
                {
                    size = 1;
                    atom_setsymbol(data, gensym(it.getString()));
                }
                
                proxy->sendMessage(message.mInfo.mStream, tag, static_cast<short>(size), data);
            }
        }
    }
    
    static unsigned long limit(unsigned long N) { return std::min(N, 32767UL); }
    
    mutable FrameLib_Lock mLock;
    mutable FrameLib_Lock mFlushLock;
    
    MessageBlock mData;
    std::deque<MessageBlock> mOutput;
};
*/
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
        enum class Mode : intptr_t { kConnect, kConfirm, kDoubleCheck };

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
    
    FrameLib_PDGlobals(t_object *x, t_symbol *sym, long ac, t_atom *av)
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
/////////////////////// FrameLib PD Object Class /////////////////////////
//////////////////////////////////////////////////////////////////////////

enum PDObjectArgsMode { kAsParams, kAllInputs, kDistribute };

template <class T, PDObjectArgsMode argsMode = kAsParams>
class FrameLib_PDClass : public PDClass_Base
{
    //using ConnectionMode = FrameLib_PDGlobals::ConnectionMode;
    using FLObject = FrameLib_Multistream;
    using FLConnection = FrameLib_Object<FLObject>::Connection;
    using PDConnection = FrameLib_Object<t_object>::Connection;
    using LockHold = FrameLib_LockHolder;
    using NumericType = FrameLib_Parameters::NumericType;
    using ClipMode = FrameLib_Parameters::ClipMode;
    
    using ConnectionInfo = FrameLib_PDGlobals::ConnectionInfo;
    
    struct ConnectionConfirmation
    {
        ConnectionConfirmation(PDConnection connection, long inIdx)
        : mConfirm(false), mConnection(connection), mInIndex(inIdx) {}
        
        bool confirm(PDConnection connection, long inIdx)
        {
            bool result = inIdx == mInIndex && connection == mConnection;
            
            if (result)
                mConfirm = true;
            
            return result;
        }
        
        bool mConfirm;
        PDConnection mConnection;
        long mInIndex;
    };
    
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
        
        PDProxy(t_object *x, t_symbol *sym, long ac, t_atom *av) : mOwner(nullptr), mIndex(-1) {}
        
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
        
        addMethod(c, (t_method) &extResolveConnections, FrameLib_PDPrivate::messageResolveConnections());
        addMethod(c, (t_method) &extMakeAutoOrdering, FrameLib_PDPrivate::messageMakeAutoOrdering());
        addMethod(c, (t_method) &extClearAutoOrdering, FrameLib_PDPrivate::messageClearAutoOrdering());
        addMethod(c, (t_method) &extConnectionConfirm, FrameLib_PDPrivate::messageConnectionConfirm());
        addMethod(c, (t_method) &extGetNumAudioIns, FrameLib_PDPrivate::messageGetNumAudioIns());
        addMethod(c, (t_method) &extGetNumAudioOuts, FrameLib_PDPrivate::messageGetNumAudioOuts());
        addMethod(c, (t_method) &extIsConnected, "__fl.is_connected");
        addMethod(c, (t_method) &extGetInternalObject, "__fl.get_internal_object");
        addMethod(c, (t_method) &extIsOutput, "__fl.is_output");
        
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

    FrameLib_PDClass(t_object *x, t_symbol *s, long argc, t_atom *argv, FrameLib_PDProxy *proxy = nullptr)
    : mProxy(proxy ? proxy : new FrameLib_PDProxy(x))
    , mConfirmObject(nullptr)
    , mConfirmInIndex(-1)
    , mConfirmOutIndex(-1)
    , mConfirm(false)
    , mCanvas(canvas_getcurrent())
    , mSyncIn(nullptr)
    , mNeedsResolve(true)
    , mUserObject(*this)
    {
        // Stream count
        
        if (argc && getStreamCount(argv))
        {
            mSpecifiedStreams = getStreamCount(argv);
            argv++;
            argc--;
        }
        
        // Object creation with parameters and arguments (N.B. the object is not a member due to size restrictions)
        
        FrameLib_Parameters::AutoSerial serialisedParameters;
        parseParameters(serialisedParameters, argc, argv);
        mObject.reset(new T(FrameLib_Context(mGlobal->getGlobal(), mCanvas), &serialisedParameters, mProxy.get(), mSpecifiedStreams));
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
        
        post("------------------ %s ------------------", class_getname(*getClassPointer<FrameLib_PDClass>()));

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
                post("N.B - arguments are distributed one per input from the second input.");
            for (long i = 0; i < static_cast<long>(mObject->getNumAudioIns()); i++)
                post("Audio Input %ld: %s", i + 1, mObject->audioInfo(i, verbose).c_str());
            for (long i = 0; i < getNumIns(); i++)
                post("Frame Input %ld [%s]: %s", i + 1, frameTypeString(mObject->inputType(i)), mObject->inputInfo(i, verbose).c_str());
            if (supportsOrderingConnections())
                post("Ordering Input [%s]: Connect to ensure ordering", frameTypeString(FrameType::Any));
        }
        
        if (flags & kInfoOutputs)
        {
            post("--- Output List ---");
            for (long i = 0; i < static_cast<long>(mObject->getNumAudioOuts()); i++)
                post("Audio Output %ld: %s", i + 1, mObject->audioInfo(i, verbose).c_str());
            for (long i = 0; i < getNumOuts(); i++)
                post("Frame Output %ld [%s]: %s", i + 1, frameTypeString(mObject->outputType(i)), mObject->outputInfo(i, verbose).c_str());
        }
        
        // Parameters
        
        if (flags & kInfoParameters)
        {
            post("--- Parameter List ---");
            
            const FrameLib_Parameters *params = mObject->getParameters();
            if (!params || !params->size()) post("< No Parameters >");
            
            // Loop over parameters
            
            for (unsigned long i = 0; params && i < params->size(); i++)
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
                        for (unsigned long j = 0; j <= static_cast<unsigned long>(params->getMax(i)); j++)
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
    
    unsigned long getSpecifiedStreams() const   { return mSpecifiedStreams; }

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
        
        iterateCanvas(mCanvas, gensym(FrameLib_PDPrivate::messageResolveConnections()));
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
            iterateCanvas(mCanvas, gensym(FrameLib_PDPrivate::messageResolveConnections()));
            iterateCanvas(mCanvas, gensym(FrameLib_PDPrivate::messageClearAutoOrdering()));
            iterateCanvas(mCanvas, gensym(FrameLib_PDPrivate::messageMakeAutoOrdering()));
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
                        objectMethod<void>(getConnection(i).mObject, "sync");
                
                if (supportsOrderingConnections())
                    for (unsigned long i = 0; i < getNumOrderingConnections(); i++)
                        objectMethod<void>(getOrderingConnection(i).mObject, "sync");
                
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
            case ConnectionInfo::Mode::kConnect:
                connect(info->mObject, info->mIndex, index);
                break;
                
            case ConnectionInfo::Mode::kConfirm:
            case ConnectionInfo::Mode::kDoubleCheck:

                if (index == mConfirmInIndex && mConfirmObject == info->mObject && mConfirmOutIndex == info->mIndex)
                {
                    mConfirm = true;
                    if (info->mMode == ConnectionInfo::Mode::kDoubleCheck)
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
                               
    static void extMakeAutoOrdering(FrameLib_PDClass *x)
    {
        x->mObject->makeAutoOrderingConnections();
    }
    
    static void extClearAutoOrdering(FrameLib_PDClass *x)
    {
        x->mObject->clearAutoOrderingConnections();
    }

    static uintptr_t extIsConnected(FrameLib_PDClass *x, unsigned long index)
    {
        return x->confirmConnection(index, ConnectionInfo::Mode::kConfirm);
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
        return objectMethod<FLObject *>(x, "__fl.get_internal_object");
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
                confirmConnection(i, ConnectionInfo::Mode::kConfirm);
            
            // Confirm ordering connections
            
            for (unsigned long i = 0; i < getNumOrderingConnections(); i++)
                confirmConnection(getOrderingConnection(i), getNumIns(), ConnectionInfo::Mode::kConfirm);
            
            // Make output connections
            
            for (unsigned long i = getNumOuts(); i > 0; i--)
                makeConnection(i - 1, ConnectionInfo::Mode::kConnect);
            
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
    
    // Convert from framelib object to pd object and vice versa
    
    static FLObject *toFLObject(t_object *x)
    {
        return FrameLib_PDPrivate::toFrameLibObject(x);
    }
    
    static t_object *toPDObject(FLObject *object)
    {
        return object ? object->getProxy()->getOwner<t_object>() : nullptr;
    }
    
//    bool versionMismatch(t_object *object, long inIdx, bool report) const
//    {
//        bool mismatch = FrameLib_PDPrivate::versionMismatch(object);
//
//        if (mismatch && report)
//            mInputs[inIdx].reportError(mUserObject, Input::kVersion);
//            
//        return mismatch;
//    }
    
    // Get the number of audio ins/outs safely from a generic pointer
    
    static long getNumAudioIns(t_object *x)
    {
        intptr_t numAudioIns = objectMethod<intptr_t>(x, FrameLib_PDPrivate::messageGetNumAudioIns());
        return static_cast<long>(numAudioIns);
    }

    static long getNumAudioOuts(t_object *x)
    {
        intptr_t numAudioOuts = objectMethod<intptr_t>(x, FrameLib_PDPrivate::messageGetNumAudioOuts());
        return static_cast<long>(numAudioOuts);
    }
    
    // Helpers for connection methods
    
    static bool isOrderingInput(long index, FLObject *object)
    {
        return object && object->supportsOrderingConnections() && index == static_cast<long>(object->getNumIns());
    }
    
    bool isOrderingInput(long index) const                  { return isOrderingInput(index, mObject.get()); }
    bool isConnected(long index) const                      { return mObject->isConnected(index); }
    
    static bool validIO(long index, unsigned long count)    { return index >= 0 && index < static_cast<long>(count); }
    static bool validInput(long index, FLObject *object)    { return object && validIO(index, object->getNumIns()); }
    static bool validOutput(long index, FLObject *object)   { return object && validIO(index, object->getNumOuts()); }
    
    bool validInput(long index) const                       { return validInput(index, mObject.get()); }
    bool validOutput(long index) const                      { return validOutput(index, mObject.get()); }
    
    PDConnection getConnection(long index)                  { return toPDConnection(mObject->getConnection(index)); }
    PDConnection getOrderingConnection(long index)          { return toPDConnection(mObject->getOrderingConnection(index)); }
    
    long getNumOrderingConnections() const                  { return static_cast<long>(mObject->getNumOrderingConnections()); }
    
    static PDConnection toPDConnection(FLConnection c)      { return PDConnection(toPDObject(c.mObject), c.mIndex); }
    static FLConnection toFLConnection(PDConnection c)      { return FLConnection(toFLObject(c.mObject), c.mIndex); }
    
    // Private connection methods

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
            objectMethod<void>(mConfirmObject, FrameLib_PDPrivate::messageConnectionConfirm(), mConfirmOutIndex, mode);
        
        if (mConfirmObject && !mConfirm)
            disconnect(mConfirmObject, mConfirmOutIndex, mConfirmInIndex);
        
        bool result = mConfirm;
        mConfirm = false;
        mConfirmObject = nullptr;
        mConfirmInIndex = mConfirmOutIndex = -1;
        
        return result;
    }
    
    bool matchConnection(t_object *src, long outIdx, long inIdx)
    {
        PDConnection connection = getConnection(inIdx);
        return connection.mObject == src && connection.mIndex == outIdx;
    }
    
    void connect(t_object *src, long outIdx, long inIdx)
    {
        FrameLib_Multistream *object = getInternalObject(src);
        
        if (!isOrderingInput(inIdx) && (!validInput(inIdx) || !validOutput(outIdx, object) || matchConnection(src, outIdx, inIdx) || confirmConnection(inIdx, ConnectionInfo::Mode::kDoubleCheck)))
            return;

        ConnectionResult result;
        
        if (isOrderingInput(inIdx))
            result = mObject->addOrderingConnection(FLConnection(object, outIdx));
        else
            result = mObject->addConnection(FLConnection(object, outIdx), inIdx);

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
            mObject->deleteOrderingConnection(FLConnection(object, outIdx));
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
    
    static unsigned long safeCount(char *str, unsigned long minCount, unsigned long maxCount)
    {
        unsigned long number = std::max(minCount, static_cast<unsigned long>(atoi(str)));
        return std::min(maxCount, number);
    }
    
    static long getStreamCount(t_atom *a)
    {
        if (atom_gettype(a) == A_SYMBOL)
        {
            t_symbol *sym = atom_getsymbol(a);
            
            if (strlen(sym->s_name) > 1 && sym->s_name[0] == '=')
                return safeCount(sym->s_name + 1, 1, 1024);
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
            t_symbol *sym = atom_getsymbol(argv + i++);
            
            if (isParameterTag(sym))
            {
                // Check for missing values
                
                if ((i >= argc) || isTag(argv + i))
                {
                    pd_error(mUserObject, "no values given for parameter %s", sym->s_name);
                    continue;
                }
                
                // Add to parameters with stray item detection
                
                if (atom_getsymbol(argv + i) != gensym(""))
                {
                    serialisedParameters.write(sym->s_name + 1, atom_getsymbol(argv + i++)->s_name);
                    
                    if (i < argc && !isTag(argv + i))
                        pd_error(mUserObject, "stray items after parameter %s", sym->s_name);
                }
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
        return safeCount(sym->s_name + 1, 0, 16384) - 1;
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
            t_symbol *sym = atom_getsymbol(argv + i++);
            
            if (isInputTag(sym))
            {
                i = parseNumericalList(values, argv, argc, i);
                mObject->setFixedInput(inputNumber(sym), values.data(), static_cast<unsigned long>(values.size()));
                
                if (inputNumber(sym) >= static_cast<unsigned long>(getNumIns()))
                    pd_error(mUserObject, "input %s out of bounds", sym->s_name);
            }
        }
    }


protected:
    
    // Proxy
    
    std::unique_ptr<FrameLib_PDProxy> mProxy;
    
private:
    
    // Data - N.B. - the order is crucial for safe deconstruction
    
    FrameLib_PDGlobals::ManagedPointer mGlobal;
    FrameLib_PDGlobals::SyncCheck mSyncChecker;
    
    std::unique_ptr<FLObject> mObject;
    
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
    
    unsigned long mSpecifiedStreams;
    
    bool mNeedsResolve;
    
public:

    t_object *mUserObject;
};

// Convenience for Objects Using FrameLib_Expand (use FrameLib_PDClass_Expand<T>::makeClass() to create)

template <class T, PDObjectArgsMode argsMode = kAsParams>
using FrameLib_PDClass_Expand = FrameLib_PDClass<FrameLib_Expand<T>, argsMode>;

#endif
