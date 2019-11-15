
#include "FrameLib_Uniform.h"
#include <algorithm>

// Constructor

FrameLib_Uniform::FrameLib_Uniform(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_Processor(context, proxy, &sParamInfo, 2, 1)
{
    mParameters.addDouble(kValue, "value", 0, 0);
    
    mParameters.addEnum(kMode, "mode", 1);
    mParameters.addEnumItem(kRequestedLength, "requested");
    mParameters.addEnumItem(kInLength, "input");
    
    mParameters.addInt(kLength, "length", 1, 2);
    mParameters.setMin(0);

    mParameters.addEnum(kUnits, "units", 3);
    mParameters.addEnumItem(kSamples, "samples");
    mParameters.addEnumItem(kMS, "ms");
    mParameters.addEnumItem(kSeconds, "seconds");
    
    mParameters.set(serialisedParameters);
        
    setParameterInput(1);
}

// Info

std::string FrameLib_Uniform::objectInfo(bool verbose)
{
    return formatInfo("Generates frames consisting of a single (uniform) value: The size of the output is dependent on the mode. "
                      "The output size may either be set as a parameter, or be set to match that of the triggering input. "
                      "The value is set with a parameter",
                      "Generates frames consisting of a single (uniform) value.", verbose);
}

std::string FrameLib_Uniform::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return parameterInputInfo(verbose);
    else
        return formatInfo("Trigger Frame - triggers generation of output", "Trigger Frame", verbose);
}

std::string FrameLib_Uniform::outputInfo(unsigned long idx, bool verbose)
{
    return "Output";
}

// Parameter Info

FrameLib_Uniform::ParameterInfo FrameLib_Uniform::sParamInfo;

FrameLib_Uniform::ParameterInfo::ParameterInfo()
{
    add("Sets the value to repeat for each output frame.");
    add("Controls how the output length is determined: "
        "requested - the output frame size is set by the length parameter. "
        "input - the output frame size will match the input size.");
    add("Sets the length of the output when the mode is set to requested. Set in the units specified by the units parameter.");
    add("Sets the units for specified output lengths.");
}

// Helpers

unsigned long FrameLib_Uniform::getLength()
{
    double time = mParameters.getValue(kLength);
    
    switch (static_cast<Units>(mParameters.getInt(kUnits)))
    {
        case kSamples:  break;
        case kMS:       time = msToSamples(time);       break;
        case kSeconds:  time = secondsToSamples(time);  break;
    }
    
    return roundToUInt(time);
}

// Process

void FrameLib_Uniform::process()
{
    unsigned long sizeIn, sizeOut;
    
    getInput(0, &sizeIn);
    
    sizeOut = ((Modes) mParameters.getInt(kMode)) == kInLength ? sizeIn : getLength();
    requestOutputSize(0, sizeOut);

    if (allocateOutputs())
    {
        double *output = getOutput(0, &sizeOut);
        std::fill(output, output + sizeOut, mParameters.getValue(kValue));
    }
}
