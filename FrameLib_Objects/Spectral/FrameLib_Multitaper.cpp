
#include "FrameLib_Multitaper.h"

// See - Riedel, K. S., & Sidorenko, A. (1995). Minimum bias multiple taper spectral estimation. IEEE Transactions on Signal Processing, 43(1), 188-195.

// Constructor

FrameLib_Multitaper::FrameLib_Multitaper(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, &sParamInfo, 1, 1)
, mProcessor(*this)
{
    mParameters.addInt(kMaxLength, "maxlength", 16384, 0);
    mParameters.setMin(0);
    mParameters.setInstantiation();
    
    mParameters.addInt(kNumTapers, "numtapers", 3, 1);
    mParameters.setMin(1);
    
    mParameters.addBool(kNormalise, "normalise", true, 2);
 
    mParameters.addEnum(kOutput, "output", 3);
    mParameters.addEnumItem(kPower, "power");
    mParameters.addEnumItem(kAmplitude, "amplitude", true);
    
    mParameters.addEnum(kWeighting, "weighting", 4);
    mParameters.addEnumItem(kUniform, "uniform");
    mParameters.addEnumItem(kLinear, "linear");
    mParameters.addEnumItem(kParabolic, "parabolic", true);
    
    mParameters.set(serialisedParameters);
    
    mProcessor.set_max_fft_size(mParameters.getInt(kMaxLength) * 2);
    
    addParameterInput();
}

// Helpers

void FrameLib_Multitaper::getWrapped(double &rOut, double&iOut, double *real, double *imag, long size, long pos)
{
    pos &= (size - 1);
    
    if (pos <= (size >> 1))
    {
        rOut = real[pos];
        iOut = imag[pos];
    }
    else
    {
        pos = size - pos;
        rOut = real[pos];
        iOut = -imag[pos];
    }
}

// Info

std::string FrameLib_Multitaper::objectInfo(bool verbose)
{
    return formatInfo("Calculates the multitaper power spectrum of a real input using the cosine tapers: All FFTs performed will use a power of two size. "
                   "Output frames will be (N / 2) + 1 in length where N is the FFT size. Inputs which are not a power of two are zero-padded to the next power of two.",
                   "Calculates the multitaper power spectrum of a real input using the cosinre tapers.", verbose);
}

std::string FrameLib_Multitaper::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return formatInfo("Frequency Domain Real Values - inputs should match in size and be (N / 2) + 1 in length.", "Freq Domain Real Values", verbose);
    else
        return formatInfo("Frequency Domain Imaginary Values - inputs should match in size and be (N / 2) + 1 in length.", "Freq Domain Imag Values", verbose);
}

std::string FrameLib_Multitaper::outputInfo(unsigned long idx, bool verbose)
{
    return "Time Domain Output";
}

// Parameter Info

FrameLib_Multitaper::ParameterInfo FrameLib_Multitaper::sParamInfo;

FrameLib_Multitaper::ParameterInfo::ParameterInfo()
{
    add("Sets the maximum input length / FFT size.");
    add("Sets the number of tapers to use.");
}

double FrameLib_Multitaper::getWeight(unsigned long k, unsigned long nTapers)
{
    switch (mParameters.getEnum<Weightings>(kWeighting))
    {
        case kUniform:
            return 1.0;
        
        case kLinear:
            return static_cast<double>(nTapers + 1UL - k) / static_cast<double>(nTapers);
            
        case kParabolic:
            return 1.0 - (((k - 1) * (k - 1)) / static_cast<double>(nTapers * nTapers));
    }
}

// Process

void FrameLib_Multitaper::process()
{
    // Get Input
    
    unsigned long sizeIn, sizeOut;
    const double *input = getInput(0, &sizeIn);
    
    // Get FFT size log 2
    
    unsigned long FFTSizeLog2 = static_cast<unsigned long>(mProcessor.calc_fft_size_log2(sizeIn));
    unsigned long FFTSize = 1 << FFTSizeLog2;
    sizeOut = (FFTSize >> 1) + 1;
    
    // Check size
    
    if ((FFTSize * 2) > mProcessor.max_fft_size() || !sizeIn)
        sizeOut = 0;
    
    // Calculate output size
    
    requestOutputSize(0, sizeOut);
    allocateOutputs();
    
    double *output = getOutput(0, &sizeOut);
    auto temp = allocAutoArray<double>((FFTSize + 1) << 1);
    
    unsigned long nTapers = mParameters.getInt(kNumTapers);
    
    // Transform
    
    if (temp && sizeOut && output)
    {
        FFT_SPLIT_COMPLEX_D spectrum;
        
        spectrum.realp = ((double *) temp);
        spectrum.imagp = ((double *) temp) + (FFTSize + 1);
        
        // Take the real fft
        
        mProcessor.rfft(spectrum, input, sizeIn, (FFTSizeLog2 + 1));
        
        // Move Nyquist Bin
        
        spectrum.realp[FFTSize] = spectrum.imagp[0];
        spectrum.imagp[FFTSize] = 0.0;
        spectrum.imagp[0] = 0.0;
        
        // Scale
        
        mProcessor.scale_spectrum(spectrum, FFTSize + 1, 0.5);
        
        // Zero output power spectrum
        
        zeroVector(output, sizeOut);

        // Do tapers
        
        double weightSum = 0.0;
        
        for (unsigned long i = 1; i <= nTapers; i++)
            weightSum += getWeight(i, nTapers);
     
        // N.B. multiplication by 2 as the taper should be * sqrt(2)
        
        bool normalise = mParameters.getBool(kNormalise);
        double normFactor = 2.0 / (weightSum * (normalise ? FFTSize * FFTSize : 1));
        
        for (unsigned long i = 1; i <= nTapers; i++)
        {
            const double weight = getWeight(i, nTapers) * normFactor;
            
            // Deal with bins safely (could be optimised)
            
            for (unsigned long j = 0; j < sizeOut; j++)
            {
                double r1, i1, r2, i2, r3, i3;
                
                getWrapped(r1, i1, spectrum.realp, spectrum.imagp, FFTSize << 1, ((j << 1) + i));
                getWrapped(r2, i2, spectrum.realp, spectrum.imagp, FFTSize << 1, ((j << 1) - i));
                
                // Note that the sine window is a multiplication by -i (RHS) and i (LHS)
                
                r3 = (i2 - i1);
                i3 = (r1 - r2);
                
                output[j] += ((r3 * r3) + (i3 * i3)) * weight;
            }
        }
        
        if (mParameters.getEnum<Outputs>(kOutput) == kAmplitude)
        {
            for (unsigned long j = 0; j < sizeOut; j++)
                output[j] = sqrt(output[j]);
        }
    }
}
