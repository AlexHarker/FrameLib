
#include "FrameLib_FFT.h"

// Constructor / Destructor

FrameLib_FFT::FrameLib_FFT(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_Processor(context, proxy, &sParamInfo, 1, 2), mProcessor(*this)
{
    mParameters.addInt(kMaxLength, "maxlength", 16384, 0);
    mParameters.setMin(0);
    mParameters.setInstantiation();
    
    mParameters.addBool(kNormalise, "normalise", true, 1);
    mParameters.setInstantiation();
    
    mParameters.addEnum(kMode, "mode", 2);
    mParameters.addEnumItem(kReal, "real");
    mParameters.addEnumItem(kComplex, "complex");
    mParameters.addEnumItem(kFullSpectrum, "fullspectrum");
    mParameters.setInstantiation();
    
    mParameters.set(serialisedParameters);
    
    mProcessor.set_max_fft_size(mParameters.getInt(kMaxLength));
    
    // Store parameters

    mMode = static_cast<Mode>(mParameters.getInt(kMode));
    mNormalise = mParameters.getBool(kNormalise);
    
    // If in complex mode create 2 inlets/outlets

    if (mMode == kComplex)
        setIO(2, 2);
}

// Info

std::string FrameLib_FFT::objectInfo(bool verbose)
{
    return formatInfo("Calculate the real Fast Fourier Transform of an input frame: All FFTs performed will use a power of two size. "
                   "Output frames will be (N / 2) + 1 in length where N is the FFT size. Inputs which are not a power of two are zero-padded to the next power of two. "
                   "Real and imaginary values are output as separate frames.",
                   "Calculate the real Fast Fourier Transform of an input frame.", verbose);
}

std::string FrameLib_FFT::inputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Time Domain Input - will be zero-padded if the length is not a power of two.", "Time Domain Input", verbose);
}

std::string FrameLib_FFT::outputInfo(unsigned long idx, bool verbose)
{
    if (!idx)
        return "Real Values";
    else
        return "Imaginary Values";
}

// Parameter Info

FrameLib_FFT::ParameterInfo FrameLib_FFT::sParamInfo;

FrameLib_FFT::ParameterInfo::ParameterInfo()
{
    add("Sets the maximum input length / FFT size.");
    add("When on the output is normalised so that sine waves produce the same level output regardless of the FFT size.");
    add("Sets the type of input expected / output produced.");
}

// Process

void FrameLib_FFT::process()
{
    FFT_SPLIT_COMPLEX_D spectrum;
    
    // Get Input(s)
    
    unsigned long sizeInR, sizeInI, sizeOut;
    const double *inputR = getInput(0, &sizeInR);
    const double *inputI = nullptr;

    sizeInI = 0;
    
    if (mMode == kComplex)
        inputI = getInput(1, &sizeInI);
    
    // Get FFT size log 2
    
    unsigned long FFTSizeLog2 = static_cast<unsigned long>(mProcessor.calc_fft_size_log2(std::max(sizeInR, sizeInI)));
    unsigned long FFTSize = 1 << FFTSizeLog2;
    sizeOut = mMode == kReal ? (FFTSize >> 1) + 1 : FFTSize;
    
    // Check size
    
    if (FFTSize > mProcessor.max_fft_size() || (!sizeInR && !sizeInI))
        sizeOut = 0;
    
    // Calculate output size
    
    requestOutputSize(0, sizeOut);
    requestOutputSize(1, sizeOut);
    allocateOutputs();
    spectrum.realp = getOutput(0, &sizeOut);
    spectrum.imagp = getOutput(1, &sizeOut);
    
    // Transform
    
    if (sizeOut && spectrum.realp && spectrum.imagp)
    {
        // Take the fft
        
        if (mMode == kComplex)
        {
            copyVector(spectrum.realp, inputR, sizeInR);
            zeroVector(spectrum.realp + sizeInR, sizeOut - sizeInR);
            copyVector(spectrum.imagp, inputI, sizeInI);
            zeroVector(spectrum.imagp + sizeInI, sizeOut - sizeInI);
            
            mProcessor.fft(spectrum, FFTSizeLog2);
        }
        else
        {
            mProcessor.rfft(spectrum, inputR, sizeInR, FFTSizeLog2);
            
            // Move Nyquist Bin
            
            spectrum.realp[FFTSize >> 1] = spectrum.imagp[0];
            spectrum.imagp[FFTSize >> 1] = 0.0;
            spectrum.imagp[0] = 0.0;
            
            // Mirror Spectrum
            
            if (mMode == kFullSpectrum)
            {
                for (unsigned long i = (FFTSize >> 1) + 1; i < sizeOut; i++)
                {
                    spectrum.realp[i] = spectrum.realp[FFTSize - i];
                    spectrum.imagp[i] = -spectrum.imagp[FFTSize - i];
                }
            }
        }
        
        // Scale
        
        double scale = ((mMode == kComplex) ? 1.0 : 0.5) / (mNormalise ? (double) FFTSize : 1.0);
        
        mProcessor.scale_spectrum(spectrum, sizeOut, scale);
    }
}
