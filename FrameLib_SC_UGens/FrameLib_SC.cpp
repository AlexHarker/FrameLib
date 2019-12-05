
#include "SC_PlugIn.h"

#include "FrameLib_Objects.h"

static InterfaceTable *ft;

// Declare struct to hold unit generator state

struct FrameLib_SC_UGen : public Unit
{
    typedef void (*FLCalcFunc)(FrameLib_SC_UGen *, int);

    FrameLib_Multistream *mObject;
    FrameLib_Proxy *mProxy;
    FLCalcFunc mFrameLibCalcFunc;
    double **mAudioBuffers;
};

// Calculation functions declarations (also used to identify units)

void FLTest_CalcZero(FrameLib_SC_UGen *unit, int inNumSamples);
void FLTest_CalcAudio(FrameLib_SC_UGen *unit, int inNumSamples);

// Declare unit generator desstructor

static void FLTest_Dtor(FrameLib_SC_UGen* unit);

// Global pbject

struct SC_FrameLib_Global
{
    static void CalcFunc(FrameLib_SC_UGen *unit, int inNumSamples)
    {
        unit->mFrameLibCalcFunc(unit, inNumSamples);
    }

    class Notifier : public FrameLib_ErrorReporter::HostNotifier
    {
        bool notify(const FrameLib_ErrorReporter::ErrorReport& report) override
        {
            char errorText[512];
            report.getErrorText(errorText, 512);
            ft->fPrint("error: %s\n", errorText);
            return true;
        }
    };
    
    SC_FrameLib_Global() : mGlobal(nullptr)
    {
        FrameLib_Global::get(&mGlobal, &mNotifier);
    }
    
    ~SC_FrameLib_Global()
    {
        FrameLib_Global::release(&mGlobal);
    }
    
    bool CheckFrameLib(Unit* unit)
    {
        return unit && (unit->mCalcFunc == GetCalcFunc());
    }

    UnitCalcFunc GetCalcFunc() const { return (UnitCalcFunc) CalcFunc; }

    FrameLib_Global *getGlobal() { return mGlobal; }
    
    FrameLib_Global *mGlobal;
    Notifier mNotifier;
};

static SC_FrameLib_Global sGlobal;

// Parameter Object

struct FrameLib_Param_UGen : public Unit
{
    SC_FrameLib_Global *mGlobal;
    FrameLib_Parameters::Serial *mSerial;
};

size_t FLParam_String(FrameLib_Param_UGen* unit, char*& str, size_t i)
{
    Wire **it = unit->mInput + i;
    Wire **end = unit->mInput + unit->mNumInputs;
    
    size_t length = (*it++)->mScalarValue;
    
    for (size_t j = 0; it != end && j < length; it++, j++)
    {
        int32 chars = (int32) (*it)->mScalarValue;;
        char c;
        str[j * 3 + 0] = c = (char) (chars & 0xFF);
        str[j * 3 + 1] = c = (char) ((chars >> 0x08) & 0xFF);
        str[j * 3 + 2] = c = (char) ((chars >> 0x10) & 0xFF);
    }
    
    str[length * 3] = 0;
    
    return unit->mNumInputs - (end - it);
}

size_t FLParam_Vector(FrameLib_Param_UGen* unit, double*& vec, size_t i, size_t& N)
{
    Wire **it = unit->mInput + i;
    Wire **end = unit->mInput + unit->mNumInputs;
    
    N = (*it++)->mScalarValue;
    
    for (size_t j = 0; it != end && j < N; it++, j++)
        vec[j] = (*it)->mScalarValue;
    
    return unit->mNumInputs - (end - it);
}

void FLParam_Release_Serial(FrameLib_Param_UGen* unit)
{
    using Serial = FrameLib_Parameters::Serial;
    
    if (unit->mSerial)
    {
        unit->mSerial->~Serial();
        ft->fRTFree(unit->mWorld, unit->mSerial);
    }
}

template <class T>
T *FLParam_Temporary(FrameLib_Param_UGen* unit, T *stack, size_t size, size_t defaultSize)
{
    return size > defaultSize ? (T *) ft->fRTAlloc(unit->mWorld, sizeof(T) * size) : stack;
}

void FLParam_Ctor(FrameLib_Param_UGen* unit)
{
    using Serial = FrameLib_Parameters::Serial;
    const size_t DEF_STR_SIZE = 1024;
    const size_t DEF_VEC_SIZE = 1024;
    
    char stack_str[DEF_STR_SIZE];
    double stack_vec[DEF_VEC_SIZE];
    
    size_t N = 0, max_str = 0, max_vec = 0, size = 0, l1 = 0, l2 = 0;
    
    unit->mGlobal = &sGlobal;
    unit->mSerial = nullptr;
    
    // Loop to calculate sizes;
    
    for (size_t i = 0; i < unit->mNumInputs;  i += 3 + l1 + l2)
    {
        l1 = unit->mInput[i + 1]->mScalarValue;
        l2 = unit->mInput[i + l1 + 2]->mScalarValue;
        
        if (unit->mInput[i]->mScalarValue)
        {
            max_str = std::max(max_str, (l1 + l2) * 3 + 2);
            size += Serial::calcString(l1 * 3, l2 * 3);
        }
        else
        {
            max_str = std::max(max_str, l1 * 3 + 1);
            max_vec = std::max(max_vec, l2);
            size += Serial::calcVector(l1 * 3, l2);
        }
    }
    
    // Allocate if needed
    
    if (size)
    {
        size_t allocationSize = Serial::inPlaceSize(size);
        unit->mSerial = Serial::newInPlace(ft->fRTAlloc(unit->mWorld, allocationSize), size);
    }
    
    char *tag = FLParam_Temporary(unit, stack_str, max_str, DEF_STR_SIZE);
    double *vec = FLParam_Temporary(unit, stack_vec, max_vec, DEF_VEC_SIZE);
    
    // Loop to serialise parameters
    
    for (size_t i = 0; i < unit->mNumInputs; )
    {
        if (unit->mInput[i]->mScalarValue)
        {
            char *str = tag + ((size_t) unit->mInput[i + 1]->mScalarValue) * 3 + 1;
            i = FLParam_String(unit, tag, i + 1);
            i = FLParam_String(unit, str, i);
            unit->mSerial->write(tag, str);
        }
        else
        {
            i = FLParam_String(unit, tag, i + 1);
            i = FLParam_Vector(unit, vec, i, N);
            unit->mSerial->write(tag, vec, N);
        }
    }
    
    // Free resources if needed
    
    if (tag != stack_str)
        ft->fRTFree(unit->mWorld, tag);
    if (vec != stack_vec)
        ft->fRTFree(unit->mWorld, vec);
    
    unit->mCalcFunc = (UnitCalcFunc) &FLTest_CalcZero;
}

void FLParam_Dtor(FrameLib_Param_UGen* unit)
{
    // In case the parameters haven't been freed
    
    FLParam_Release_Serial(unit);
}

//////////////////////////////////////////////////////////////////

// Proxies

template <class T>
FrameLib_Proxy *GetProxy(FrameLib_SC_UGen *, T *)
{
    return nullptr;
}


// Read Proxy Class

struct ReadProxy : public FrameLib_Read::Proxy
{
    ReadProxy(Unit *unit) : mUnit(unit), mBufNum(-1), mBuffer(nullptr) {}
    
    struct fetch : public table_fetcher<double>
    {
        fetch(float *data, int length)
        : table_fetcher(1.0), mData(data), mLength(length) {}
        
        float operator()(intptr_t offset)
        {
            return (offset < 0 || offset >= mLength) ? 0.f : mData[offset];
        }
        
        float *mData;
        int mLength;
    };
    
    void update(const char *name) override
    {
        mBufNum = atoi(name);
    }
    
    void acquire(unsigned long& length, double& samplingRate) override
    {
        World *world = mUnit->mWorld;
        mBuffer = nullptr;
        
        if (mBufNum >= world->mNumSndBufs)
        {
            int localBufNum = mBufNum - world->mNumSndBufs;
            Graph *parent = mUnit->mParent;
            if (localBufNum <= parent->localBufNum)
                mBuffer = parent->mLocalSndBufs + localBufNum;
            else
                mBuffer = nullptr;
        }
        else if (mBufNum >= 0)
            mBuffer = world->mSndBufs + mBufNum;
        
        //ACQUIRE_SNDBUF(mBuffer);
        length = mBuffer->frames;
        samplingRate = mBuffer->samplerate;
    }
    
    void release() override
    {
        //RELEASE_SNDBUF(mBuffer);
    }
    
    void read(double *output, const double *positions, unsigned long size, long chan, InterpType interpType)  override
    {
        if (mBuffer)
        {
            chan = std::max(0L, std::min(chan, static_cast<long>(mBuffer->channels - 1)));
            table_read(fetch(mBuffer->data, mBuffer->frames), output, positions, size, 1.0, interpType);
        }
    }
    
private:

    Unit *mUnit;
    int mBufNum;
    SndBuf *mBuffer;
};

template<>
FrameLib_Proxy *GetProxy(FrameLib_SC_UGen *unit, FrameLib_Expand<FrameLib_Read> *)
{
    return new ReadProxy(unit);
}

//////////////////////////////////////////////////////////////////

// Ctor is called to initialize the unit generator.

// A Ctor usually does 3 things.
// 1. set the calculation function.
// 2. initialize the unit generator state variables.
// 3. calculate one sample of output.

template<class T>
void FLTest_Ctor(FrameLib_SC_UGen* unit)
{
    // FIX - number of streams / fixed inputs

    FrameLib_Param_UGen *paramUGen = (FrameLib_Param_UGen *) unit->mInput[0]->mFromUnit;
    
    SC_FrameLib_Global* global = paramUGen->mGlobal;
    FrameLib_Context context(global->getGlobal(), unit->mParent);
    unit->mProxy = GetProxy<T>(unit, static_cast<T *>(nullptr));
    
    int nStreams = 1;
    unit->mObject = new T(context, paramUGen->mSerial, unit->mProxy, nStreams);
    unit->mAudioBuffers = nullptr;
    bool handlesAudio = T::handlesAudio();
    
    unsigned long numUnitIns = unit->mNumInputs;
    unsigned long numAudioIns = unit->mObject->getNumAudioIns();
    unsigned long numFrameIns = unit->mObject->getNumIns();
    unsigned long numFrameConnections = std::min(numUnitIns >= numAudioIns + 1 ? numUnitIns - (numAudioIns - 1) : 0, numFrameIns);
    unsigned long numAudioChans = unit->mObject->getNumAudioChans();
    
    for (unsigned long i = 0; i < numFrameConnections; i++)
    {
        Wire *wire = unit->mInput[i + numAudioIns + 1];
        Unit *connect = wire->mFromUnit;
        /*
        if (params && params->mInputsSerial)
        {
            char tag[10];
            sprintf(tag, "i%ld", i);
            
            FrameLib_Parameters::Serial::Iterator it = params->mInputsSerial->find(tag);
            
            if (it != params->mInputsSerial->end())
            {
                unsigned long arraySize;
                const double *array = it.getVector(&arraySize);
                unit->mObject->setFixedInput(i, array, arraySize);
                continue;
            }
        }*/
        
        if (global->CheckFrameLib(connect))
        {
            FrameLib_SC_UGen *connectable = reinterpret_cast<FrameLib_SC_UGen *>(connect);
            unsigned long cNumAudioOuts = connectable->mObject->getNumAudioOuts();
            unsigned long o = cNumAudioOuts;

            for (; o < connect->mNumOutputs; o++)
            {
                if (connect->mOutput[o] == wire)
                    break;
            }
            
            if (o < connect->mNumOutputs)
                unit->mObject->addConnection(FrameLib_Multistream::Connection(connectable->mObject, o - cNumAudioOuts), i);
        }
        else if (wire->mCalcRate == calc_ScalarRate)
        {
            double scalar = wire->mScalarValue;
            unit->mObject->setFixedInput(i, &scalar, 1);
        }
    }

    unit->mCalcFunc = global->GetCalcFunc();

    if (handlesAudio)
    {
        if (numAudioChans)
        {
            unit->mAudioBuffers = (double **) RTAlloc(unit->mWorld, sizeof(double *) * numAudioChans);
            unit->mAudioBuffers[0] = (double *) RTAlloc(unit->mWorld, sizeof(double) * numAudioChans * unit->mBufLength);
            for (unsigned long i = 1; i <numAudioChans; i++)
                unit->mAudioBuffers[1] = unit->mAudioBuffers[0] + unit->mBufLength * i;
        }
        unit->mFrameLibCalcFunc = &FLTest_CalcAudio;
    }
    else
        unit->mFrameLibCalcFunc = &FLTest_CalcZero;


    unit->mObject->autoOrderingConnections();
    unit->mObject->reset(unit->mRate->mSampleRate, unit->mRate->mBufLength);
    FLTest_CalcZero(unit, 1);
}

void FLTest_Dtor(FrameLib_SC_UGen* unit)
{
    if (unit->mAudioBuffers)
    {
        RTFree(unit->mWorld, unit->mAudioBuffers[0]);
        RTFree(unit->mWorld, unit->mAudioBuffers);
    }
    delete unit->mObject;
    if (unit->mProxy)
        delete unit->mProxy;
}

//////////////////////////////////////////////////////////////////

void FLTest_OutputZero(FrameLib_SC_UGen *unit, unsigned long output, int numSamples)
{
    float *out = unit->mOutBuf[output];
    
    if (out)
    {
        for (int i = 0; i < numSamples; ++i)
            out[i] = 0.f;
    }
}

void FLTest_CalcZero(FrameLib_SC_UGen *unit, int inNumSamples)
{
    for (int i = 0; i < unit->mNumOutputs; ++i)
        FLTest_OutputZero(unit, i, inNumSamples);
}

void FLTest_CalcAudio(FrameLib_SC_UGen *unit, int inNumSamples)
{
    bool outputObject = unit->mObject->getType() == kOutput;
    unsigned long numAudioChannels = unit->mObject->getNumAudioChans();
    
    if (outputObject)
    {
        unit->mObject->blockUpdate(nullptr, unit->mAudioBuffers, inNumSamples);
        
        unsigned long validOutputs = std::min(numAudioChannels, static_cast<unsigned long>(unit->mNumOutputs));
        
        for (unsigned long i = 0; i < validOutputs; ++i)
        {
            double *temp = unit->mAudioBuffers[i];
            float *out = unit->mOutBuf[i];
            
            for (int j = 0; j < inNumSamples; ++j)
                out[j] = temp[j];
        }
    }
    else
    {
        unsigned long validInputs = std::min(numAudioChannels, static_cast<unsigned long>(unit->mNumInputs - 1));
        
        for (unsigned long i = 0; i < validInputs; ++i)
        {
            float *in = unit->mInBuf[i + 1];
            double *temp = unit->mAudioBuffers[i];
            
            for (int j = 0; j < inNumSamples; ++j)
                temp[j] = in[j];
        }
        
        unit->mObject->blockUpdate(unit->mAudioBuffers, nullptr, inNumSamples);
    }

    for (unsigned long i = unit->mObject->getNumAudioOuts(); i < unit->mNumOutputs; ++i)
        FLTest_OutputZero(unit, i, inNumSamples);
}

//////////////////////////////////////////////////////////////////

template <class T>
void DefineFrameLibUnit(const char *name)
{
    (*ft->fDefineUnit)(name, sizeof(FrameLib_SC_UGen), (UnitCtorFunc)&FLTest_Ctor<T>,(UnitDtorFunc)&FLTest_Dtor, 0);
}

template <class T>
void DefineFrameLibExpUnit(const char *name)
{
    (*ft->fDefineUnit)(name, sizeof(FrameLib_SC_UGen), (UnitCtorFunc)&FLTest_Ctor<FrameLib_Expand<T>>,(UnitDtorFunc)&FLTest_Dtor, 0);
}

PluginLoad(FrameLib)
{
    ft = inTable; // store pointer to InterfaceTable
    
    // Define parameter UGen
    
    (*ft->fDefineUnit)("FLParam", sizeof(FrameLib_Param_UGen), (UnitCtorFunc)&FLParam_Ctor,(UnitDtorFunc)&FLParam_Dtor, 0);
    
    DefineFrameLibExpUnit<FrameLib_Read>("FLRead");
    DefineFrameLibExpUnit<FrameLib_Window>("FLWindow");
    DefineFrameLibExpUnit<FrameLib_Map>("FLMap");

    // Schedulers
    
    DefineFrameLibExpUnit<FrameLib_Interval>("FLInterval");
    
    // IO

    DefineFrameLibExpUnit<FrameLib_Source>("FLSource");
    DefineFrameLibExpUnit<FrameLib_Sink>("FLSink");
    DefineFrameLibExpUnit<FrameLib_Trace>("FLTrace");

    // Generators
    
    DefineFrameLibExpUnit<FrameLib_Random>("FLRand");
    DefineFrameLibExpUnit<FrameLib_Ramp>("FLRamp");
    DefineFrameLibExpUnit<FrameLib_Uniform>("FLUniform");

    // Unary Operators
    
    DefineFrameLibExpUnit<FrameLib_LogicalNot>("FLNot");
    
    DefineFrameLibExpUnit<FrameLib_Acos>("FLAcos");
    DefineFrameLibExpUnit<FrameLib_Asin>("FLAsin");
    DefineFrameLibExpUnit<FrameLib_Atan>("FLAtan");
    DefineFrameLibExpUnit<FrameLib_Cos>("FLCos");
    DefineFrameLibExpUnit<FrameLib_Sin>("FLSin");
    DefineFrameLibExpUnit<FrameLib_Tan>("FLTan");
    DefineFrameLibExpUnit<FrameLib_Acosh>("FLAcosh");
    DefineFrameLibExpUnit<FrameLib_Asinh>("FLAsinh");
    DefineFrameLibExpUnit<FrameLib_Atanh>("FLAtanh");
    DefineFrameLibExpUnit<FrameLib_Cosh>("FLCosh");
    DefineFrameLibExpUnit<FrameLib_Sinh>("FLSinh");
    DefineFrameLibExpUnit<FrameLib_Tanh>("FLTanh");
    
    DefineFrameLibExpUnit<FrameLib_Log>("FLLog");
    DefineFrameLibExpUnit<FrameLib_Log2>("FLLog2");
    DefineFrameLibExpUnit<FrameLib_Log10>("FLLog10");
    DefineFrameLibExpUnit<FrameLib_Exp>("FLExp");
    DefineFrameLibExpUnit<FrameLib_Exp2>("FLExp2");
    
    DefineFrameLibExpUnit<FrameLib_Abs>("FLAbs");
    DefineFrameLibExpUnit<FrameLib_Ceil>("FLCeil");
    DefineFrameLibExpUnit<FrameLib_Floor>("FLFloor");
    DefineFrameLibExpUnit<FrameLib_Round>("FLRound");
    DefineFrameLibExpUnit<FrameLib_Trunc>("FLTrunc");
    
    DefineFrameLibExpUnit<FrameLib_Sqrt>("FLSqrt");
    DefineFrameLibExpUnit<FrameLib_Cbrt>("FLCbrt");
    DefineFrameLibExpUnit<FrameLib_Erf>("FLErf");
    DefineFrameLibExpUnit<FrameLib_Erfc>("FLErfc");
    
    // Binary Operators
    
    DefineFrameLibExpUnit<FrameLib_Multiply>("FLMul");
    DefineFrameLibExpUnit<FrameLib_Plus>("FLAdd");
    DefineFrameLibExpUnit<FrameLib_Minus>("FLSub");
    DefineFrameLibExpUnit<FrameLib_Divide>("FLDiv");
    
    DefineFrameLibExpUnit<FrameLib_Pow>("FLPow");
    DefineFrameLibExpUnit<FrameLib_Atan2>("FLAtan2");
    DefineFrameLibExpUnit<FrameLib_Hypot>("FLHypot");
    DefineFrameLibExpUnit<FrameLib_CopySign>("FLCopySign");
    DefineFrameLibExpUnit<FrameLib_Min>("FLMin");
    DefineFrameLibExpUnit<FrameLib_Max>("FLMax");
    DefineFrameLibExpUnit<FrameLib_Modulo>("FLModulo");
}
