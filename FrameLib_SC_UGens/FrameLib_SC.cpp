
#include "SC_PlugIn.h"
#include "../../server/scsynth/SC_UnitDef.h"

#include "FrameLib_Objects.h"

static InterfaceTable *ft;

// declare struct to hold unit generator state

struct FrameLib_SC_UGen : public Unit
{
    FrameLib_Multistream *mObject;
    FrameLib_Proxy *mProxy;
    double **mAudioBuffers;
};


static int kFrameLibFlag = 0x100;

struct SC_FrameLib_Global
{
    struct InitParameters
    {
        InitParameters() : mSerial(nullptr), mInputsSerial(nullptr) {}
        
        ~InitParameters()
        {
            delete mSerial;
            delete mInputsSerial;
        }
        
        // Non-copyable
        
        InitParameters(const InitParameters&) = delete;
        InitParameters& operator=(const InitParameters&) = delete;
        
        InitParameters(InitParameters&& rhs)
        {
            mSerial = rhs.mSerial;
            mInputsSerial = rhs.mInputsSerial;
            rhs.mSerial = nullptr;
            rhs.mInputsSerial = nullptr;
        }

        FrameLib_Parameters::AutoSerial *mSerial;
        FrameLib_Parameters::AutoSerial *mInputsSerial;
    };
    
    SC_FrameLib_Global()
    {
        FrameLib_Global::get(&mGlobal);
    }
    
    ~SC_FrameLib_Global()
    {
        FrameLib_Global::release(&mGlobal);
    }
    
    bool CheckFrameLib(Unit* unit)
    {
        return unit && unit->mUnitDef->mFlags & kFrameLibFlag;
    }
    
    void parseString(World *inWorld, FrameLib_Parameters::AutoSerial& serial, const char *str)
    {
        char *tempStr = (char *) RTAlloc(inWorld, strlen(str) + 1);
        strcpy(tempStr, str);
        
        double array[4096];
        int arraySize;
        
        for (char *strParse = strtok(tempStr, " "); strParse;)
        {
            char *tag = strParse;
            strParse = strtok(nullptr, " ");
            
            for (arraySize = 0; strParse; arraySize++)
            {
                char *endPtr;
                double nextDouble = strtod(strParse, &endPtr);
                
                if (endPtr[0])
                    break;
                
                array[arraySize] = nextDouble;
                strParse = strtok(nullptr, " ");
            }
            
            if (arraySize)
                serial.write(tag, array, arraySize);
            else
            {
                if (strParse)
                {
                    serial.write(tag, strParse);
                    strParse = strtok(nullptr, " ");
                }
            }
        }
        
        RTFree(inWorld, tempStr);
    }
    
    void SetInitParameters(World *inWorld,  int id, const char *params, const char *inputs)
    {
        if (id >= mInitParameters.size())
            mInitParameters.resize(id + 1);
        
        if (mInitParameters[id].mSerial)
            mInitParameters[id].mSerial->clear();
        else
            mInitParameters[id].mSerial = new FrameLib_Parameters::AutoSerial;
        
        parseString(inWorld, *mInitParameters[id].mSerial, params);
        
        if (mInitParameters[id].mInputsSerial)
            mInitParameters[id].mInputsSerial->clear();
        else
            mInitParameters[id].mInputsSerial = new FrameLib_Parameters::AutoSerial;

        parseString(inWorld, *mInitParameters[id].mInputsSerial, inputs);
}
    
    InitParameters *GetInitParameters(FrameLib_SC_UGen* unit)
    {
        if (unit->mNumInputs && unit->mInput[0]->mCalcRate == calc_ScalarRate)
        {
            int paramID = unit->mInput[0]->mScalarValue;
            
            if (paramID >= 0 && paramID < mInitParameters.size())
                return &mInitParameters[paramID];
        }
        
        return nullptr;
    }
    
    FrameLib_Global *getGlobal() { return mGlobal; }
    
    FrameLib_Global *mGlobal;
    
    std::vector<InitParameters> mInitParameters;
};

static SC_FrameLib_Global sGlobal;

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

// declare unit generator functions
static void FLTest_next_a_zero(FrameLib_SC_UGen *unit, int inNumSamples);
static void FLTest_next_a_calc(FrameLib_SC_UGen *unit, int inNumSamples);
static void FLTest_Dtor(FrameLib_SC_UGen* unit);

//////////////////////////////////////////////////////////////////

// Ctor is called to initialize the unit generator.

// A Ctor usually does 3 things.
// 1. set the calculation function.
// 2. initialize the unit generator state variables.
// 3. calculate one sample of output.

template<class T>
void FLTest_Ctor(FrameLib_SC_UGen* unit)
{
    FrameLib_Context context(sGlobal.getGlobal(), unit->mParent);
    
    // FIX - number of streams / fixed inputs
    
    SC_FrameLib_Global::InitParameters *params = sGlobal.GetInitParameters(unit);
    unit->mProxy = GetProxy<T>(unit, static_cast<T *>(nullptr));
    
    int nStreams = 1;
    unit->mObject = new T(context, params ? params->mSerial : nullptr, unit->mProxy, nStreams);
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
        }
        
        if (sGlobal.CheckFrameLib(connect))
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

    if (handlesAudio)
    {
        if (numAudioChans)
        {
            unit->mAudioBuffers = (double **) RTAlloc(unit->mWorld, sizeof(double *) * numAudioChans);
            unit->mAudioBuffers[0] = (double *) RTAlloc(unit->mWorld, sizeof(double) * numAudioChans * unit->mBufLength);
            for (unsigned long i = 1; i <numAudioChans; i++)
                unit->mAudioBuffers[1] = unit->mAudioBuffers[0] + unit->mBufLength * i;
        }
        unit->mCalcFunc = (UnitCalcFunc)&FLTest_next_a_calc;
    }
    else
        unit->mCalcFunc = (UnitCalcFunc)&FLTest_next_a_zero;

    unit->mObject->reset(unit->mRate->mSampleRate, unit->mRate->mBufLength);
    FLTest_next_a_zero(unit, 1);
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

void FLTest_output_zero(FrameLib_SC_UGen *unit, unsigned long output, int numSamples)
{
    float *out = unit->mOutBuf[output];
    
    if (out)
    {
        for (int i = 0; i < numSamples; ++i)
            out[i] = 0.f;
    }
}

void FLTest_next_a_zero(FrameLib_SC_UGen *unit, int inNumSamples)
{
    for (int i = 0; i < unit->mNumOutputs; ++i)
        FLTest_output_zero(unit, i, inNumSamples);
}

void FLTest_next_a_calc(FrameLib_SC_UGen *unit, int inNumSamples)
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
        FLTest_output_zero(unit, i, inNumSamples);
}

//////////////////////////////////////////////////////////////////

template <class T>
void DefineFrameLibUnit(const char *name)
{
    int flags = kFrameLibFlag;
    (*ft->fDefineUnit)(name, sizeof(FrameLib_SC_UGen), (UnitCtorFunc)&FLTest_Ctor<T>,(UnitDtorFunc)&FLTest_Dtor, flags);
}

template <class T>
void DefineFrameLibExpUnit(const char *name)
{
    int flags = kFrameLibFlag;
    (*ft->fDefineUnit)(name, sizeof(FrameLib_SC_UGen), (UnitCtorFunc)&FLTest_Ctor<FrameLib_Expand<T>>,(UnitDtorFunc)&FLTest_Dtor, flags);
}

void ParameterSetup(World *inWorld, void* inUserData, sc_msg_iter *args, void *replyAddr)
{
    double count = args->getd();
    const char *paramMessage = args->gets();
    const char *inputMessage = args->gets();
    
    sGlobal.SetInitParameters(inWorld, count, paramMessage, inputMessage);
}

PluginLoad(FrameLib)
{
    ft = inTable; // store pointer to InterfaceTable
    
    (*ft->fDefinePlugInCmd)("FLParameters", &ParameterSetup, nullptr);
    
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
