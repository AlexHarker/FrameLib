
#include "FrameLib_KernelSmooth.h"

// Constructor

FrameLib_KernelSmooth::FrameLib_KernelSmooth(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, &sParamInfo, 2, 1), mSmoother(*this)
{
    mParameters.addVariableDoubleArray(kSmooth, "smooth", 0.0, 2, 0);
    mParameters.setMin(0.0);
    
    mParameters.set(serialisedParameters);
}

// Info

std::string FrameLib_KernelSmooth::objectInfo(bool verbose)
{
    return formatInfo("Splits a frame sequentially into smaller frames based on the trigger input.", verbose);
}

std::string FrameLib_KernelSmooth::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return "Kernel Input";
    else
        return "Input Frame";
}

std::string FrameLib_KernelSmooth::outputInfo(unsigned long idx, bool verbose)
{
    return "Smoothed Frame";
}

// Parameter Info

FrameLib_KernelSmooth::ParameterInfo FrameLib_KernelSmooth::sParamInfo;

FrameLib_KernelSmooth::ParameterInfo::ParameterInfo()
{
    add("Set the amount of smoothing." );
}

// Process

void FrameLib_KernelSmooth::process()
{
    unsigned long sizeIn1, sizeIn2, sizeOut;
    const double *input = getInput(0, &sizeIn1);
    const double *kernel = getInput(1, &sizeIn2);

    sizeOut = sizeIn1;
    
    requestOutputSize(0, sizeOut);
    allocateOutputs();

    double *output = getOutput(0, &sizeOut);

    if (output)
    {
        if (!sizeIn2)
        {
            copyVector(output, input, sizeOut);
            return;
        }
        
        Allocator allocator(*this);
        
        unsigned long widthSize;
        double width_lo = 0.0;
        double width_hi = 0.0;
        
        const double *widths = mParameters.getArray(kSmooth, &widthSize);
        
        if (widthSize == 2)
        {
            width_lo = widths[0];
            width_hi = widths[1];
        }
        else if (widthSize == 1)
        {
            width_lo = width_hi = widths[0];
            
        }
        
        mSmoother.smooth(output, input, kernel, sizeIn1, sizeIn2, width_lo, width_hi, Smoother::kSmoothFold);
    }
}
