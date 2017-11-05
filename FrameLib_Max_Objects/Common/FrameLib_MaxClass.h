
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
    : mGlobal(NULL), mConnectionInfo(NULL), mCount(0) { FrameLib_Global::get(&mGlobal); }
    ~FrameLib_MaxGlobals() { FrameLib_Global::release(&mGlobal); }

    // Getters and setters for max global items
    
    FrameLib_Global *getGlobal() const                      { return mGlobal; }
    
    const ConnectionInfo *getConnectionInfo() const         { return mConnectionInfo; }
    void setConnectionInfo(ConnectionInfo *info = NULL)     { mConnectionInfo = info; }

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
    long mCount;
};

//////////////////////////////////////////////////////////////////////////
/////////////////////// FrameLib Max Object Class ////////////////////////
//////////////////////////////////////////////////////////////////////////

template <class T, bool argsSetAllInputs = false> class FrameLib_MaxClass : public MaxClass_Base
{
    typedef FrameLib_Object<FrameLib_MultiChannel>::Connection FrameLibConnection;
    typedef FrameLib_Object<t_object>::Connection MaxConnection;
    typedef FrameLib_MaxGlobals::ConnectionInfo ConnectionInfo;
    typedef std::vector<t_object *>::iterator MaxObjectIterator;

public:
    
    // Class Initialisation (must explicitly give U for classes that inherit from FrameLib_MaxClass<>)
    
    template <class U = FrameLib_MaxClass<T, argsSetAllInputs> > static void makeClass(t_symbol *nameSpace, const char *className)
    {
        MaxClass_Base::makeClass<U>(nameSpace, className);
    }
    
    static void classInit(t_class *c, t_symbol *nameSpace, const char *classname)
    {
        addMethod(c, (method) dspstate, "dspstate");
        addMethod<FrameLib_MaxClass<T>, &FrameLib_MaxClass<T>::assist>(c, "assist");
        addMethod<FrameLib_MaxClass<T>, &FrameLib_MaxClass<T>::info>(c, "info");
        addMethod<FrameLib_MaxClass<T>, &FrameLib_MaxClass<T>::frame>(c, "frame");
        addMethod<FrameLib_MaxClass<T>, &FrameLib_MaxClass<T>::dsp>(c);
        addMethod(c, (method) &externalPatchLineUpdate, "patchlineupdate");
        addMethod(c, (method) &externalConnectionAccept, "connectionaccept");
        addMethod(c, (method) &externalResolveConnections, "__fl.resolve_connections");
        addMethod(c, (method) &externalAutoOrderingConnections, "__fl.auto_ordering_connections");
        addMethod(c, (method) &externalClearAutoOrderingConnections, "__fl.clear_auto_ordering_connections");
        addMethod(c, (method) &externalIsConnected, "__fl.is_connected");
        addMethod(c, (method) &externalConnectionConfirm, "__fl.connection_confirm");
        addMethod(c, (method) &externalGetInternalObject, "__fl.get_internal_object");
        addMethod(c, (method) &externalGetNumAudioIns, "__fl.get_num_audio_ins");
        addMethod(c, (method) &externalGetNumAudioOuts, "__fl.get_num_audio_outs");
        
        dspInit(c);
    }

    // Constructor and Destructor
    
    FrameLib_MaxClass(t_symbol *s, long argc, t_atom *argv) : mConfirmObject(NULL), mConfirmInIndex(-1), mConfirmOutIndex(-1), mConfirm(false), mNeedsResolve(true), mTopLevelPatch(jpatcher_get_toppatcher(gensym("#P")->s_thing))
    {
        // Object creation with parameters and arguments (N.B. the object is not a member due to size restrictions)
        
        unsigned long nStreams = 1;
        
        if (argc && (getStreamCount(argv) > 0))
        {
            nStreams = getStreamCount(argv);
            argv++;
            argc--;
        }
        
        FrameLib_Parameters::AutoSerial serialisedParameters;
        parseParameters(serialisedParameters, argc, argv);
        mObject = new T(FrameLib_Context(mGlobal->getGlobal(), mTopLevelPatch), &serialisedParameters, this, nStreams);
        parseInputs(argc, argv);
        
        long numIns = getNumIns() + (supportsOrderingConnections() ? 1 : 0);

        mInputs.resize(numIns);
        mOutputs.resize(getNumOuts());
        
        // Create frame inlets and outlets
        
        // N.B. - we create a proxy if the inlet is not the first inlet (not the first frame input or the object has audio inputs)
        
        for (long i = numIns - 1; i >= 0; i--)
            mInputs[i] = (t_object *) ((i || getNumAudioIns()) ? proxy_new(this, getNumAudioIns() + i, &mProxyNum) : NULL);
        
        for (unsigned long i = getNumOuts(); i > 0; i--)
            mOutputs[i - 1] = outlet_new(this, NULL);
        
        // Setup for audio, even if the object doesn't handle it, so that dsp recompile works correctly
        
        dspSetup(getNumAudioIns(), mObject->getType() == kOutput ? Z_PUT_LAST : Z_PUT_FIRST);
 
        for (unsigned long i = 0; i < getNumAudioOuts(); i++)
            outlet_new(this, "signal");
    }

    ~FrameLib_MaxClass()
    {
        dspFree();

        delete mObject;

        for (MaxObjectIterator it = mInputs.begin(); it != mInputs.end(); it++)
            object_free(*it);
    }
    
    void assist(void *b, long m, long a, char *s)
    {
        if (m == ASSIST_OUTLET)
        {
            if (a < getNumAudioOuts())
                sprintf(s,"(signal) %s", mObject->audioInfo(a - 1).c_str());
            else
                sprintf(s,"(frame) %s", mObject->outputInfo(a - getNumAudioOuts()).c_str());
        }
        else
        {
            if (a < getNumAudioIns())
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
        
        object_post(*this, "********* %s *********", object_classname(*this)->s_name);

        // Description
        
        if (flags & kInfoDesciption)
        {
            object_post(*this, "--- Description ---");
            postSplit(mObject->objectInfo(verbose).c_str(), "", "-");
        }
        
        // IO
        
        if (flags & kInfoInputs)
        {
            object_post(*this, "--- Input List ---");
            if (argsSetAllInputs)
                object_post(*this, "N.B. - arguments set the fixed array values for all inputs.");
            for (long i = 0; i < mObject->getNumAudioIns(); i++)
                object_post(*this, "Audio Input %ld: %s", i + 1, mObject->audioInfo(i, verbose).c_str());
            for (long i = 0; i < mObject->getNumIns(); i++)
                object_post(*this, "Frame Input %ld [%s]: %s", i + 1, frameTypeString(mObject->inputType(i)), mObject->inputInfo(i, verbose).c_str());
            if (supportsOrderingConnections())
                object_post(*this, "Ordering Input [%s]: Connect to ensure ordering", frameTypeString(kFrameAny));
        }
        
        if (flags & kInfoOutputs)
        {
            object_post(*this, "--- Output List ---");
            for (long i = 0; i < mObject->getNumAudioOuts(); i++)
                object_post(*this, "Audio Output %ld: %s", i + 1, mObject->audioInfo(i, verbose).c_str());
            for (long i = 0; i < mObject->getNumOuts(); i++)
                object_post(*this, "Frame Output %ld [%s]: %s", i + 1, frameTypeString(mObject->outputType(i)), mObject->outputInfo(i, verbose).c_str());
        }
        
        // Parameters
        
        if (flags & kInfoParameters)
        {
            object_post(*this, "--- Parameter List ---");
            
            const FrameLib_Parameters *params = mObject->getParameters();
            if (!params || !params->size()) object_post(*this, "< No Parameters >");
            
            // Loop over parameters
            
            for (long i = 0; params && i < params->size(); i++)
            {
                FrameLib_Parameters::Type type = params->getType(i);
                FrameLib_Parameters::NumericType numericType = params->getNumericType(i);
                std::string defaultStr = params->getDefaultString(i);

                // Name, type and default value
                
                if (defaultStr.size())
                    object_post(*this, "Parameter %ld: %s [%s] (default: %s)", i + 1, params->getName(i).c_str(), params->getTypeString(i).c_str(), defaultStr.c_str());
                else
                    object_post(*this, "Parameter %ld: %s [%s]", i + 1, params->getName(i).c_str(), params->getTypeString(i).c_str());

                // Verbose - arguments, range (for numeric types), enum items (for enums), array sizes (for arrays), description
                
                if (verbose)
                {
                    if (!argsSetAllInputs && params->getArgumentIdx(i) >= 0)
                        object_post(*this, "- Argument: %ld", params->getArgumentIdx(i) + 1);
                    if (numericType == FrameLib_Parameters::kNumericInteger || numericType == FrameLib_Parameters::kNumericDouble)
                    {
                        switch (params->getClipMode(i))
                        {
                            case FrameLib_Parameters::kNone:    break;
                            case FrameLib_Parameters::kMin:     object_post(*this, "- Min Value: %lg", params->getMin(i));                        break;
                            case FrameLib_Parameters::kMax:     object_post(*this, "- Max Value: %lg", params->getMax(i));                        break;
                            case FrameLib_Parameters::kClip:    object_post(*this, "- Clipped: %lg-%lg", params->getMin(i), params->getMax(i));   break;
                        }
                    }
                    if (type == FrameLib_Parameters::kEnum)
                        for (long j = 0; j <= params->getMax(i); j++)
                            object_post(*this, "   [%ld] - %s", j, params->getItemString(i, j).c_str());
                    else if (type == FrameLib_Parameters::kArray)
                        object_post(*this, "- Array Size: %ld", params->getArraySize(i));
                    else if (type == FrameLib_Parameters::kVariableArray)
                        object_post(*this, "- Array Max Size: %ld", params->getArrayMaxSize(i));
                    postSplit(params->getInfo(i).c_str(), "- ", "-");
                }
            }
        }
    }

    // IO Helpers
    
    bool supportsOrderingConnections()      { return mObject->supportsOrderingConnections(); }
    bool handlesAudio()                     { return mObject->handlesAudio(); }
    
    long getNumAudioIns()   { return (long) mObject->getNumAudioIns(); }
    long getNumAudioOuts()  { return (long) mObject->getNumAudioOuts(); }
    long getNumIns()        { return (long) mObject->getNumIns(); }
    long getNumOuts()       { return (long) mObject->getNumOuts(); }
    
    // Perform and DSP

    void perform(t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
    {
        mObject->blockUpdate(ins, outs, vec_size);
    }

    void dsp(t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
    {
        // Resolve connections
        
        if (mNeedsResolve)
        {
            traversePatch(mTopLevelPatch, gensym("__fl.resolve_connections"));
            traversePatch(mTopLevelPatch, gensym("__fl.clear_auto_ordering_connections"));
            traversePatch(mTopLevelPatch, gensym("__fl.auto_ordering_connections"));
        }
        
        // Reset DSP
        
        mObject->reset(samplerate, maxvectorsize);
        
        // Add a perform routine to the chain if the object handles audio
        
        if (handlesAudio())
            addPerform<FrameLib_MaxClass, &FrameLib_MaxClass<T>::perform>(dsp64);
    }
    
    static void dspstate(FrameLib_MaxClass *x, long n)
    {
        // Object needs resolving
        
        x->mNeedsResolve = true;
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
                        object_error(*this, "extra connection to input %ld", index + 1);
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
    
    static FrameLib_MultiChannel *externalGetInternalObject(FrameLib_MaxClass *x)
    {
        return x->mObject;
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

    // Get an internal object from a generic pointer safely
    
    FrameLib_MultiChannel *getInternalObject(t_object *x)
    {
        return (FrameLib_MultiChannel *) object_method(x, gensym("__fl.get_internal_object"));
    }
    
    // Private connection methods
    
    void traversePatch(t_patcher *p, t_symbol *method)
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
                traversePatch(p, method);

            object_method(jbox_get_object(b), method);
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
        outlet_anything(mOutputs[index], gensym("frame"), 0, NULL);
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
        mConfirmObject = NULL;
        mConfirmInIndex = mConfirmOutIndex = -1;
        
        return result;
    }
    
    bool validInput(long index, FrameLib_MultiChannel *object) const        { return object && index >= 0 && index < object->getNumIns(); }
    bool validOutput(long index, FrameLib_MultiChannel *object) const       { return object && index >= 0 && index < object->getNumOuts(); }
    bool isOrderingInput(long index, FrameLib_MultiChannel *object) const   { return object && object->supportsOrderingConnections() && index == object->getNumIns(); }
    bool validInput(long index) const                                       { return validInput(index, mObject); }
    bool validOutput(long index) const                                      { return validOutput(index, mObject); }
    bool isOrderingInput(long index) const                                  { return isOrderingInput(index, mObject); }
    
    bool isConnected(long index) const                                      { return mObject->isConnected(index); }
    
    MaxConnection getConnection(long index) const
    {
        if (isConnected(index))
        {
            FrameLibConnection connection = mObject->getConnection(index);
            return MaxConnection((t_object *) connection.mObject->getOwner(), connection.mIndex);
        }
        else
            return MaxConnection();
    }
    
    unsigned long getNumOrderingConnections() const
    {
        return mObject->getNumOrderingConnections();
    }
    
    MaxConnection getOrderingConnection(long index) const
    {
        FrameLibConnection connection = mObject->getOrderingConnection(index);
        return MaxConnection((t_object *) connection.mObject->getOwner(), connection.mIndex);
    }
    
    bool matchConnection(t_object *src, long outIdx, long inIdx) const
    {
        MaxConnection connection = getConnection(inIdx);
        return connection.mObject == src && connection.mIndex == outIdx;
    }
    
    void connect(t_object *src, long outIdx, long inIdx)
    {
        FrameLib_MultiChannel *object = getInternalObject(src);
        
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
                object_error(*this, "feedback loop detected");
                break;
                
            case kConnectWrongContext:
                object_error(*this, "cannot connect objects from different top-level patchers");
                break;
                
            case kConnectSelfConnection:
                object_error(*this, "direct feedback loop detected");
                break;
                
            case kConnectSuccess:
            case kConnectNoOrderingSupport:
            case kConnectAliased:
                break;
        }
    }
    
    void disconnect(t_object *src, long outIdx, long inIdx)
    {
        FrameLib_MultiChannel *object = getInternalObject(src);

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
            srcout -= (long) object_method(src, gensym("__fl.get_num_audio_outs"));
            dstin -= getNumAudioIns();
            
            if (sys_getdspobjdspstate(*this))
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

        dstin -= (long) object_method(dst, gensym("__fl.get_num_audio_ins"));
        
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
            object_post(*this, "%s%s", oldPos ? lineTag : firstLineTag, str.substr(oldPos, (pos - oldPos) + 1).c_str());
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
    
    long getStreamCount(t_atom *a)
    {
        if (atom_gettype(a) == A_SYM)
        {
            t_symbol *sym = atom_getsym(a);
            
            // FIX - need to check better here!
            
            if (strlen(sym->s_name) > 1 && sym->s_name[0] == '=')
                return atoi(sym->s_name + 1);
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
            
            if (atom_gettype(argv + idx) == A_SYM)
                object_error(*this, "string %s in entry list where value expected", atom_getsym(argv + idx)->s_name);
            
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
                    object_error(*this, "stray items after entry %s", sym->s_name);
            }
            
            // Check for lack of values or end of list
            
            if ((++i >= argc) || isTag(argv + i))
            {
                if (i < (argc + 1))
                    object_error(*this, "no values given for entry %s", sym->s_name);
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
    
    std::vector<t_object *> mInputs;
    std::vector<void *> mOutputs;

    long mProxyNum;
    t_object *mConfirmObject;
    long mConfirmInIndex;
    long mConfirmOutIndex;
    bool mConfirm;
    bool mNeedsResolve;
    
    t_object *mTopLevelPatch;
    
    FrameLib_MaxGlobals::ManagedPointer mGlobal;
};

// Convenience for Objects Using FrameLib_Expand (use FrameLib_MaxClass_Expand<T>::makeClass() to create)

template <class T, bool argsSetAllInputs = false>
class FrameLib_MaxClass_Expand : public FrameLib_MaxClass<FrameLib_Expand<T>, argsSetAllInputs> {};
