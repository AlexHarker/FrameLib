
#include "SC_PlugIn.h"
#include "../../server/scsynth/SC_UnitDef.h"
#include <iostream>

#include "FrameLib_Multistream.h"

#include "FrameLib_Binary_Objects.h"
#include "FrameLib_Interval.h"
#include "FrameLib_Source.h"
#include "FrameLib_Sink.h"


static InterfaceTable *ft;

// declare struct to hold unit generator state

struct FrameLib_SC_UGen : public Unit
{
    FrameLib_Multistream *mObject;
    double **mAudioBuffers;
};


static int kFrameLibFlag = 0x100;

struct SC_FrameLib_Global
{
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
    
    FrameLib_Global *getGlobal() { return mGlobal; }
    
    FrameLib_Global *mGlobal;
};

static SC_FrameLib_Global sGlobal;


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
    
    // FIX - no parameters / proxy / number of streams / fixed inputs
    
    int nStreams = 1;
    unit->mObject = new T(context, nullptr, nullptr, nStreams);
    unit->mAudioBuffers = nullptr;
    bool handlesAudio = T::handlesAudio();
    
    unsigned long numUnitIns = unit->mNumInputs;
    unsigned long numAudioIns = unit->mObject->getNumAudioIns();
    unsigned long numFrameIns = unit->mObject->getNumIns();
    unsigned long numFrameConnections = std::min(numUnitIns >= numAudioIns ? numUnitIns - numAudioIns: 0, numFrameIns);
    unsigned long numAudioChans = unit->mObject->getNumAudioChans();
    
    for (unsigned long i = 0; i < numFrameConnections; i++)
    {
        Wire *wire = unit->mInput[i + numAudioIns];
        Unit *connect = wire->mFromUnit;
        
        if (sGlobal.CheckFrameLib(connect))
        {
            // FIX - outputs don't work like this...
            
            FrameLib_SC_UGen *connectable = reinterpret_cast<FrameLib_SC_UGen *>(connect);
            unsigned long o = connectable->mObject->getNumAudioOuts();
            
            for (; o < connect->mNumOutputs; o++)
            {
                if (connect->mOutput[o] == wire)
                    break;
            }
            
            if (o < connect->mNumOutputs)
            {
                unit->mObject->addConnection(FrameLib_Multistream::Connection(connectable->mObject, o), i);
                std::cout << "Connection Input " << i << " Output " << o << " " << connect;
            }
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
        unsigned long validInputs = std::min(numAudioChannels, static_cast<unsigned long>(unit->mNumInputs));
        
        for (unsigned long i = 0; i < validInputs; ++i)
        {
            float *in = unit->mInBuf[i];
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
    int flags = kUnitDef_CantAliasInputsToOutputs | kFrameLibFlag;
    (*ft->fDefineUnit)(name, sizeof(FrameLib_SC_UGen), (UnitCtorFunc)&FLTest_Ctor<T>,(UnitDtorFunc)&FLTest_Dtor, flags);
}

PluginLoad(FLTest)
{
    ft = inTable; // store pointer to InterfaceTable
    
    DefineFrameLibUnit<FrameLib_Expand<FrameLib_Plus>>("FLTest");
    DefineFrameLibUnit<FrameLib_Expand<FrameLib_Multiply>>("FLMultiply");
    DefineFrameLibUnit<FrameLib_Expand<FrameLib_Interval>>("FLInterval");
    DefineFrameLibUnit<FrameLib_Expand<FrameLib_Source>>("FLSource");
    DefineFrameLibUnit<FrameLib_Expand<FrameLib_Sink>>("FLSink");
}
