
#include "FrameLib_IRPhase.h"
#include "FrameLib_Spectral_Functions.h"

// Constructor / Destructor

FrameLib_IRPhase::FrameLib_IRPhase(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_Processor(context, proxy, &sParamInfo, 1, 2)
{
    mParameters.addInt(kMaxLength, "maxlength", 16384, 0);
    mParameters.setMin(0);
    mParameters.setInstantiation();
    mParameters.addDouble(kPhase, "phase", 0.0, 1);
    mParameters.setClip(0.0, 1.0);
    
    mParameters.set(serialisedParameters);
    
    unsigned long maxFFTSizeLog2 = ilog2(mParameters.getInt(kMaxLength));
    
    hisstools_create_setup(&mFFTSetup, maxFFTSizeLog2);
    
    // Store parameters

    mMaxFFTSize = 1 << maxFFTSizeLog2;
}

FrameLib_IRPhase::~FrameLib_IRPhase()
{
    hisstools_destroy_setup(mFFTSetup);
}

// Info

std::string FrameLib_IRPhase::objectInfo(bool verbose)
{
    return formatInfo("Calculate the real Fast Fourier Transform of an input frame: All FFTs performed will use a power of two size. "
                   "Output frames will be (N / 2) + 1 in length where N is the FFT size. Inputs which are not a power of two are zero-padded to the next power of two. "
                   "Real and imaginary values are output as separate frames.",
                   "Calculate the real Fast Fourier Transform of an input frame.", verbose);
}

std::string FrameLib_IRPhase::inputInfo(unsigned long idx, bool verbose)
{
    return "IR Time Domain Input";
}

std::string FrameLib_IRPhase::outputInfo(unsigned long idx, bool verbose)
{
    return "IR Time Domain Output";
}

// Parameter Info

FrameLib_IRPhase::ParameterInfo FrameLib_IRPhase::sParamInfo;

FrameLib_IRPhase::ParameterInfo::ParameterInfo()
{
    add("Sets the maximum input length / FFT size.");
    add("When on the output is normalised so that sine waves produce the same level output regardless of the FFT size.");
    add("Sets the type of input expected / output produced.");
}

// Process

void FrameLib_IRPhase::process()
{
    FFT_SPLIT_COMPLEX_D spectrum;
    
    // Get Input(s)
    
    unsigned long sizeIn, sizeOut;
    const double *input = getInput(0, &sizeIn);
    
    // Get FFT size log 2
    
    unsigned long FFTSizelog2 = ilog2(sizeIn);
    unsigned long FFTSize = 1 << FFTSizelog2;
    sizeOut = FFTSize;
    
    // Check size
    
    if (FFTSize > mMaxFFTSize || !sizeIn)
        sizeOut = 0;
    
    // Calculate output size
    
    requestOutputSize(0, sizeOut);
    allocateOutputs();
    double *output = getOutput(0, &sizeOut);
    
    spectrum.realp = alloc<double>(sizeOut);
    spectrum.imagp = spectrum.realp + (sizeOut >>1);
    
    // Transform
    
    if (sizeOut && spectrum.realp)
    {
        // Take the fft
        
        hisstools_rfft(mFFTSetup, input, &spectrum, sizeIn, FFTSizelog2);
        ir_phase(mFFTSetup, &spectrum, &spectrum, FFTSize, mParameters.getValue(kPhase));
        hisstools_rifft(mFFTSetup, &spectrum, output, FFTSizelog2);
        
        // Scale
        
        const double scale = 0.5 / (double) FFTSize;
        
        for (unsigned long i = 0; i < sizeOut; i++)
            output[i] *= scale;
    }
    
    dealloc(spectrum.realp);
}
