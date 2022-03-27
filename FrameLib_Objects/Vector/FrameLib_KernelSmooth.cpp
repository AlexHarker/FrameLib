
#include "FrameLib_KernelSmooth.h"

// Constructor

FrameLib_KernelSmooth::FrameLib_KernelSmooth(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, &sParamInfo, 2, 1)
, mSmoother(*this, 0)
{
    mParameters.addVariableDoubleArray(kSmooth, "smooth", 0.0, 2, 0, 0);
    mParameters.setMin(0.0);
    
    mParameters.addEnum(kScale, "scale", 1);
    mParameters.addEnumItem(kSamples, "samples");
    mParameters.addEnumItem(kNormalised, "normalised");
    
    mParameters.addBool(kSymmetric, "symmetric", true, 2);

    mParameters.addEnum(kEdges, "edges", 3);
    mParameters.addEnumItem(static_cast<unsigned long>(Edges::ZeroPad), "zero");
    mParameters.addEnumItem(static_cast<unsigned long>(Edges::Extend), "extend");
    mParameters.addEnumItem(static_cast<unsigned long>(Edges::Wrap), "wrap");
    mParameters.addEnumItem(static_cast<unsigned long>(Edges::Fold), "fold");
    mParameters.addEnumItem(static_cast<unsigned long>(Edges::Mirror), "mirror");
    
    mParameters.addInt(kMaxFFTSize, "max_fft", 32768);
    mParameters.setInstantiation();
    
    setInputMode(1, false, false, false);

    mParameters.set(serialisedParameters);
    
    mSmoother.set_max_fft_size(mParameters.getInt(kMaxFFTSize));
    
    addParameterInput();
}

// Info

std::string FrameLib_KernelSmooth::objectInfo(bool verbose)
{
    return formatInfo("Smooth an input based on a provided smoothing kernel: "
                      "The kernel is interpolated to the specified width(s) to perform smoothing. "
                      "If the kernel is symmetric only the right-hand side should be provided. "
                      "The edge behaviour can be set as appropriate for the application. "
                      "The output is the same length as the frame at the first input. "
                      "Internally kernels are always stretched across a whole number of samples. "
                      "Kernels with zeroes at the ends are auto detected and stretched appropriately. "
                      "The smoother may use FFT processing for efficiency but it is not required. "
                      "Only the first input triggers output.",
                      "Smooth an input based on a provided smoothing kernel.", verbose);
}

std::string FrameLib_KernelSmooth::inputInfo(unsigned long idx, bool verbose)
{
    switch (idx)
    {
        case 0: return formatInfo("Input - values to be smoother", "Input", verbose);
        case 1: return formatInfo("Kernel Input - does not trigger output", "Kernel Input", verbose);
        default: return parameterInputInfo(verbose);
    }
}

std::string FrameLib_KernelSmooth::outputInfo(unsigned long idx, bool verbose)
{
    return "Output";
}

// Parameter Info

FrameLib_KernelSmooth::ParameterInfo FrameLib_KernelSmooth::sParamInfo;

FrameLib_KernelSmooth::ParameterInfo::ParameterInfo()
{
    add("Sets the width of smoothing as interpreted by the scale parameter. "
        "If only one value is provided it sets the smoothing width directly. "
        "If two values are provided they set the smoothing amounts at either edge. "
        "The smoothing width is then interpolated per sample across the output.");
    add("Sets the scaling for the smoothing parameter: "
        "samples - smoothing is specified in samples. "
        "normalised - smoothing is specified in relation to the width of the input [0-1].");
    add("Indicates that the kernel is symmetric with only the right-hand side provided." );
    add("Sets the edge behaviour for smoothing: "
        "zero - values beyond the edges of the input are read as zeros. "
        "extend - the edge values are extended infinitely in either direction. "
        "wrap - values are read as wrapped or cyclical. "
        "fold - values are folded at edges without repetition of the edge values. "
        "mirror - values are mirrored at edges with the edge values repeated.");
    add("Sets the maximum FFT size available for internal processing. "
        "Note that this needs to be large enough to contain the input, kernel and edges.");
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
        
        Smoother::EdgeMode edges = mParameters.getEnum<Smoother::EdgeMode>(kEdges);
        
        Allocator allocator(*this);
        
        unsigned long widthSize;
        bool symmetric = mParameters.getBool(kSymmetric);
        
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
        
        if (mParameters.getEnum<Scales>(kScale) == kNormalised)
        {
            width_lo /= static_cast<double>(sizeIn2);
            width_hi /= static_cast<double>(sizeIn2);
        }
        
        mSmoother.smooth(output, input, kernel, sizeIn1, sizeIn2, width_lo, width_hi, symmetric, edges);
    }
}
