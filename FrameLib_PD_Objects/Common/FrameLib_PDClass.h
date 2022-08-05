
#ifndef FRAMELIB_PD_CLASS_H
#define FRAMELIB_PD_CLASS_H

#include "PDClass_Base.h"

#include "g_canvas.h"

#include "pd_buffer.h"

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
    
    static inline t_symbol *globalTag()                     { return gensym(VersionString("__fl.pd_global_tag")); }
    
    static inline VersionString objectGlobal()              { return "__fl.pd_global_items"; }
    static inline VersionString objectMessageHandler()      { return "__fl.message.handler"; }
    
    static inline const char *messageUnwrap()               { return "__fl.unwrap"; }
    static inline const char *messageIsWrapper()            { return "__fl.is_wrapper"; }
    static inline const char *messageFindAudioObjects()     { return "__fl.find_audio_objects"; }
    static inline const char *messageResolveContext()       { return "__fl.resolve_context"; }
    static inline const char *messageResolveConnections()   { return "__fl.resolve_connections"; }
    static inline const char *messageMarkUnresolved()       { return "__fl.mark_unresolved"; }
    static inline const char *messageMakeAutoOrdering()     { return "__fl.make_auto_ordering"; }
    static inline const char *messageClearAutoOrdering()    { return "__fl.clear_auto_ordering"; }
    static inline const char *messageReset()                { return "__fl.reset"; }
    static inline const char *messageConnectionConfirm()    { return "__fl.connection_confirm"; }
    static inline const char *messageConnectionUpdate()     { return "__fl.connection_update"; }
    static inline const char *messageGetFrameLibObject()    { return "__fl.get_framelib_object"; }
    
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
};

struct FrameLib_PDMessageProxy : FrameLib_PDProxy
{
    FrameLib_PDMessageProxy(t_object *x) : FrameLib_PDProxy(x) {}
    
    // Override for objects that require max messages to be sent from framelib
    
    virtual void sendMessage(unsigned long stream, t_symbol *s, short ac, t_atom *av) {}
};

struct FrameLib_PDNRTAudio
{
    FrameLib_Multistream *mObject;
    t_symbol *mBuffer;
};

struct FrameLib_PDContext
{
    bool mRealtime;
    t_glist *mCanvas;
    t_symbol *mName;
    
    bool operator == (const FrameLib_PDContext& b) const
    {
        return mRealtime == b.mRealtime && mCanvas == b.mCanvas && mName == b.mName;
    }
};

//////////////////////////////////////////////////////////////////////////
/////////////////////////// Messaging Classes ////////////////////////////
//////////////////////////////////////////////////////////////////////////

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
    
    MessageHandler(t_object *x, t_symbol *sym, long ac, t_atom *av)
    : mClock(x, (t_method) &service) {}
    
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
        mClock.delay();
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
    
    Clock mClock;
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////// PD Globals Class ////////////////////////////
//////////////////////////////////////////////////////////////////////////

class FrameLib_PDGlobals : public PDClass_Base
{
public:
    
    enum ConnectionMode : intptr_t { kConnect, kConfirm, kDoubleCheck };

    using PDConnection = FrameLib_Connection<t_object, long>;

private:
    
    struct Hash
    {
        size_t operator()(void *a, void *b) const
        {
            size_t hash = 0;
            hash ^= std::hash<void *>()(a) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
            hash ^= std::hash<void *>()(b) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
            
            return hash;
        }
        
        size_t operator()(const FrameLib_Context& context) const
        {
            return operator()(context.getGlobal(), context.getReference());
        }
        
        size_t operator()(const FrameLib_PDContext& key) const
        {
            // N.B. Realtime state isn't used in the hash, but used during full lookup
            
            return operator()(key.mCanvas, key.mName);
        }
    };
    
    enum DSPDeferMode { kOff, kDefer, kNeedResume };
    enum RefDataItem { kKey, kCount, kLastResolved, kFinal, kHandler, kQueuePtr };

    using QueuePtr = std::unique_ptr<FrameLib_Context::ProcessingQueue>;
    using RefData = std::tuple<FrameLib_PDContext, int, double, t_object *, unique_pd_ptr, QueuePtr>;
    using RefMap = std::unordered_map<FrameLib_PDContext, std::unique_ptr<RefData>, Hash>;
    
public:
    
    // Error Notification Class
    
    struct ErrorNotifier : public FrameLib_ErrorReporter::HostNotifier
    {
        ErrorNotifier(FrameLib_Global **globalHandle)
        : mQelem(globalHandle, (t_method) &flush) {}
        
        bool notify(const FrameLib_ErrorReporter::ErrorReport& report) override
        {
            mQelem.set();
            return false;
        }
        
        static void flushIgnore(FrameLib_Global **globalHandle, t_object *ignore = nullptr)
        {
            auto reports = (*globalHandle)->getErrors();

            for (auto it = reports->begin(); it != reports->end(); it++)
            {
                std::string errorText;
                t_object *object = it->getReporter() ? it->getReporter()->getOwner<t_object>() : nullptr;
                
                it->getErrorText(errorText);
                
                if (object)
                {
                    if (object == ignore)
                        continue;
                    pd_error(object, "%s", errorText.c_str());
                }
                else
                    error("%s", errorText.c_str());
            }

            if (reports->isFull())
                error("*** FrameLib - too many errors - reporting only some ***");
        }
        
        static void flush(FrameLib_Global **globalHandle)
        {
            flushIgnore(globalHandle);
        }

        Qelem mQelem;
    };
    
    // Convenience Pointer for automatic deletion and RAII
    
    struct ManagedPointer
    {
        ManagedPointer() : mPointer(get()) {}
        ~ManagedPointer() { mPointer->release(); }
        
        ManagedPointer(const ManagedPointer&) = delete;
        ManagedPointer& operator=(const ManagedPointer&) = delete;
        
        FrameLib_PDGlobals *operator->() { return mPointer; }
        const FrameLib_PDGlobals *operator->() const { return mPointer; }
        
    private:
        
        FrameLib_PDGlobals *mPointer;
    };
    
    // Constructor and Destructor (public for pd API, but use ManagedPointer from outside this class)
    
    FrameLib_PDGlobals(t_object *x, t_symbol *sym, long ac, t_atom *av)
    : mReportContextErrors(false)
    , mRTNotifier(&mRTGlobal)
    , mNRTNotifier(&mNRTGlobal)
    , mDSPDefer(kOff)
    , mInDSP(false)
    , mRequireDSPRestart(false)
    , mRTGlobal(nullptr)
    , mNRTGlobal(nullptr)
    {
        pd_bind(asPD(), gensym("pd-dsp-started"));
    }
    
    ~FrameLib_PDGlobals()
    {
        pd_unbind(asPD(), gensym("pd-dsp-started"));
    }
    
    //
    
    static void classInit(t_class *c, const char *classname)
    {
        addMethod<FrameLib_PDGlobals, &FrameLib_PDGlobals::dspStartNotification>(c, "bang");
    }
    
    // Pd global item methods
    
    void clearQueue()                           { mQueue.clear(); }
    void pushToQueue(t_object * object)         { return mQueue.push_back(object); }
    
    t_object *popFromQueue()
    {
        t_object *object = mQueue.empty() ? nullptr : mQueue.front();
        
        if (object)
            mQueue.pop_front();
        
        return object;
    }
    
    bool setLastResolved(FrameLib_Context c, double time)
    {
        bool changed = time != data<kLastResolved>(c);
        
        if (changed)
            data<kLastResolved>(c) = time;
        
        return changed;
    }
    
    void contextMessages(FrameLib_Context c)
    {
        MessageHandler *handler = getHandler(c);
        
        handler->ready();
        MessageHandler::service(handler, nullptr, 0, nullptr);
    }
    
    void contextMessages(FrameLib_Context c, t_object *object)
    {
        if (data<kFinal>(c) == object)
            getHandler(c)->schedule();
    }
    
    MessageHandler *getHandler(FrameLib_Context c)
    {
        return reinterpret_cast<MessageHandler *>(data<kHandler>(c).get());
    }
    
    FrameLib_Context makeContext(const FrameLib_PDContext& key)
    {
        std::unique_ptr<RefData>& item = mContextRefs[key];
        
        if (!item)
        {
            item.reset(new RefData());
            FrameLib_Context context(key.mRealtime ? mRTGlobal : mNRTGlobal, item.get());
            
            std::get<kKey>(*item) = key;
            std::get<kCount>(*item) = 1;
            std::get<kLastResolved>(*item) = 0;
            std::get<kFinal>(*item) = nullptr;
            std::get<kHandler>(*item) = unique_pd_ptr(createNamed<t_pd>(FrameLib_PDPrivate::objectMessageHandler()));
            std::get<kQueuePtr>(*item) = QueuePtr(new FrameLib_Context::ProcessingQueue(context));
            
            // Set timeouts
            
            if (key.mRealtime)
                (*(std::get<kQueuePtr>(*item).get()))->setTimeOuts(16.0, 1.0);
            else
                (*(std::get<kQueuePtr>(*item).get()))->setTimeOuts(100.0, 30.0);
        }
        else
            std::get<kCount>(*item)++;
        
        return FrameLib_Context(key.mRealtime ? mRTGlobal : mNRTGlobal, item.get());
    }
    
    void retainContext(FrameLib_Context c)      { data<kCount>(c)++; }
    void releaseContext(FrameLib_Context c)     { if (--data<kCount>(c) == 0) mContextRefs.erase(data<kKey>(c)); }
    
    void flushContext(FrameLib_Context c, FrameLib_PDProxy *proxy)
    {
        ErrorNotifier::flush(data<kKey>(c).mRealtime ? &mRTGlobal : &mNRTGlobal);
        
        getHandler(c)->flush(proxy);
    }
    
    void flushErrors(FrameLib_Context c, t_object *object)
    {
        ErrorNotifier::flushIgnore(data<kKey>(c).mRealtime ? &mRTGlobal : &mNRTGlobal, object);
    }
    
    FrameLib_PDContext getPDContext(FrameLib_Context c)
    {
        return data<kKey>(c);
    }
    
    bool isRealtime(FrameLib_Context c) const   { return c.getGlobal() == mRTGlobal; }
    t_object *&finalObject(FrameLib_Context c)  { return data<kFinal>(c); }
    
    void setReportContextErrors(bool report)    { mReportContextErrors = report; }
    bool getReportContextErrors() const         { return mReportContextErrors; }
    
    void setConnection(PDConnection c)          { mConnection = c; }
    void setConnectionMode(ConnectionMode m)    { mConnectionMode = m; }
    PDConnection getConnection() const          { return mConnection; }
    ConnectionMode getConnectionMode() const    { return mConnectionMode; }
        
    // DSP Handling / Suspending + Resume (allows deferal when resolving contexts)
    
    void dspStartNotification()
    {
        if (mRequireDSPRestart)
        {
            mRequireDSPRestart = false;
            canvas_resume_dsp(canvas_suspend_dsp());
        }
    }
    
    void dspBuilding(bool state)
    {
        mInDSP = state;
    }
    
    void dspDeferResume(bool state)
    {
        if (state)
        {
            if (mDSPDefer == kOff)
                mDSPDefer = kDefer;
        }
        else
        {
            bool willResume = mDSPDefer == kNeedResume;
        
            mDSPDefer = kOff;
            
            if (willResume)
                dspResume(true);
        }
    }
    
    bool dspSuspendRequest()
    {
        if (mInDSP)
        {
            mRequireDSPRestart = true;
            return false;
        }
        
        return dspSuspend();
    }
    
    void dspResumeRequest(bool oldState)
    {
        if (oldState && mDSPDefer != kOff)
            mDSPDefer = kNeedResume;
        else
            dspResume(oldState);
    }
                                                       
private:
    
    // Context methods
    
    template <size_t N>
    typename std::tuple_element<N, RefData>::type& data(FrameLib_Context context)
    {
        return std::get<N>(*static_cast<RefData *>(context.getReference()));
    }
    
    // Generate some relevant thread priorities
    
    static FrameLib_Thread::Priorities priorities(bool realtime)
    {
        if (!realtime)
#if defined __linux__ || defined __APPLE__
            return { 31, 31, 31, SCHED_OTHER, true };
#else
            return { THREAD_PRIORITY_NORMAL, THREAD_PRIORITY_HIGHEST, THREAD_PRIORITY_TIME_CRITICAL, 0, true };
#endif
        
        return FrameLib_Thread::defaultPriorities();
    }
    
    // Get and release the max global items (singleton)
    
    static FrameLib_PDGlobals *get()
    {
        auto pdGlobalTag = FrameLib_PDPrivate::globalTag();
        auto pdGlobalClass = FrameLib_PDPrivate::objectGlobal();
        auto messageClassName = FrameLib_PDPrivate::objectMessageHandler();
        
        // Make sure the pd globals and message handler classes exist
         
        if (!classExists(pdGlobalClass))
            makeClass<FrameLib_PDGlobals>(pdGlobalClass, CLASS_PD);
        
        if (!classExists(messageClassName))
            MessageHandler::makeClass<MessageHandler>(messageClassName, CLASS_PD);
        
        // Create a global to get the class name
        
        FrameLib_PDGlobals *y = createNamed<FrameLib_PDGlobals>(pdGlobalClass);
        
        // See if an object is already registered (and either free the new object or bind it...)
        
        FrameLib_PDGlobals *x = (FrameLib_PDGlobals *) pd_findbyclass(pdGlobalTag, objectClass(y->asPD()));
        
        if (!x)
        {
            x = y;
            pd_bind(x->asPD(), pdGlobalTag);
        }
        else
            pd_free(y->asPD());
        
        FrameLib_Global::get(&x->mRTGlobal, priorities(true), &x->mRTNotifier);
        FrameLib_Global::get(&x->mNRTGlobal, priorities(false), &x->mNRTNotifier);
        
        return x;
    }
    
    void release()
    {
        FrameLib_Global::release(&mRTGlobal);
        FrameLib_Global::release(&mNRTGlobal);
        
        if (!mRTGlobal)
        {
            assert(!mNRTGlobal && "Reference counting error");
            
            pd_unbind(asPD(), FrameLib_PDPrivate::globalTag());
            pd_free(asPD());
        }
    }
    
    // Connection Info
    
    PDConnection mConnection;
    ConnectionMode mConnectionMode;
    bool mReportContextErrors;
    
    // Member Objects / Pointers
    
    ErrorNotifier mRTNotifier;
    ErrorNotifier mNRTNotifier;
    
    std::deque<t_object *> mQueue;
    RefMap mContextRefs;
    
    DSPDeferMode mDSPDefer;
    bool mInDSP;
    bool mRequireDSPRestart;
    
    FrameLib_Global *mRTGlobal;
    FrameLib_Global *mNRTGlobal;
};

//////////////////////////////////////////////////////////////////////////
/////////////////////// FrameLib PD Object Class /////////////////////////
//////////////////////////////////////////////////////////////////////////

enum PDObjectArgsMode { kAsParams, kAllInputs, kDistribute };

template <class T, PDObjectArgsMode argsMode = kAsParams>
class FrameLib_PDClass : public PDClass_Base
{
    using ConnectionMode = FrameLib_PDGlobals::ConnectionMode;
    using FLObject = FrameLib_Multistream;
    using FLConnection = FrameLib_Object<FLObject>::Connection;
    using PDConnection = FrameLib_PDGlobals::PDConnection;
    using NumericType = FrameLib_Parameters::NumericType;
    using ClipMode = FrameLib_Parameters::ClipMode;
        
    struct PDInputProxy : public PDClass_Base
    {
        static t_pd *create(FrameLib_PDClass *owner, long index)
        {
            t_pd *proxy = pd_new(*PDInputProxy::getClassPointer<PDInputProxy>());
            
            ((PDInputProxy *)proxy)->mOwner = owner;
            ((PDInputProxy *)proxy)->mIndex = index;
            
            return proxy;
        }
        
        static void classInit(t_class *c, const char *classname)
        {
            addMethod<PDInputProxy, &PDInputProxy::anything>(c, "anything");
        }
        
        PDInputProxy(t_object *x, t_symbol *sym, long ac, t_atom *av) : mOwner(nullptr), mIndex(-1) {}
        
        void anything(t_symbol *s, long ac, t_atom *av)
        {
            if (!s || s == gensym("signal"))
                mOwner->frameInlet(mIndex);
            else
                pd_error(mOwner->asPD(), "%s: no method for '%s'", class_getname(*mOwner->asPD()), s->s_name);
        }
        
        FrameLib_PDClass *mOwner;
        long mIndex;
    };
    
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
    
    class Input
    {
    public:
        
        enum Error { kNone = 0x00, kVersion = 0x01, kContext = 0x02, kExtra= 0x04, kFeedback = 0x08, kDirect = 0x10 };
        
        Input(t_pd *proxy, t_object *owner, long index)
        : mProxy(toUnique(proxy))
        , mOwner(owner)
        , mIndex(index)
        , mErrorTime(-1)
        , mErrorFlags(0)
        {}
        
        Input() : Input(nullptr, nullptr, 0) {}
        
        void reportError(Error error) const
        {
            auto postError = [&](const char *str) { pd_error(mOwner, "%s input %ld", str, mIndex + 1); };
            
            double time = clock_getlogicaltime();
            bool validTime = mErrorTime + 500 >= time;
            
            if (validTime && mErrorFlags & error)
                return;
            
            switch (error)
            {
                case kVersion:      postError("can't connect objects from different versions of framelib -");   break;
                case kContext:      postError("can't connect objects in different patching contexts -");        break;
                case kExtra:        postError("extra connection to");                                           break;
                case kFeedback:     postError("feedback loop detected at");                                     break;
                case kDirect:       postError("direct feedback loop detected at");                              break;
                
                default:
                    return;
            }
            
            if (validTime)
                mErrorFlags |= error;
            else
            {
                mErrorTime = time;
                mErrorFlags = error;
            }
        }
        
    private:
        
        unique_pd_ptr mProxy;
        t_object *mOwner;
        long mIndex;
        mutable long mErrorTime;
        mutable long mErrorFlags;
    };
    
public:
    
    // Class Initialisation (must explicitly give U for classes that inherit from FrameLib_PDClass<>)
    
    template <class U = FrameLib_PDClass<T, argsMode>>
    static void makeClass(const char *className)
    {
        // If handles audio/scheduler then make wrapper class and name the inner object differently..
        
        std::string internalClassName = className;
        std::string proxyClassName = className;

        proxyClassName.append(".proxy");
        PDClass_Base::makeClass<U>(internalClassName.c_str());
        PDClass_Base::makeClass<PDInputProxy>(proxyClassName.c_str(), CLASS_PD);
    }
    
    static void classInit(t_class *c, const char *classname)
    {
        addMethod<FrameLib_PDClass<T>, &FrameLib_PDClass<T>::info>(c, "info");
        addMethod<FrameLib_PDClass<T>, &FrameLib_PDClass<T>::frame>(c, "signal");
        addMethod<FrameLib_PDClass, &FrameLib_PDClass::anything>(c, "anything");
        addMethod<FrameLib_PDClass<T>, &FrameLib_PDClass<T>::dsp>(c);
        
        // Attributes
        
        addMethod<FrameLib_PDClass, &FrameLib_PDClass::idSet>(c, "id");
        addMethod<FrameLib_PDClass, &FrameLib_PDClass::rtSet>(c, "rt");
        
        // Audio only
        
        if (T::sHandlesAudio)
        {
            addMethod<FrameLib_PDClass<T>, &FrameLib_PDClass<T>::reset>(c, "reset");
            addMethod<FrameLib_PDClass<T>, &FrameLib_PDClass<T>::process>(c, "process");
            
            addMethod(c, (t_method) &extFindAudio, FrameLib_PDPrivate::messageFindAudioObjects());
        }
        
        // External Methods
        
        addMethod(c, (t_method) &extResolveContext, FrameLib_PDPrivate::messageResolveContext());
        addMethod(c, (t_method) &extResolveConnections, FrameLib_PDPrivate::messageResolveConnections());
        addMethod(c, (t_method) &extMarkUnresolved, FrameLib_PDPrivate::messageMarkUnresolved());
        addMethod(c, (t_method) &extMakeAutoOrdering, FrameLib_PDPrivate::messageMakeAutoOrdering());
        addMethod(c, (t_method) &extClearAutoOrdering, FrameLib_PDPrivate::messageClearAutoOrdering());
        addMethod(c, (t_method) &extReset, FrameLib_PDPrivate::messageReset());
        addMethod(c, (t_method) &extConnectionConfirm, FrameLib_PDPrivate::messageConnectionConfirm());
        addMethod(c, (t_method) &extConnectionUpdate, FrameLib_PDPrivate::messageConnectionUpdate());
        addMethod(c, (t_method) &extGetFLObject, FrameLib_PDPrivate::messageGetFrameLibObject());
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
    
    static bool isAttributeTag(t_symbol *sym)
    {
        return strlen(sym->s_name) > 1 && sym->s_name[0] == '-';
    }
    
    static bool isNamedAttributeTag(t_symbol *sym, const char *tag)
    {
        return isAttributeTag(sym) && !strcmp(sym->s_name + 1, tag);
    }
    
    static bool isTag(t_atom *a)
    {
        t_symbol *sym = atom_getsymbol_default(a);
        return atom_gettype(a) == A_SYMBOL && (isParameterTag(sym) || isInputTag(sym) || isAttributeTag(sym));
    }
    
    // Constructor and Destructor

    FrameLib_PDClass(t_object *x, t_symbol *s, long argc, t_atom *argv, FrameLib_PDProxy *proxy = nullptr)
    : mProxy(proxy ? proxy : new FrameLib_PDProxy(x))
    , mCanvas(canvas_getcurrent())
    , mConfirmation(nullptr)
    , mSpecifiedStreams(1)
    , mConnectionsUpdated(false)
    , mContextPatchConfirmed(false)
    , mResolved(false)
    , mBuffer(gensym(""))
    , mPDContext{ T::sType == ObjectType::Scheduler, mCanvas, gensym("") }
    {
        // Attributes
        
        parseAttributes(argc, argv);
        
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
        FrameLib_Context context = mGlobal->makeContext(mPDContext);
        mObject.reset(new T(context, &serialisedParameters, mProxy.get(), mSpecifiedStreams));
        parseInputs(argc, argv);
        
        long numIns = getNumIns() + (supportsOrderingConnections() ? 1 : 0);

        mInputs.resize(numIns);
        mOutputs.resize(getNumOuts());
        
        // Create signal inlets + signal outlets
        
        dspSetup(getNumAudioIns(), getNumAudioOuts());
        
        // Create frame inlets
        // N.B. - we create a proxy if the inlet is not the first inlet (not the first frame input or the object has audio ins)

        for (long i = 0; i < numIns; i++)
        {
            if (i || getNumAudioIns())
            {
                t_pd *proxy = PDInputProxy::create(this, i);
                inlet_new(asObject(), proxy, gensym("signal"), gensym("signal"));
                mInputs[i] = Input(proxy, asObject(), i);
            }
            else
                mInputs[i] = Input(nullptr, asObject(), i);
        }
        
        // Create frame outlets (as signals)
        
        for (unsigned long i = 0; i < getNumOuts(); i++)
            mOutputs[i] = outlet_new(asObject(), gensym("signal"));
        
        // Resize the signal IO
        
        dspResize(getNumAudioIns() + numIns, getNumAudioOuts() + getNumOuts());
    }

    ~FrameLib_PDClass()
    {
        if (isRealtime())
            dspSetBroken();
        
        mGlobal->flushContext(getContext(), mProxy.get());
        mGlobal->releaseContext(getContext());
    }
    
    void info(t_symbol *sym, long ac, t_atom *av)
    {
        // Determine what to post
        
        enum InfoFlags { kInfoDesciption = 0x01, kInfoInputs = 0x02, kInfoOutputs = 0x04, kInfoParameters = 0x08 };
        bool verbose = true;
        long flags = 0;
        
        while (ac--)
        {
            t_symbol *type = atom_getsymbol_default(av++);
            
            if (type == gensym("description"))          flags |= kInfoDesciption;
            else if (type == gensym("inputs"))          flags |= kInfoInputs;
            else if (type == gensym("outputs"))         flags |= kInfoOutputs;
            else if (type == gensym("io"))              flags |= kInfoInputs | kInfoOutputs;
            else if (type == gensym("parameters"))      flags |= kInfoParameters;
            else if (type == gensym("quick"))           verbose = false;
        }
        
        flags = !flags ? (kInfoDesciption | kInfoInputs | kInfoOutputs | kInfoParameters) : flags;
        
        // Start Tag
                
        post("------------------ %s ------------------", class_getname(*asPD()));

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
                    post("Parameter %lu: %s [%s] (default: %s)", i + 1, params->getName(i), params->getTypeString(i).c_str(), defaultStr.c_str());
                else
                    post("Parameter %lu: %s [%s]", i + 1, params->getName(i), params->getTypeString(i).c_str());

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
                            post("  [%ld] - %s", j, params->getItemString(i, j));
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

    bool isRealtime() const                     { return mGlobal->isRealtime(getContext()); }
    bool handlesAudio() const                   { return T::sHandlesAudio; }
    bool supportsOrderingConnections() const    { return mObject->supportsOrderingConnections(); }

    long getNumIns() const                      { return static_cast<long>(mObject->getNumIns()); }
    long getNumOuts() const                     { return static_cast<long>(mObject->getNumOuts()); }
    long getNumAudioIns() const                 { return mObject->getNumAudioIns(); }
    long getNumAudioOuts() const                { return mObject->getNumAudioOuts(); }
    
    unsigned long getSpecifiedStreams() const   { return mSpecifiedStreams; }

    // Perform and DSP

    void perform(int vec_size)
    {
        if (handlesAudio() && isRealtime())
        {
            // Copy Audio In
        
            for (int i = 0; i < getNumAudioIns(); i++)
                for (int j = 0; j < vec_size; j++)
                    mTempIns[i][j] = getAudioIn(i)[j];
                
            mObject->blockUpdate(mTempIns.data(), mTempOuts.data(), vec_size);
        
            for (int i = 0; i < getNumAudioOuts(); i++)
                for (int j = 0; j < vec_size; j++)
                    getAudioOut(i)[j] = mTempOuts[i][j];
        
            // Zero frame outputs
            
            for (long i = getNumAudioOuts(); i < getNumAudioOuts() + getNumOuts(); i++)
                std::fill_n(getAudioOut(i), vec_size, t_sample(0));
            
            mGlobal->contextMessages(getContext(), asObject());
        }
        else
        {
            // Zero outputs
            
            for (int i = 0; i < getNumAudioOuts() + getNumOuts(); i++)
                std::fill_n(getAudioOut(i), vec_size, t_sample(0));
        }
    }
    
    void dsp(t_signal **sp)
    {
        mGlobal->dspBuilding(true);
        
        bool realtime = isRealtime();
        double samplingRate = sp[0]->s_sr;
        int vec_size = sp[0]->s_vecsize;
        
        // Resolve connections (in case there are no schedulers left in the patch) and mark unresolved for next time
        
        if (realtime)
        {
            resolveConnections();
            mResolved = false;
        
            // Reset DSP
        
            mObject->reset(samplingRate, vec_size);
        }
    
        // Add a perform routine to the chain whether or not the object handles audio (RT or Non-RT)
        
        addPerform<FrameLib_PDClass, &FrameLib_PDClass<T>::perform>(sp);

        if (handlesAudio())
        {
            // Resolve this context
            
            if (realtime && getType() == ObjectType::Scheduler && mGlobal->setLastResolved(mObject->getContext(), clock_getlogicaltime()))
                resolveGraph(samplingRate, vec_size, true);
                    
            // Prepare temporary memory
            
            mTemp.resize(vec_size * (getNumAudioIns() + getNumAudioOuts()));
            mTempIns.resize(getNumAudioIns());
            mTempOuts.resize(getNumAudioOuts());
        
            for (int i = 0; i < getNumAudioIns(); i++)
                mTempIns[i] = mTemp.data() + (i * vec_size);
            for (int i = 0; i < getNumAudioOuts(); i++)
                mTempOuts[i] = (mTemp.data() + (getNumAudioIns() * vec_size)) + (i * vec_size);
            
            // If realtime set the final object
            
            if (realtime)
                mGlobal->finalObject(getContext()) = asObject();
        }
        
        mGlobal->dspBuilding(false);
    }

    // Non-realtime processing
    
    static unsigned long maxBlockSize() { return 16384UL; }
    
    void reset(t_deffloatarg sampleRate = 0.0)
    {
        if (!isRealtime())
            resolveNRTGraph(sampleRate > 0.0 ? sampleRate.mValue : sys_getsr(), true);
    }
    
    void process(t_floatarg length)
    {
        unsigned long updateLength = length > 0 ? static_cast<unsigned long>(length) : 0UL;
        unsigned long time = static_cast<unsigned long>(mObject->getBlockTime()) - 1UL;
        
        if (!updateLength || isRealtime())
            return;
        
        resolveNRTGraph(0.0, false);
        
        // Retrieve all the audio objects in a list
        
        std::vector<FrameLib_PDNRTAudio> audioObjects;
        traversePatch(FrameLib_PDPrivate::messageFindAudioObjects(), &audioObjects);
        
        // Set up buffers
        
        unsigned long maxAudioIO = 0;
        
        for (auto it = audioObjects.begin(); it != audioObjects.end(); it++)
            maxAudioIO = std::max(maxAudioIO, it->mObject->getNumAudioChans());
        
        std::vector<double> audioBuffer(maxBlockSize() * maxAudioIO);
        std::vector<double *> ioBuffers(maxAudioIO);
        
        for (unsigned long i = 0; i < maxAudioIO; i++)
            ioBuffers[i] = audioBuffer.data() + i * maxBlockSize();
        
        // Loop to process audio
        
        for (unsigned long i = 0; i < updateLength; i += maxBlockSize())
        {
            unsigned long blockSize = std::min(maxBlockSize(), updateLength - i);
            
            // Process inputs and schedulers (block controls object lifetime)
            
            if (true)
            {
                FrameLib_AudioQueue queue;
                
                for (auto it = audioObjects.begin(); it != audioObjects.end(); it++)
                {
                    if (it->mObject->getType() == ObjectType::Output)
                        continue;
                    
                    read(it->mBuffer, ioBuffers.data(), it->mObject->getNumAudioIns(), blockSize, time + i);
                    it->mObject->blockUpdate(ioBuffers.data(), nullptr, blockSize, queue);
                }
            }
            
            // Process outputs
            
            for (auto it = audioObjects.begin(); it != audioObjects.end(); it++)
            {
                if (it->mObject->getType() != ObjectType::Output)
                    continue;
                
                it->mObject->blockUpdate(nullptr, ioBuffers.data(), blockSize);
                write(it->mBuffer, ioBuffers.data(), it->mObject->getNumAudioOuts(), blockSize, time + i);
            }
        }
        
        // Process Messages
        
        mGlobal->contextMessages(getContext());
    }
    
    // Connection Routines

    void frame()
    {
        if (!getNumAudioIns())
            frameInlet(0);
    }
    
    void frameInlet(long index)
    {
        const PDConnection connection = mGlobal->getConnection();
                
        // Make sure there's an object to connect that has the same framelib version
        
        if (!connection.mObject || versionMismatch(connection.mObject, index, true))
            return;
        
        if (mGlobal->getConnectionMode() == ConnectionMode::kConnect)
        {
            connect(connection, index);
        }
        else if (mConfirmation)
        {
            if (mConfirmation->confirm(connection, index) && mGlobal->getConnectionMode() == ConnectionMode::kDoubleCheck)
                mInputs[index].reportError(Input::kExtra);
        }
    }

    void anything(t_symbol *s, long ac, t_atom *av)
    {
        if (s && s != gensym("signal"))
            pd_error(asPD(), "%s: no method for '%s'", class_getname(*asPD()), s->s_name);
    }
    
    // External methods (A_CANT)
    
    static void extFindAudio(FrameLib_PDClass *x, std::vector<FrameLib_PDNRTAudio> *objects)
    {
        objects->push_back(FrameLib_PDNRTAudio{x->mObject.get(), x->mBuffer});
    }
    
    static void extResolveContext(FrameLib_PDClass *x)
    {
        x->resolveGraph(sys_getsr(), static_cast<intptr_t>(maxBlockSize()), false);
    }
    
    static void extResolveConnections(FrameLib_PDClass *x, intptr_t *flag)
    {
        bool updated = x->resolveConnections();
        *flag = *flag || updated;
    }
    
    static void extMarkUnresolved(FrameLib_PDClass *x)
    {
        x->mResolved = false;
    }
    
    static void extMakeAutoOrdering(FrameLib_PDClass *x)
    {
        x->mObject->makeAutoOrderingConnections();
    }
    
    static void extClearAutoOrdering(FrameLib_PDClass *x)
    {
        x->mObject->clearAutoOrderingConnections();
    }

    static void extReset(FrameLib_PDClass *x, const double *samplerate, intptr_t maxvectorsize)
    {
        x->mObject->reset(*samplerate, static_cast<unsigned long>(maxvectorsize));
    }
    
    static void extConnectionUpdate(FrameLib_PDClass *x, intptr_t state)
    {
        x->mConnectionsUpdated = state;
    }
    
    static FLObject *extGetFLObject(FrameLib_PDClass *x, uint64_t *version)
    {
        *version = FrameLib_PDPrivate::version();
        return x->mObject.get();
    }

    static void extConnectionConfirm(FrameLib_PDClass *x, unsigned long index, ConnectionMode mode)
    {
        x->makeConnection(index, mode);
    }
    
    // id attribute
        
    void idSet(t_symbol *name)
    {
        mPDContext.mName = name;
        updateContext();
    }
    
    // rt attribute
    
    void rtSet(t_floatarg arg)
    {
        mPDContext.mRealtime = arg;
        updateContext();
    }
    
    // buffer attribute
    
    void bufferSet(t_symbol *buffer)
    {
        mBuffer = buffer;
    }
    
private:
    
    // Update the context if any of the pd context values have changed
    
    void updateContext()
    {
        if (mObject)
        {
            FrameLib_Context context = mGlobal->makeContext(mPDContext);
            
            if (context != mObject->getContext())
            {
                if (!mGlobal->isRealtime(context))
                    mGlobal->dspDeferResume(true);
                
                matchContext(context, true);
                                
                if (!isRealtime())
                    resolveNRTGraph(0.0, false);
            }
            
            // N.B. release because otherwise it is retained twice
            
            mGlobal->releaseContext(context);
        }
    }
    
    // Attempt to match the context to a specified one
    
    void matchContext(FrameLib_Context context, bool force)
    {
        FrameLib_PDContext pdContext = mGlobal->getPDContext(context);
        FrameLib_Context current = getContext();
        bool mismatchedPatch = mGlobal->getPDContext(current).mCanvas != pdContext.mCanvas;
        bool oldState = false;
        
        unsigned long size = 0;
        
        if ((!force && mismatchedPatch) || current == context)
            return;
        
        mResolved = false;
                
        T *newObject = new T(context, mObject->getSerialised(), mProxy.get(), mSpecifiedStreams);
        
        for (unsigned long i = 0; i < mObject->getNumIns(); i++)
            if (const double *values = mObject->getFixedInput(i, &size))
                newObject->setFixedInput(i, values, size);
        
        if (mGlobal->isRealtime(context) || mGlobal->isRealtime(current))
            oldState = mGlobal->dspSuspendRequest();
        
        mPDContext = pdContext;
        mGlobal->retainContext(context);
        mGlobal->releaseContext(current);
        mGlobal->flushErrors(context, asObject());
        
        mObject.reset(newObject);
        
        mGlobal->pushToQueue(asObject());
        
        mGlobal->dspResumeRequest(oldState);
    }
    
    // Private connection methods
    
    template <typename...Args>
    void iterateCanvas(t_glist *gl, t_symbol *method, Args...args)
    {        
        // Call method on all objects
                
        for (t_gobj *g = gl->gl_list; g; g = g->g_next)
        {
            if (t_object *object = pd_checkobject(&g->g_pd))
                objectMethod<void>(object, method, args...);
        }
    }
    
    template <typename...Args>
    void traversePatch(const char *theMethodName, Args...args)
    {
        t_symbol *theMethod = gensym(theMethodName);
        
        // Clear the queue and after traversing call objects added to the queue
        
        mGlobal->clearQueue();
        
        iterateCanvas(mCanvas, theMethod, args...);
        
        while (t_object *object = mGlobal->popFromQueue())
            objectMethod<void>(object, theMethodName, args...);
    }
    
    void resolveGraph(double sampleRate, intptr_t vecSize, bool force)
    {
        if (!force && isRealtime() && dspIsRunning())
            return;
        
        intptr_t updated = false;
        
        mGlobal->dspDeferResume(true);
        
        mGlobal->setReportContextErrors(true);
        traversePatch(FrameLib_PDPrivate::messageMarkUnresolved());
        traversePatch(FrameLib_PDPrivate::messageResolveConnections(), &updated);
        traversePatch(FrameLib_PDPrivate::messageConnectionUpdate(), intptr_t(false));
        mGlobal->setReportContextErrors(false);
        
        // If updated then redo auto ordering connections
        
        if (updated)
        {
            traversePatch(FrameLib_PDPrivate::messageClearAutoOrdering());
            traversePatch(FrameLib_PDPrivate::messageMakeAutoOrdering());
        }
          
        if (updated || force)
            traversePatch(FrameLib_PDPrivate::messageReset(), &sampleRate, vecSize);
        
        mGlobal->dspDeferResume(false);
    }
    
    void resolveNRTGraph(double sampleRate, bool forceReset)
    {
        resolveGraph(0.0, static_cast<intptr_t>(maxBlockSize()), forceReset);
    }
    
    // Convert from framelib object to max object and vice versa
    
    static FLObject *toFLObject(t_object *x)
    {
        return FrameLib_PDPrivate::toFrameLibObject(x);
    }
    
    static t_object *toPDObject(FLObject *object)
    {
        return object ? object->getProxy()->getOwner<t_object>() : nullptr;
    }
    
    bool versionMismatch(t_object *object, long inIdx, bool report) const
    {
        bool mismatch = FrameLib_PDPrivate::versionMismatch(object);

        if (mismatch && report)
            mInputs[inIdx].reportError(Input::kVersion);
            
        return mismatch;
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

    bool resolveConnections()
    {
        if (mResolved)
            return false;

        // Confirm input connections
        
        for (long i = 0; i < getNumIns(); i++)
            confirmConnection(i, ConnectionMode::kConfirm);
        
        // Confirm ordering connections
        
        for (long i = 0; i < getNumOrderingConnections(); i++)
            confirmConnection(getOrderingConnection(i), getNumIns(), ConnectionMode::kConfirm);
        
        // Make output connections
        
        for (long i = getNumOuts(); i > 0; i--)
            makeConnection(i - 1, ConnectionMode::kConnect);
        
        // Check if anything has updated since the last call to this method and make realtime resolved
        
        bool updated = mConnectionsUpdated;
        mConnectionsUpdated = false;
        mResolved = true;

        return updated;
    }

    void makeConnection(unsigned long index, ConnectionMode mode)
    {
        mGlobal->setConnection(PDConnection(asObject(), index));
        mGlobal->setConnectionMode(mode);
        outlet_anything(mOutputs[index], gensym("signal"), 0, nullptr);
        mGlobal->setConnection(PDConnection());
    }
    
    bool confirmConnection(unsigned long inIndex, ConnectionMode mode)
    {
        return confirmConnection(getConnection(inIndex), inIndex, mode);
    }
    
    bool confirmConnection(PDConnection connection, unsigned long inIndex, ConnectionMode mode)
    {
        if (!validInput(inIndex) || !connection.mObject)
            return false;
        
        ConnectionConfirmation confirmation(connection, inIndex);
        mConfirmation = &confirmation;
        objectMethod(connection.mObject, FrameLib_PDPrivate::messageConnectionConfirm(), intptr_t(connection.mIndex), mode);
        mConfirmation = nullptr;
        
        if (!confirmation.mConfirm)
            disconnect(connection, inIndex);
        
        return confirmation.mConfirm;
    }
    
    void connect(PDConnection connection, long inIdx)
    {
        ConnectionResult result;
        FLConnection internalConnection = toFLConnection(connection);
        
        if (!(validOutput(connection.mIndex, internalConnection.mObject) && (isOrderingInput(inIdx) || (validInput(inIdx) && getConnection(inIdx) != connection && !confirmConnection(inIdx, ConnectionMode::kDoubleCheck)))))
            return;
        
        matchContext(internalConnection.mObject->getContext(), false);

        if (isOrderingInput(inIdx))
            result = mObject->addOrderingConnection(internalConnection);
        else
            result = mObject->addConnection(internalConnection, inIdx);

        switch (result)
        {
            case ConnectionResult::Success:
                mConnectionsUpdated = true;
                objectMethod(connection.mObject, FrameLib_PDPrivate::messageConnectionUpdate(), intptr_t(true));
                break;
                
            case ConnectionResult::FeedbackDetected:
                mInputs[inIdx].reportError(Input::kFeedback);
                break;
                
            case ConnectionResult::WrongContext:
                if (mGlobal->getReportContextErrors())
                    mInputs[inIdx].reportError(Input::kContext);
                break;
                
            case ConnectionResult::SelfConnection:
                mInputs[inIdx].reportError(Input::kDirect);
                break;
                
            case ConnectionResult::NoOrderingSupport:
            case ConnectionResult::Aliased:
                break;
        }
    }
    
    void disconnect(PDConnection connection, long inIdx)
    {
        if (!(isOrderingInput(inIdx) || (validInput(inIdx) && getConnection(inIdx) == connection)))
            return;
        
        if (isOrderingInput(inIdx))
            mObject->deleteOrderingConnection(toFLConnection(connection));
        else
            mObject->deleteConnection(inIdx);
        
        mConnectionsUpdated = true;
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
            t_symbol *sym = atom_getsymbol_default(a);
            
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
                pd_error(asObject(), "string %s in entry list where value expected", atom_getsymbol_default(argv + idx)->s_name);
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
                    t_symbol *str = atom_getsymbol_default(argv + i);
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
            t_symbol *sym = atom_getsymbol_default(argv + i++);
            
            if (isParameterTag(sym))
            {
                // Check for missing values
                
                if ((i >= argc) || isTag(argv + i))
                {
                    pd_error(asObject(), "no values given for parameter %s", sym->s_name);
                    continue;
                }
                
                // Add to parameters with stray item detection
                
                if (atom_gettype(argv + i) == A_SYMBOL)
                {
                    serialisedParameters.write(sym->s_name + 1, atom_getsymbol_default(argv + i++)->s_name);
                    
                    if (i < argc && !isTag(argv + i))
                        pd_error(asObject(), "stray items after parameter %s", sym->s_name);
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
            t_symbol *sym = atom_getsymbol_default(argv + i++);
            
            if (isInputTag(sym))
            {
                i = parseNumericalList(values, argv, argc, i);
                mObject->setFixedInput(inputNumber(sym), values.data(), static_cast<unsigned long>(values.size()));
                
                if (inputNumber(sym) >= static_cast<unsigned long>(getNumIns()))
                    pd_error(asObject(), "input %s out of bounds", sym->s_name);
            }
        }
    }

    // Attribute parsing
    
    void parseAttributes(long argc, t_atom *argv)
    {
        long i = 0;
        
        // Parse attribute tags
        
        while (i < argc)
        {
            t_symbol *sym = atom_getsymbol_default(argv + i++);
        
            if (isAttributeTag(sym))
            {
                // Check attributes are valid
                
                bool commonAttributes = isNamedAttributeTag(sym, "rt") || isNamedAttributeTag(sym, "id");
                bool bufferAttribute = handlesAudio() && isNamedAttributeTag(sym, "buffer");
                
                if (!commonAttributes && !bufferAttribute)
                {
                    pd_error(asObject(), "unknown attribute %s", sym->s_name);
                    continue;
                }
                
                // Check for missing values
                
                if ((i >= argc) || isTag(argv + i))
                {
                    pd_error(asObject(), "no values given for attribute %s", sym->s_name);
                    continue;
                }
                
                if (isNamedAttributeTag(sym, "rt"))
                    rtSet(atom_getfloat(argv + i++));
                else if (isNamedAttributeTag(sym, "id"))
                    idSet(atom_getsymbol_default(argv + i++));
                else if (isNamedAttributeTag(sym, "buffer"))
                    bufferSet(atom_getsymbol_default(argv + i++));
                
                if (i < argc && !isTag(argv + i))
                    pd_error(asObject(), "stray items after attribute %s", sym->s_name);
            }
        }
    }
    
    // Buffer access (read and write multichannel buffers)
    
    void read(t_symbol *buffer, double **outs, size_t numChans, size_t size, size_t offset)
    {
        // FIX - multichannel for read and write?
        
        pd_buffer access(buffer);
        
        for (size_t i = 0; i < numChans; i++)
            access.read(outs[i], size, offset, i);
    }
    
    void write(t_symbol *buffer, const double * const *ins, size_t numChans, size_t size, size_t offset)
    {
        pd_buffer access(buffer);

        for (size_t i = 0; i < numChans; i++)
            access.write(ins[i], size, offset, i);
    }

protected:
    
    MessageHandler *getHandler() { return mGlobal->getHandler(getContext()); }

    // Proxy
    
    std::unique_ptr<FrameLib_PDProxy> mProxy;
    
private:
    
    // Data - N.B. - the order is crucial for safe deconstruction
    
    FrameLib_PDGlobals::ManagedPointer mGlobal;
    
    std::unique_ptr<FLObject> mObject;
    
    std::vector<Input> mInputs;
    std::vector<t_outlet *> mOutputs;
    
    std::vector<double *> mTempIns;
    std::vector<double *> mTempOuts;
    std::vector<double> mTemp;
    
    t_glist *mCanvas;
        
    ConnectionConfirmation *mConfirmation;
        
    unsigned long mSpecifiedStreams;
    
    bool mConnectionsUpdated;
    bool mContextPatchConfirmed;
    bool mResolved;
    
    t_symbol *mBuffer;
    FrameLib_PDContext mPDContext;
};

// Convenience for Objects Using FrameLib_Expand (use FrameLib_PDClass_Expand<T>::makeClass() to create)

template <class T, PDObjectArgsMode argsMode = kAsParams>
using FrameLib_PDClass_Expand = FrameLib_PDClass<FrameLib_Expand<T>, argsMode>;

#endif
