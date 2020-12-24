
#include "FrameLib_FIRPhase.h"

// Constructor

FrameLib_FIRPhase::FrameLib_FIRPhase(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, &sParamInfo, 1, 1)
, mProcessor(*this, 0)
{
    mParameters.addInt(kMaxLength, "max_length", 16384, 0);
    mParameters.setMin(0);
    mParameters.setInstantiation();
    
    mParameters.addDouble(kPhase, "phase", 0.0, 1);
    mParameters.setClip(0.0, 1.0);
    
    mParameters.addDouble(kTimeFactor, "time_factor", 1.0, 2);
    mParameters.setMin(1.0);
    
    mParameters.set(serialisedParameters);
    
    addParameterInput();
    
    mProcessor.set_max_fft_size(mParameters.getInt(kMaxLength));
}

// Info

std::string FrameLib_FIRPhase::objectInfo(bool verbose)
{
    return formatInfo("Transform FIR coefficients to preserve the amplitude spectrum but alter the phase. "
                      "The phase can be directly specified using the phase parameter. "
                      "Output frames will be the length of the FFT size used internally for processing. "
                      "Inputs which are not a power of two are zero-padded before processing. "
                      "Additionally the input can be further zero-padded to avoid time aliasing.",
                      "Transform FIR coefficients to preserve the amplitude spectrum but alter the phase.",
                      verbose);
}

std::string FrameLib_FIRPhase::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return parameterInputInfo(verbose);
    
    return formatInfo("Input - zero-padded if the length is not a power of two.", "Input", verbose);
}

std::string FrameLib_FIRPhase::outputInfo(unsigned long idx, bool verbose)
{
    return "Output";
}

// Parameter Info

FrameLib_FIRPhase::ParameterInfo FrameLib_FIRPhase::sParamInfo;

FrameLib_FIRPhase::ParameterInfo::ParameterInfo()
{
    add("Sets the input length and maximum FFT size.");
    add("Sets the phase for the transformed FIR [0 to 1]. "
        "minimum phase can be requested with a value of zero. "
        "linear phase can be requested with value of a half. "
        "maximum phase can be requested with a value of one.");
    add("Sets the factor used to determine the length for zero padding the FIR before processing. "
        "The FIR is padded to at least its original length multiplied by this factor. "
        "Note that this needs to be accounted for when setting the max_length parameter.");
}

// Process

void FrameLib_FIRPhase::process()
{
    // Get Input(s)
    
    unsigned long sizeIn, sizeOut;
    const double *input = getInput(0, &sizeIn);
    double timeFactor = mParameters.getValue(kTimeFactor);
    unsigned long processingLength = static_cast<unsigned long>(round(sizeIn * timeFactor));
    
    // Get FFT size log 2
    
    unsigned long FFTSizeLog2 = static_cast<unsigned long>(mProcessor.calc_fft_size_log2(processingLength));
    unsigned long FFTSize = 1 << FFTSizeLog2;
    sizeOut = FFTSize;
    
    // Check size
    
    if (FFTSize > mProcessor.max_fft_size() || !sizeIn)
    {
        if (FFTSize > mProcessor.max_fft_size())
            getReporter()(kErrorObject, getProxy(), "processing size (#) larger than maximum FFT size (#)", static_cast<size_t>(processingLength), mProcessor.max_fft_size());
        sizeOut = 0;
    }
    
    // Calculate output size
    
    requestOutputSize(0, sizeOut);
    allocateOutputs();
    double *output = getOutput(0, &sizeOut);
   
    // Transform
    
    if (sizeOut)
        mProcessor.change_phase(output, input, sizeIn, mParameters.getValue(kPhase), timeFactor);
}
