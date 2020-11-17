
#include "FrameLib_Route.h"

// Internal Valve Class

// Constructor

FrameLib_Route::Valve::Valve(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy, long num)
: FrameLib_Processor(context, proxy, nullptr, 2, 1)
, mValveNumber(num)
{
    mParameters.addInt(kActiveValve, "output", 0);

    // FIX - this means that we don't get errors at all... this needs review
    
    mParameters.setErrorReportingEnabled(false);
    mParameters.set(serialisedParameters);
    
    mActiveValve = truncToInt(mParameters.getValue(kActiveValve) - 1.0);
    
    setInputMode(0, false, mValveNumber == mActiveValve, true, kFrameAny);
    setOutputType(0, kFrameAny);
    
    setParameterInput(1);
}

// Update and Process

void FrameLib_Route::Valve::update()
{
    mActiveValve = truncToInt(mParameters.getValue(kActiveValve) - 1.0);
    updateTrigger(0, mValveNumber == mActiveValve);
}

void FrameLib_Route::Valve::process()
{
    prepareCopyInputToOutput(0, 0);
    allocateOutputs();
    copyInputToOutput(0, 0);
}

// Main Class

// Constructor

FrameLib_Route::FrameLib_Route(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Block(kProcessor, context, proxy)
, mParameters(context, proxy, &sParamInfo)
{
    mParameters.addDouble(kNumOuts, "num_outs", 2, 0);
    mParameters.setClip(2, 32);
    mParameters.setInstantiation();
    
    mParameters.addInt(kActiveOut, "output", 0, 1);
    
    mParameters.set(serialisedParameters);
    
    mNumOuts = mParameters.getInt(kNumOuts);
    
    setIO(2, mNumOuts);

    for (long i = 0; i < mNumOuts; i++)
    {
        mValves.add(new Valve(context, serialisedParameters, proxy, i));
        mValves[i]->setInputAlias(Connection(this, 0), 0);
        mValves[i]->setInputAlias(Connection(this, 1), 1);
        mValves[i]->setOutputAlias(Connection(this, i), 0);
    }
}
    
// Info

std::string FrameLib_Route::objectInfo(bool verbose)
{
    return formatInfo("Routes the input to one of a number of outputs: "
                      "The number of outputs is variable. "
                      "The selected output can be changed with a parameter.",
                      "Routes the input to one of a number of outputs.", verbose);
}

std::string FrameLib_Route::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return parameterInputInfo(verbose);
    else
        return "Input";
}

std::string FrameLib_Route::outputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Output #", "Output #", idx, verbose);
}

// Parameter Info

FrameLib_Route::ParameterInfo FrameLib_Route::sParamInfo;

FrameLib_Route::ParameterInfo::ParameterInfo()
{
    add("Sets the number of outputs.");
    add("Sets the current output counting from 1 (off if out of range).");
}

// Reset

void FrameLib_Route::reset(double samplingRate, unsigned long maxBlockSize)
{
    for (auto it = mValves.begin(); it != mValves.end(); it++)
        (*it)->reset(samplingRate, maxBlockSize);
}
