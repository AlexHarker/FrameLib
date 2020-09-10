
#include "FrameLib_MakeWindow.h"

// Constructor

FrameLib_MakeWindow::FrameLib_MakeWindow(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, &sParamInfo, 2, 1)
, mGenerator(*this, mParameters)
{
    mParameters.addEnum(kMode, "mode", 0);
    mParameters.addEnumItem(kRequestedLength, "requested");
    mParameters.addEnumItem(kInLength, "input");
    
    mParameters.addInt(kLength, "length", 1, 1);
    mParameters.setMin(0);

    mParameters.addEnum(kUnits, "units", 2);
    mParameters.addEnumItem(kSamples, "samples");
    mParameters.addEnumItem(kMS, "ms");
    mParameters.addEnumItem(kSeconds, "seconds");
    
    mGenerator.addWindowType(3);
    mGenerator.addExponent(4);
    mGenerator.addCompensation();
    mGenerator.addWindowParameters();
    mGenerator.addEndpoints(); // see ramp
    
    mParameters.set(serialisedParameters);
    
    mGenerator.updateParameters();
    
    setParameterInput(1);
}

// Info

std::string FrameLib_MakeWindow::objectInfo(bool verbose)
{
    return formatInfo("Generates a linear ramp across each output frame: "
                      "The scaling parameter is used to set the range of the ramp. "
                      "The length of the output is dependent on the mode. "
                      "Output length may be set by parameter or based on that of the trigger input.",
                      "Generates a linear ramp across each output frame.", verbose);
}

std::string FrameLib_MakeWindow::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return parameterInputInfo(verbose);
    else
        return formatInfo("Trigger Input - triggers output", "Trigger Input", verbose);
}

std::string FrameLib_MakeWindow::outputInfo(unsigned long idx, bool verbose)
{
    return "Output";
}

// Parameter Info

FrameLib_MakeWindow::ParameterInfo FrameLib_MakeWindow::sParamInfo;

FrameLib_MakeWindow::ParameterInfo::ParameterInfo()
{
    add("Controls how the output length is determined: "
        "requested - the output length is set by the length parameter. "
        "input - the output length follows the length of the trigger input.");
    add("Sets the requested output length in the units specified by the units parameter.");
    add("Sets the units for specified output lengths.");
    add("Sets the scaling of the output ramp. "
        "count - scaled in samples (an interger count from zero). "
        "ms - scaled in milliseconds starting at zero. "
        "seconds - scaled in seconds starting at zero. "
        "normalised - output is normalised according to the edges parameter.");
    add("Sets the included edges when the scaling is set to normalised: "
        "both - the ramp is scaled [0-1]. "
        "first - the ramp is scaled [0-1). "
        "last - the ramp is scaled (0-1]. "
        "none - the ramp is scaled (0-1).");
}

// Helpers

unsigned long FrameLib_MakeWindow::getLength()
{
    double time = mParameters.getValue(kLength);
    
    switch (mParameters.getEnum<Units>(kUnits))
    {
        case kSamples:  break;
        case kMS:       time = msToSamples(time);       break;
        case kSeconds:  time = secondsToSamples(time);  break;
    }
    
    return roundToUInt(time);
}

// Process

void FrameLib_MakeWindow::process()
{
    unsigned long sizeIn, sizeOut;
    
    getInput(0, &sizeIn);
    
    sizeOut = mParameters.getEnum<Modes>(kMode) == kInLength ? sizeIn : getLength();
    requestOutputSize(0, sizeOut);
    allocateOutputs();
    
    double *output = getOutput(0, &sizeOut);
    
    if (sizeOut)
    {
        unsigned long N = mGenerator.sizeAdjustForEndpoints(sizeOut);
        unsigned long begin = mGenerator.doFirst() ? 0 : 1;

        mGenerator.generate(output, N, begin, begin + sizeOut, true);
    
        double gain = mGenerator.gainCompensation();
        
        if (gain != 1.0)
        {
            for (unsigned long i = 0; i < sizeOut; i++)
                output[i] *= gain;
        }
    }
}
