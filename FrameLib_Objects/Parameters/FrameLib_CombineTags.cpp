
#include "FrameLib_CombineTags.h"

// Constructor

FrameLib_CombineTags::FrameLib_CombineTags(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, owner, &sParamInfo)
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
    setOutputType(0, kFrameTagged);
}

// Info

std::string FrameLib_CombineTags::objectInfo(bool verbose)
{
    return formatInfo("Combines a number of tagged input frames into a single tagged output frame: "
                   "The number of inputs is set using a parameter.",
                   "Combines a number of tagged input frames into a single tagged output frame.", verbose);
}

std::string FrameLib_CombineTags::inputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Parameter Input # - takes tagged input", "Parameter Input #", idx, verbose);
}

std::string FrameLib_CombineTags::outputInfo(unsigned long idx, bool verbose)
{
    return "Tagged Output Frames";
}

// Parameter Info

FrameLib_CombineTags::ParameterInfo FrameLib_CombineTags::sParamInfo;

FrameLib_CombineTags::ParameterInfo::ParameterInfo()
{
    add("Sets the number of inputs.");
}

// Process

void FrameLib_CombineTags::process()
{
    unsigned long sizeOut;
    
    sizeOut = 0;
    
    for (unsigned long i = 0; i < mNumIns; i++)
        sizeOut += FrameLib_Parameters::Serial::calcSize(getInput(i));
    
    requestOutputSize(0, sizeOut);
    allocateOutputs();
    
    FrameLib_Parameters::Serial *output = getOutput(0);
    
    if (output)
    {
        for (unsigned long i = 0; i < mNumIns; i++)
            output->write(getInput(i));
    }
}
