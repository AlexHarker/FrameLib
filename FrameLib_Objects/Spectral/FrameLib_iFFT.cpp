
#include "FrameLib_iFFT.h"
#include "FrameLib_Spectral_Functions.h"

// Constructor / Destructor

FrameLib_iFFT::FrameLib_iFFT(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_Processor(context, proxy, &sParamInfo, 2, 1)
{
    mParameters.addInt(kMaxLength, "maxlength", 16384, 0);
    mParameters.setMin(0);
    mParameters.setInstantiation();
    mParameters.addBool(kNormalise, "normalise", false, 1);
    mParameters.setInstantiation();
    mParameters.addEnum(kMode, "mode", 2);
    mParameters.addEnumItem(kReal, "real");
    mParameters.addEnumItem(kComplex, "complex");
    mParameters.addEnumItem(kFullSpectrum, "fullspectrum");
    mParameters.setInstantiation();

    mParameters.set(serialisedParameters);
    
    unsigned long maxFFTSizeLog2 = ilog2(mParameters.getInt(kMaxLength));
    
    hisstools_create_setup(&mFFTSetup, maxFFTSizeLog2);
    
    // Store parameters
    
    mMaxFFTSize = 1 << maxFFTSizeLog2;
    mMode = static_cast<Mode>(mParameters.getInt(kMode));
    mNormalise = mParameters.getBool(kNormalise);
    
    // If in complex mode create 2 inlets/outlets
    
    if (mMode == kComplex)
        setIO(2, 2);
}

FrameLib_iFFT::~FrameLib_iFFT()
{
    hisstools_destroy_setup(mFFTSetup);
}

// Info

std::string FrameLib_iFFT::objectInfo(bool verbose)
{
    return formatInfo("Calculate the inverse real Fast Fourier Transform of two input frames (comprising the real and imaginary values): All FFTs performed will use a power of two size. "
                   "Output frames will be N in length where N is the FFT size. Inputs are expected to match in length with a length of (N / 2) + 1.",
                   "Calculate the inverse real Fast Fourier Transform of two input frames (comprising the real and imaginary values).", verbose);
}

std::string FrameLib_iFFT::inputInfo(unsigned long idx, bool verbose)
{
    if (!idx)
        return formatInfo("Frequency Domain Real Values - inputs should match in size and be (N / 2) + 1 in length.", "Freq Domain Real Values", verbose);
    else
        return formatInfo("Frequency Domain Imaginary Values - inputs should match in size and be (N / 2) + 1 in length.", "Freq Domain Imag Values", verbose);
}

std::string FrameLib_iFFT::outputInfo(unsigned long idx, bool verbose)
{
    return "Time Domain Output";
}

// Parameter Info

FrameLib_iFFT::ParameterInfo FrameLib_iFFT::sParamInfo;

FrameLib_iFFT::ParameterInfo::ParameterInfo()
{
    add("Sets the maximum output length / FFT size.");
    add("When on the input is expected to be normalised.");
    add("Sets the type of input expected / output produced.");
}

// Process

void FrameLib_iFFT::process()
{
    FFT_SPLIT_COMPLEX_D spectrum;
    
    unsigned long sizeInR, sizeInI, sizeIn, sizeOut, spectrumSize;
    unsigned long FFTSizelog2 = 0;
    
    // Get Inputs
    
    const double *inputR = getInput(0, &sizeInR);
    const double *inputI = getInput(1, &sizeInI);
    
    sizeIn = std::max(sizeInR, sizeInI);;
    
    if (sizeIn)
    {
        unsigned long calcSize = mMode == kReal ? (sizeIn - 1) << 1 : sizeIn;
        FFTSizelog2 = ilog2(calcSize);
        sizeOut = 1 << FFTSizelog2;
    }
    else
        sizeOut = 0;
    
    // Sanity Check
    
    if (sizeOut > mMaxFFTSize)
        sizeOut = 0;
    
    // Calculate output size
    
    requestOutputSize(0, sizeOut);
    if (mMode == kComplex)
        requestOutputSize(1, sizeOut);
    allocateOutputs();
    
    // Setup output and temporary memory
    
    double *outputR = getOutput(0, &sizeOut);
    double *outputI = nullptr;
    
    if (mMode == kComplex && sizeOut)
    {
        outputI = getOutput(1, &sizeOut);
        
        spectrum.realp = outputR;
        spectrum.imagp = outputI;
        
        spectrumSize = sizeOut;
    }
    else
    {
        spectrum.realp = alloc<double>(sizeOut ? sizeOut * sizeof(double) : 0);
        spectrum.imagp = spectrum.realp + (sizeOut >> 1);
        
        spectrumSize = sizeOut >> 1;
    }
    
    if (sizeOut && spectrum.realp)
    {
        double scale = mNormalise ? 1.0 : 1.0 / static_cast<double>(1 << FFTSizelog2);
        
        // Copy Spectrum
        
        unsigned long copySizeR = std::min(sizeInR, spectrumSize);
        unsigned long copySizeI = std::min(sizeInI, spectrumSize);
        
        copyVector(spectrum.realp, inputR, copySizeR);
        zeroVector(spectrum.realp + copySizeR, spectrumSize - copySizeR);
        copyVector(spectrum.imagp, inputI, copySizeI);
        zeroVector(spectrum.imagp + copySizeI, spectrumSize - copySizeI);
        
        if (mMode == kComplex)
        {
            // Convert to time domain

            hisstools_ifft(mFFTSetup, &spectrum, FFTSizelog2);

            // Scale
            
            for (unsigned long i = 0; i < sizeOut; i++)
            {
                spectrum.realp[i] *= scale;
                spectrum.imagp[i] *= scale;
            }
        }
        else
        {
            // Copy Nyquist Bin
            
            if (sizeInR >= ((sizeOut >> 1) + 1))
                spectrum.imagp[0] = inputR[sizeOut >> 1];
        
            // Convert to time domain
        
            hisstools_rifft(mFFTSetup, &spectrum, outputR, FFTSizelog2);
        
            // Scale
        
            for (unsigned long i = 0; i < sizeOut; i++)
                outputR[i] *= scale;
        
            dealloc(spectrum.realp);
        }
    }
}
