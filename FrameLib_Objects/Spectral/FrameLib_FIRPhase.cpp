
#include "FrameLib_FIRPhase.h"

// Constructor / Destructor

FrameLib_FIRPhase::FrameLib_FIRPhase(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_Processor(context, proxy, &sParamInfo, 1, 1), mProcessor(*this)
{
    mParameters.addInt(kMaxLength, "maxlength", 16384, 0);
    mParameters.setMin(0);
    mParameters.setInstantiation();
    mParameters.addDouble(kPhase, "phase", 0.0, 1);
    mParameters.setClip(0.0, 1.0);
    
    mParameters.set(serialisedParameters);
    
    mProcessor.set_max_fft_size(mParameters.getInt(kMaxLength));
}

// Info

std::string FrameLib_FIRPhase::objectInfo(bool verbose)
{
    return formatInfo("Calculate the real Fast Fourier Transform of an input frame: All FFTs performed will use a power of two size. "
                   "Output frames will be (N / 2) + 1 in length where N is the FFT size. Inputs which are not a power of two are zero-padded to the next power of two. "
                   "Real and imaginary values are output as separate frames.",
                   "Calculate the real Fast Fourier Transform of an input frame.", verbose);
}

std::string FrameLib_FIRPhase::inputInfo(unsigned long idx, bool verbose)
{
    return "IR Time Domain Input";
}

std::string FrameLib_FIRPhase::outputInfo(unsigned long idx, bool verbose)
{
    return "IR Time Domain Output";
}

// Parameter Info

FrameLib_FIRPhase::ParameterInfo FrameLib_FIRPhase::sParamInfo;

FrameLib_FIRPhase::ParameterInfo::ParameterInfo()
{
    add("Sets the maximum input length / FFT size.");
    add("When on the output is normalised so that sine waves produce the same level output regardless of the FFT size.");
    add("Sets the type of input expected / output produced.");
}

// Process

void FrameLib_FIRPhase::process()
{
    // Get Input(s)
    
    unsigned long sizeIn, sizeOut;
    const double *input = getInput(0, &sizeIn);
    
    // Get FFT size log 2
    
    unsigned long FFTSizeLog2 = static_cast<unsigned long>(mProcessor.calc_fft_size_log2(sizeIn));
    unsigned long FFTSize = 1 << FFTSizeLog2;
    sizeOut = FFTSize;
    
    // Check size
    
    if (FFTSize > mProcessor.max_fft_size() || !sizeIn)
        sizeOut = 0;
    
    // Calculate output size
    
    requestOutputSize(0, sizeOut);
    allocateOutputs();
    double *output = getOutput(0, &sizeOut);
   
    // Transform
    
    if (sizeOut)
        mProcessor.change_phase(output, input, sizeIn, mParameters.getValue(kPhase));
}
