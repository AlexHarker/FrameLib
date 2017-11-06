
#include "FrameLib_Dispatch.h"

// Internal Valve Class

// Constructor

FrameLib_Dispatch::Select::Select(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner, long numIns, long num) : FrameLib_Processor(context, owner, NULL, numIns, 1), mNumIns(numIns)
{
    char name[32];
    sprintf(name, "input_%2ld", num + 1);
    mParameters.addInt(kActiveIn, name, 0);
    
    mParameters.set(serialisedParameters);
    
    mActiveIn = floor(mParameters.getInt(kActiveIn));
    
    for (unsigned long i = 0; i < mNumIns; i++)
        setInputMode(i, false, i == mActiveIn, true, kFrameAny);
    
    setOutputType(0, kFrameAny);
    addParameterInput();
}

// Update and Process

void FrameLib_Dispatch::Select::update()
{
    if (mParameters.changed(kActiveIn))
    {
        // FIX - which way to index the inputs?
        
        mActiveIn = floor(mParameters.getValue(kActiveIn));
        
        for (unsigned long i = 0; i < mNumIns; i++)
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

FrameLib_Dispatch::FrameLib_Dispatch(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner)
: FrameLib_Block(kProcessor, context, owner), mParameters(&sParamInfo)
{
    mParameters.addDouble(kNumIns, "num_ins", 2, 0);
    mParameters.setClip(2, 32);
    mParameters.setInstantiation();
    mParameters.addDouble(kNumOuts, "num_outs", 2, 1);
    mParameters.setClip(2, 32);
    mParameters.setInstantiation();
    
    mParameters.set(serialisedParameters);
    
    mNumIns = mParameters.getInt(kNumIns);
    mNumOuts = mParameters.getInt(kNumOuts);
    
    for (long i = 0; i < mNumOuts; i++)
    {
        char name[32];
        sprintf(name, "input_%2ld", i + 1);
        mParameters.addInt(kActiveIn1 + i, name, 0);
    }
              
    mParameters.set(serialisedParameters);
              
    setIO(mNumIns + 1, mNumOuts);

    for (int i = 0; i < mNumOuts; i++)
    {
        mSelects.push_back(new Select(context, serialisedParameters, owner, mNumIns, i));
        for (unsigned long j = 0; j < mNumIns + 1; j++)
            mSelects[i]->setInputAlias(Connection(this, j), j);
        mSelects[i]->setOutputAlias(Connection(this, i), 0);
    }
}

FrameLib_Dispatch::~FrameLib_Dispatch()
{    
    for (std::vector<Select *>::iterator it = mSelects.begin(); it != mSelects.end(); it++)
        delete (*it);
}
    
// Info

std::string FrameLib_Dispatch::objectInfo(bool verbose)
{
    return formatInfo("Dispatches multuple input frame streams dynamically to multuple outputs. WEach output can be independently connected to any one (or none) pf a number of incoming input frame streams, or turned off: The number of inputs and outputs is variable. The selected input for each output is changed with a parameter.",
                      "Dispatches multuple input frame streams dynamically to multuple outputs.", verbose);
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
    add("Sets the number of object outputs.");
    add("Sets the current output (or off if out of range).");
}

// Reset

void FrameLib_Dispatch::reset(double samplingRate, unsigned long maxBlockSize)
{
    for (std::vector<Select *>::iterator it = mSelects.begin(); it != mSelects.end(); it++)
        (*it)->reset(samplingRate, maxBlockSize);
}
