
#include "FrameLib_CombineParam.h"

// Constructor

FrameLib_CombineParam::FrameLib_CombineParam(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, owner, &sParamInfo)
{
    mParameters.addInt(kNumIns, "num_ins", 2, 0);
    mParameters.setClip(2, maxNumIns);
    mParameters.setInstantiation();
    
    // Read in
    
    mParameters.set(serialisedParameters);
    mNumIns = mParameters.getInt(kNumIns);
 
    // Setup IO
    
    setIO(mNumIns, 1);
    
    for (int i = 0; i < mNumIns; i++)
        setInputMode(i, false, true, false, kFrameTagged);
    setOutputMode(0, kFrameTagged);
}

// Info

std::string FrameLib_CombineParam::objectInfo(bool verbose)
{
    return formatInfo("Separates vectors from a tagged frame according to parameter names: "
                   "A variable number of outputs is available, each of which deal will a specific parameter name.",
                   "Separates vectors from a tagged frame according to parameter names.", verbose);
}

std::string FrameLib_CombineParam::inputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Parameter Input # - takes tagged input", "Parameter Input #", idx, verbose);
}

std::string FrameLib_CombineParam::outputInfo(unsigned long idx, bool verbose)
{
    return "Tagged Output Frames";
}

// Parameter Info

FrameLib_CombineParam::ParameterInfo FrameLib_CombineParam::sParamInfo;

FrameLib_CombineParam::ParameterInfo::ParameterInfo()
{
    add("Sets the number of object inputs");
}

// Process

void FrameLib_CombineParam::process()
{
    unsigned long sizeOut;
    
    sizeOut = 0;
    
    for (unsigned long i = 0; i < mNumIns; i++)
        sizeOut += FrameLib_Parameters::Serial::calcSize(getInput(i));
    
    requestOutputSize(0, sizeOut);
    allocateOutputs();
    
    FrameLib_Parameters::Serial *output = getOutput(0);
    
    for (unsigned long i = 0; i < mNumIns; i++)
        output->write(getInput(i));    
}
