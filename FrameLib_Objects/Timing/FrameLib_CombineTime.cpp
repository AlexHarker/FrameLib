
#include "FrameLib_CombineTime.h"

// Constructor

FrameLib_CombineTime::FrameLib_CombineTime(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_Processor(context, proxy, &sParamInfo, 1, 1)
{
    mParameters.addInt(kNumIns, "num_ins", 2, 0);
    mParameters.setClip(2, maxNumIns);

    mParameters.setInstantiation();
    
    mParameters.set(serialisedParameters);
    
    mNumIns = mParameters.getInt(kNumIns);
    setIO(mNumIns, 1);
    
    for (long i=0; i < mNumIns; i++)
        setInputMode(i, false, true, false, kFrameAny);
    setOutputType(0, kFrameAny);
}

// Info

std::string FrameLib_CombineTime::objectInfo(bool verbose)
{
    return formatInfo("Combines multiple trigger frame inputs into a single output.",
                   "Combines trigger frames.", verbose);
}

std::string FrameLib_CombineTime::inputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Trigger Input - input frames generate output", "Trigger Input", verbose);
}

std::string FrameLib_CombineTime::outputInfo(unsigned long idx, bool verbose)
{
    return "Output Values";
}

// Parameter Info

FrameLib_CombineTime::ParameterInfo FrameLib_CombineTime::sParamInfo;

FrameLib_CombineTime::ParameterInfo::ParameterInfo()
{
    add("Sets the time units used for output.");
}

// Process

void FrameLib_CombineTime::process()
{
}
