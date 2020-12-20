
#include "FrameLib_SampleRate.h"

// Constructor

FrameLib_SampleRate::FrameLib_SampleRate(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, &sParamInfo, 2, 1)
{
    mParameters.addEnum(kMode, "mode", 0);
    mParameters.addEnumItem(kRate, "rate");
    mParameters.addEnumItem(kNyquist, "nyquist");
    mParameters.addEnumItem(kSampsToMS, "samples->ms");
    mParameters.addEnumItem(kSampsToSecs, "samples->seconds");
    mParameters.addEnumItem(kMSToSamps, "ms->samples");
    mParameters.addEnumItem(kSecsToSamps, "seconds->samples");
    mParameters.addEnumItem(kNormToFreq, "normalised->hz");
    mParameters.addEnumItem(kFreqToNorm, "hz->normalised");
    
    mParameters.addEnum(kNormalisation, "normalisation", 1);
    mParameters.addEnumItem(kNormFull, "full");
    mParameters.addEnumItem(kNormNyquist, "nyquist");
    
    mParameters.set(serialisedParameters);
    
    setParameterInput(1);
}

// Info

std::string FrameLib_SampleRate::objectInfo(bool verbose)
{
    return formatInfo("Convert values based on the sample rate / Output a value based on the sampling rate value: "
                   "The output is either a single value or (when converting values) the size matches the input size. "
                   "The sample rate or the nyquist frequency can be requested. A number of conversions are offered.",
                   "Convert values based on the sample rate / Output a value based on the sampling rate value.", verbose);
}

std::string FrameLib_SampleRate::inputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Trigger / Input Frame - triggers cause output of a value / input frames are converted as specified",
                   "Trigger / Input Frame", verbose);
}

std::string FrameLib_SampleRate::outputInfo(unsigned long idx, bool verbose)
{
    return "Output Value / Scaled Frame";
}

// Parameter Info

FrameLib_SampleRate::ParameterInfo FrameLib_SampleRate::sParamInfo;

FrameLib_SampleRate::ParameterInfo::ParameterInfo()
{
    add("Sets the type of conversion / value to output: "
        "rate - output the sample rate in Hz. "
        "nyquist - output the nyquist frequency in Hz. "
        "samples->ms - convert from samples to milliseconds. "
        "samples->seconds - convert from samples to seconds. "
        "ms->samples - convert from milliseconds to samples. "
        "seconds->samples - convert from seconds to samples. "
        "normalised->freq - convert from normalised frequency to frequency in Hz. "
        "freq->normalised  - convert from frequency in Hz to normalised frequency.");
    add("Sets the refernce point for normalised frequency: "
        "rate - [0 - 1] is equivalent to [0 - sample rate]. "
        "nyquist - [0 - 1] is equivalent to [0 - nyquist].");
}

// Process

void FrameLib_SampleRate::process()
{
    unsigned long sizeIn, sizeOut;
    
    const double *input = getInput(0, &sizeIn);
    
    Modes mode = mParameters.getEnum<Modes>(kMode);
    bool outputValue = mode == kRate || mode == kNyquist;
    bool nyquistNormalise = mParameters.getEnum<Normalisations>(kNormalisation) == kNormNyquist;
    
    sizeOut = outputValue ? 1 : sizeIn;
    requestOutputSize(0, sizeOut);
    allocateOutputs();
    
    double *output = getOutput(0, &sizeOut);
    
    double conversionFactor = 1.0;
    
    switch(mode)
    {
        case kRate:             conversionFactor = 1.0;                         break;
        case kNyquist:          conversionFactor = 0.5;                         break;
        case kSampsToMS:        conversionFactor = 1000.0 / mSamplingRate;      break;
        case kSampsToSecs:      conversionFactor = 1.0 / mSamplingRate;         break;
        case kMSToSamps:        conversionFactor = mSamplingRate / 1000.0;      break;
        case kSecsToSamps:      conversionFactor = mSamplingRate;               break;
            
        case kNormToFreq:
            conversionFactor = nyquistNormalise ? mSamplingRate / 2.0 : mSamplingRate;
            break;
            
        case kFreqToNorm:
            conversionFactor = (nyquistNormalise ? 2.0 : 1.0) / mSamplingRate;
            break;
    }
    
    if (outputValue && sizeOut)
    {
        output[0] = mSamplingRate * conversionFactor;
    }
    else
    {
        for (unsigned long i = 0; i < sizeOut; i++)
            output[i] = input[i] * conversionFactor;
    }
}
