
#include "FrameLib_CombineTags.h"

// Constructor

FrameLib_CombineTags::FrameLib_CombineTags(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, &sParamInfo)
{
    mParameters.addInt(kNumIns, "num_ins", 2, 0);
    mParameters.setClip(2, maxNumIns);
    mParameters.setInstantiation();
    
    // Read in
    
    mParameters.set(serialisedParameters);
    mNumIns = mParameters.getInt(kNumIns);
 
    // Setup IO
    
    setIO(mNumIns, 1);
    
    for (unsigned long i = 0; i < mNumIns; i++)
        setInputMode(i, false, true, false, kFrameTagged);
    setOutputType(0, kFrameTagged);
}

// Info

std::string FrameLib_CombineTags::objectInfo(bool verbose)
{
    return formatInfo("Combines a number of tagged input frames into a single tagged output frame: "
                      "The number of inputs is set using a parameter. "
                      "All inputs trigger output. "
                      "Tagged Franes arriving asynchronously are sent to the output. "
                      "Tagged frames arriving synchronously are combined into a signel tagged output frame.",
                      "Combines a number of tagged input frames into a single tagged output frame.", verbose);
}

std::string FrameLib_CombineTags::inputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Tagged Input # - to be combined with the other inputs", "Tagged Input #", idx, verbose);
}

std::string FrameLib_CombineTags::outputInfo(unsigned long idx, bool verbose)
{
    return "Output";
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
    requestOutputSize(0, 0);
    
    for (unsigned long i = 0; i < mNumIns; i++)
        requestAddedOutputSize(0, FrameLib_Parameters::Serial::calcSize(getInput(i)));
    
    if (allocateOutputs())
    {
        FrameLib_Parameters::Serial *output = getOutput(0);
        
        for (unsigned long i = 0; i < mNumIns; i++)
            output->write(getInput(i));
    }
}
