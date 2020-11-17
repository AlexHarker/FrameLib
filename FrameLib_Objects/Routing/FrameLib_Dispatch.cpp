
#include "FrameLib_Dispatch.h"

// Internal Valve Class

// Constructor

FrameLib_Dispatch::Select::Select(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy, long numIns, long num)
: FrameLib_Processor(context, proxy, nullptr, numIns, 1)
, mNumIns(numIns)
{
    const int strBufSize = 32;
    char name[strBufSize];
    snprintf(name, strBufSize, "input_%02ld", num + 1);
    mParameters.addInt(kActiveIn, name, 0);
    
    // FIX - this means that we don't get errors at all... this needs review

    mParameters.setErrorReportingEnabled(false);
    mParameters.set(serialisedParameters);
    
    mActiveIn = truncToInt(mParameters.getValue(kActiveIn) - 1.0);
    
    for (long i = 0; i < mNumIns; i++)
        setInputMode(i, false, i == mActiveIn, true, kFrameAny);
    
    setOutputType(0, kFrameAny);
    addParameterInput();
}

// Update and Process

void FrameLib_Dispatch::Select::update()
{
    if (mParameters.changed(kActiveIn))
    {
        mActiveIn = truncToInt(mParameters.getValue(kActiveIn) - 1.0);
        
        for (long i = 0; i < mNumIns; i++)
            updateTrigger(i, i == mActiveIn);
    }
}

void FrameLib_Dispatch::Select::process()
{
    // Copy active input to output
    
    prepareCopyInputToOutput(mActiveIn, 0);
    allocateOutputs();
    copyInputToOutput(mActiveIn, 0);
}

// Main Class

// Constructor

FrameLib_Dispatch::FrameLib_Dispatch(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Block(kProcessor, context, proxy)
, mParameters(context, proxy, &sParamInfo)
{
    mParameters.addDouble(kNumIns, "num_ins", 2, 0);
    mParameters.setClip(2, 32);
    mParameters.setInstantiation();
    mParameters.addDouble(kNumOuts, "num_outs", 2, 1);
    mParameters.setClip(2, 32);
    mParameters.setInstantiation();
    
    mParameters.setErrorReportingEnabled(false);
    mParameters.set(serialisedParameters);
    mParameters.setErrorReportingEnabled(true);
    
    mNumIns = mParameters.getInt(kNumIns);
    mNumOuts = mParameters.getInt(kNumOuts);
    
    for (long i = 0; i < mNumOuts; i++)
    {
        const int strBufSize = 32;
        char name[strBufSize];
        snprintf(name, strBufSize, "input_%02ld", i + 1);
        mParameters.addInt(kActiveIn1 + i, name, 0);
    }
              
    mParameters.set(serialisedParameters);
              
    setIO(mNumIns + 1, mNumOuts);

    for (int i = 0; i < mNumOuts; i++)
    {
        mSelects.add(new Select(context, serialisedParameters, proxy, mNumIns, i));
        for (long j = 0; j < mNumIns + 1; j++)
            mSelects[i]->setInputAlias(Connection(this, j), j);
        mSelects[i]->setOutputAlias(Connection(this, i), 0);
    }
}
    
// Info

std::string FrameLib_Dispatch::objectInfo(bool verbose)
{
    return formatInfo("Dispatches multiple inputs dynamically to multiple outputs: "
                      "Each output can be independently connected to any one input. "
                      "The number of inputs and outputs is variable. "
                      "The selected input for each output is changed with a parameter.",
                      "Dispatches multiple input frame streams dynamically to multiple outputs.", verbose);
}

std::string FrameLib_Dispatch::inputInfo(unsigned long idx, bool verbose)
{
    if (idx == mNumIns)
        return parameterInputInfo(verbose);
    else
        return formatInfo("Input #", "Input #", idx, verbose);
}

std::string FrameLib_Dispatch::outputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Output #", "Output #", idx, verbose);
}

// Parameter Info

FrameLib_Dispatch::ParameterInfo FrameLib_Dispatch::sParamInfo;

FrameLib_Dispatch::ParameterInfo::ParameterInfo()
{
    add("Sets the number of inputs.");
    add("Sets the number of outputs.");
    for (long i = 0; i < 32; i++)
        add(formatInfo("Sets the current input for output # counting from 1 (off if out of range).", "", i, true));
}

// Reset

void FrameLib_Dispatch::reset(double samplingRate, unsigned long maxBlockSize)
{
    for (auto it = mSelects.begin(); it != mSelects.end(); it++)
        (*it)->reset(samplingRate, maxBlockSize);
}
