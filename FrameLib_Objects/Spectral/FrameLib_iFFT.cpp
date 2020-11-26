
#include "FrameLib_iFFT.h"

// Constructor

FrameLib_iFFT::FrameLib_iFFT(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, &sParamInfo, 2, 1)
, mProcessor(*this)
{
    mParameters.addInt(kMaxLength, "max_length", 16384, 0);
    mParameters.setMin(0);
    mParameters.setInstantiation();
    
    mParameters.addBool(kNormalise, "normalise", true, 1);
    mParameters.setInstantiation();
    
    mParameters.addEnum(kMode, "mode", 2);
    mParameters.addEnumItem(kReal, "real");
    mParameters.addEnumItem(kComplex, "complex");
    mParameters.addEnumItem(kFullSpectrum, "full_spectrum");
    mParameters.setInstantiation();

    mParameters.set(serialisedParameters);
        
    mProcessor.set_max_fft_size(mParameters.getInt(kMaxLength));

    // Store parameters
    
    mMode = mParameters.getEnum<Modes>(kMode);
    mNormalise = mParameters.getBool(kNormalise);
    
    // If in complex mode create 2 inlets/outlets
    
    if (mMode == kComplex)
        setIO(2, 2);
}

// Info

std::string FrameLib_iFFT::objectInfo(bool verbose)
{
    return formatInfo("Calculate the real or complex inverse Fast Fourier Transform of the two inputs (real and imaginary): "
                      "All FFTs use a power of two size, with zero-padding applied at the input(s) if necessary. "
                      "The expected input lengths depend on the mode parameter. "
                      "The mode parameter is used to select either real or complex iFFTs and also the input and output types. "
                      "For complex mode, real and imaginary values are output as separate frames.",
                      "Calculate the real or complex inverse Fast Fourier Transform of the inputs.", verbose);
}

std::string FrameLib_iFFT::inputInfo(unsigned long idx, bool verbose)
{
    if (!idx)
        return formatInfo("Real Input - zero-padded if length doesn't match a power of two FFT size.", "Real Input", verbose);
    else
        return formatInfo("Imaginary Input - zero-padded if length doesn't match a power of two FFT size.", "Imag Input", verbose);
}

std::string FrameLib_iFFT::outputInfo(unsigned long idx, bool verbose)
{
    if (mMode == kComplex)
    {
        if (idx == 0)
            return "Real Output";
        else
            return verbose ? "Imaginary Output" : "Imag Output";
    }
    else
        return "Output";
}

// Parameter Info

FrameLib_iFFT::ParameterInfo FrameLib_iFFT::sParamInfo;

FrameLib_iFFT::ParameterInfo::ParameterInfo()
{
    add("Sets the maximum output length and FFT size.");
    add("Sets normalisation on such that a full-scale real sine wave at the input should have an amplitude of 1.");
    add("Sets the type of output produced and the input expected. "
        "real - real output (power of two length) for input without reflection (length is N / 2 + 1). "
        "complex - complex output (two frames) with the same (power of two) input and output lengths. "
        "fullspectrum - real output for input of the same (power of two) length with ignored redundant reflection.");
}

// Process

void FrameLib_iFFT::process()
{
    FFT_SPLIT_COMPLEX_D spectrum;
    
    unsigned long sizeInR, sizeInI, sizeIn, sizeOut, spectrumSize;
    unsigned long FFTSizeLog2 = 0;
    
    // Get Inputs
    
    const double *inputR = getInput(0, &sizeInR);
    const double *inputI = getInput(1, &sizeInI);
    
    sizeIn = std::max(sizeInR, sizeInI);;
    
    if (sizeIn)
    {
        unsigned long calcSize = mMode == kReal ? (sizeIn - 1) << 1 : sizeIn;
        FFTSizeLog2 = static_cast<unsigned long>(mProcessor.calc_fft_size_log2(calcSize));
        sizeOut = 1 << FFTSizeLog2;
    }
    else
        sizeOut = 0;
    
    // Sanity Check
    
    if (sizeOut > mProcessor.max_fft_size())
    {
        getReporter()(kErrorObject, getProxy(), "requested FFT size (#) larger than maximum FFT size (#)", static_cast<size_t>(sizeOut), mProcessor.max_fft_size());
        sizeOut = 0;
    }
    
    // Calculate output size
    
    requestOutputSize(0, sizeOut);
    if (mMode == kComplex)
        requestOutputSize(1, sizeOut);
    if (!allocateOutputs())
        return;
    
    // Setup output and temporary memory
    
    double *outputR = getOutput(0, &sizeOut);
    double *outputI = nullptr;
    
    auto temp = allocAutoArray<double>(mMode == kReal ? sizeOut : 0);
    
    if (mMode == kComplex)
    {
        outputI = getOutput(1, &sizeOut);
        
        spectrum.realp = outputR;
        spectrum.imagp = outputI;
        
        spectrumSize = sizeOut;
    }
    else
    {
        spectrum.realp = temp;
        spectrum.imagp = spectrum.realp + (sizeOut >> 1);
        
        spectrumSize = sizeOut >> 1;
    }
    
    if (spectrum.realp)
    {
        double scale = mNormalise ? 0.5 : 1.0 / static_cast<double>(1 << FFTSizeLog2);
        
        // Copy Spectrum
        
        unsigned long copySizeR = std::min(sizeInR, spectrumSize);
        unsigned long copySizeI = std::min(sizeInI, spectrumSize);
        
        copyVector(spectrum.realp, inputR, copySizeR);
        zeroVector(spectrum.realp + copySizeR, spectrumSize - copySizeR);
        copyVector(spectrum.imagp, inputI, copySizeI);
        zeroVector(spectrum.imagp + copySizeI, spectrumSize - copySizeI);
        
        if (mMode == kComplex)
        {
            // Convert to time domain and scale

            mProcessor.ifft(spectrum, FFTSizeLog2);
            mProcessor.scale_spectrum(spectrum, sizeOut, scale);
        }
        else
        {
            // Copy Nyquist Bin
            
            if (sizeInR >= ((sizeOut >> 1) + 1))
                spectrum.imagp[0] = inputR[sizeOut >> 1];
        
            // Convert to time domain and scale
        
            mProcessor.rifft(outputR, spectrum, FFTSizeLog2);
            mProcessor.scale_vector(outputR, sizeOut, scale);
        }
    }
}
