
#include "FrameLib_MultiSelect.h"

// Internal Valve Class

// Constructor

FrameLib_MultiSelect::Select::Select(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner, long numIns, long num) : FrameLib_Processor(context, owner, NULL, 1, 1), mNumIns(numIns)
{
    char name[32];
    sprintf(name, "input_%2ld", num);
    mParameters.addInt(kActiveIn, name, 0);
    
    mParameters.set(serialisedParameters);
    
    mActiveIn = floor(mParameters.getInt(kActiveIn));
    
    for (unsigned long i = 0; i < mNumIns; i++)
        setInputMode(i, false, i == mActiveIn, true, kFrameAny);
    
    setOutputMode(0, kFrameAny);
    addParameterInput();
}

// Update and Process

void FrameLib_MultiSelect::Select::update()
{
    if (mParameters.changed(kActiveIn))
    {
        // FIX - which way to index the inputs?
        
        mActiveIn = floor(mParameters.getValue(kActiveIn));
        
        for (unsigned long i = 0; i < mNumIns; i++)
            updateTrigger(i, i == mActiveIn);
    }
}

void FrameLib_MultiSelect::Select::process()
{
    // Copy active input to output
    
    prepareCopyInputToOutput(mActiveIn, 0);
    allocateOutputs();
    copyInputToOutput(mActiveIn, 0);

}

// Main Class

// Constructor

FrameLib_MultiSelect::FrameLib_MultiSelect(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner)
: FrameLib_Block(kProcessor, context, owner), mParameters(&sParamInfo)
{
    mParameters.addDouble(kNumIns, "num_ins", 2, 0);
    mParameters.setClip(2, 32);
    mParameters.setInstantiation();
    mParameters.addDouble(kNumOuts, "num_outs", 2, 1);
    mParameters.setClip(2, 32);
    mParameters.setInstantiation();
    
    mParameters.set(serialisedParameters);
    
    mNumOuts = mParameters.getInt(kNumIns);
    mNumOuts = mParameters.getInt(kNumOuts);
    
    for (long i = 0; i < mNumOuts; i++)
    {
        char name[32];
        sprintf(name, "input_%2ld", i);
        mParameters.addInt(kActiveIn1 + i, name, 0);
    }
              
    mParameters.set(serialisedParameters);
              
    setIO(kNumIns + 1, kNumOuts);

    for (int i = 0; i < mNumOuts; i++)
    {
        mSelects.push_back(new Select(context, serialisedParameters, owner, mNumIns, i));
        for (unsigned long j = 0; j < mNumIns + 1; j++)
            mSelects[i]->setInputAlias(Connection(this, j), j);
        mSelects[i]->setOutputAlias(Connection(this, i), 0);
    }
}

FrameLib_MultiSelect::~FrameLib_MultiSelect()
{    
    for (std::vector<Select *>::iterator it = mSelects.begin(); it != mSelects.end(); it++)
        delete (*it);
}
    
// Info

std::string FrameLib_MultiSelect::objectInfo(bool verbose)
{
    return formatInfo("Routes input frames to one of a number of outputs: The number of outputs is variable. The selected output can be changed with a parameter.",
                      "Routes input frames to one of a number of outputs.", verbose);
}

std::string FrameLib_MultiSelect::inputInfo(unsigned long idx, bool verbose)
{
    if (idx == mNumOuts)
        return formatInfo("Parameter Update - tagged input updates parameters", "Parameter Update", verbose);
    else
        return "Input Frames";
}

std::string FrameLib_MultiSelect::outputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Output #", "Output #", idx, verbose);
}

// Parameter Info

FrameLib_MultiSelect::ParameterInfo FrameLib_MultiSelect::sParamInfo;

FrameLib_MultiSelect::ParameterInfo::ParameterInfo()
{
    add("Sets the number of object outputs.");
    add("Sets the current output (or off if out of range).");
}

// Reset

void FrameLib_MultiSelect::reset(double samplingRate, unsigned long maxBlockSize)
{
    for (std::vector<Select *>::iterator it = mSelects.begin(); it != mSelects.end(); it++)
        (*it)->reset(samplingRate, maxBlockSize);
}
