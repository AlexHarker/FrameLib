
#include "FrameLib_Chop.h"

// Constructor

FrameLib_Chop::FrameLib_Chop(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy): FrameLib_Processor(context, proxy, &sParamInfo)
{
    mParameters.addInt(kNumOuts, "num_outs", 1, 0);
    mParameters.setClip(2, 32);
    mParameters.setInstantiation();
    
    mParameters.addDouble(kSize, "size", 1.0, 1);
    mParameters.setMin(0.0);
    
    mParameters.addEnum(kUnits, "units", 2);
    mParameters.addEnumItem(kSamples, "samples");
    mParameters.addEnumItem(kRatio, "ratios");
    
    mParameters.set(serialisedParameters);
    
    mNumOuts = mParameters.getInt(kNumOuts);
    
    setIO(2, mNumOuts);
    
    setParameterInput(1);
}

// Info

std::string FrameLib_Chop::objectInfo(bool verbose)
{
    return formatInfo("Chops input frames into equally sized output frames: "
                   "The size can be set in samples, or as a ratio of the length of the input frame.",
                   "Chops input frames into equally sized output frames.", verbose);
}

std::string FrameLib_Chop::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return parameterInputInfo(verbose);
    else
        return "Input";
}

std::string FrameLib_Chop::outputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Output #", "Output #", idx, verbose);
}

// Parameter Info

FrameLib_Chop::ParameterInfo FrameLib_Chop::sParamInfo;

FrameLib_Chop::ParameterInfo::ParameterInfo()
{
    add("Sets the number of outputs.");
    add("Sets the size of the output frames.");
    add("Sets units for the chop size (samples or ratio of the input length [0-1]).");
}

// Process

void FrameLib_Chop::process()
{
    // Get Input
    
    unsigned long sizeIn, sizeOut, sizeCalc, chop, i;
    const double *input = getInput(0, &sizeIn);
    Units units = (Units) mParameters.getInt(kUnits);
    
    // Calculate chop sizes
    
    if (units == kSamples)
        chop = mParameters.getInt(kSize);
    else
        chop = roundToUInt(mParameters.getValue(kSize) * sizeIn);
    
    for (i = 0, sizeCalc = sizeIn; i < mNumOuts; i++)
    {
        sizeOut = (chop <= sizeCalc) ? chop : sizeCalc;
        sizeCalc -= sizeOut;
        requestOutputSize(i, sizeOut);
    }
    
    allocateOutputs();
    
    for (i = 0, sizeCalc = 0; i < mNumOuts; i++)
    {
        // Copy to output
        
        double *output = getOutput(i, &sizeOut);
        
        copyVector(output, input + sizeCalc, sizeOut);
        sizeCalc += sizeOut;
    }
}
