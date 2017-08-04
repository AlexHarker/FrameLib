
#include "FrameLib_SampleRate.h"

// Constructor

FrameLib_SampleRate::FrameLib_SampleRate(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, &sParamInfo, 1, 1)
{
    mParameters.addEnum(kMode, "mode", 0);
    mParameters.addEnumItem(kValue, "value");
    mParameters.addEnumItem(kNyquist, "nyquist");
    mParameters.addEnumItem(kSampsToMS, "samples->ms");
    mParameters.addEnumItem(kSampsToSecs, "samples->seconds");
    mParameters.addEnumItem(kMSToSamps, "ms->samples");
    mParameters.addEnumItem(kSecsToSamps, "seconds->samples");
    mParameters.addEnumItem(kNormToFreq, "norm->freq");
    mParameters.addEnumItem(kHalfNormToFreq, "halfnorm->freq");
    mParameters.addEnumItem(kFreqToNorm, "freq->norm");
    mParameters.addEnumItem(kFreqToHalfNorm, "freq->halfnorm");
    mParameters.setInstantiation();
    
    mParameters.set(serialisedParameters);
}

// Info

std::string FrameLib_SampleRate::objectInfo(bool verbose)
{
    return getInfo("Convert values based on the sample rate / Output a value based on the sampling rate value: "
                   "The output is either a single value or (when converting values) the size matches the input size. "
                   "The sample rate or the nyquist frequency can be requested. A number of conversions are offered.",
                   "Convert values based on the sample rate / Output a value based on the sampling rate value.", verbose);
}

std::string FrameLib_SampleRate::inputInfo(unsigned long idx, bool verbose)
{
    return getInfo("Trigger / Input Frame - triggers cause output of a value / input frames are converted as specified",
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
        "value - output the sample rate. "
        "nyquist - output the nyquist frequency. "
        "samples->ms - convert from samples to milliseconds. "
        "samples->seconds - convert from samples to seconds. "
        "ms->samples - convert from milliseconds to samples. "
        "seconds->samples - convert from seconds to samples. "
        "norm->freq - convert from normalised frequency [0 - 1 is 0 - sample rate] to frequency in Hz. "
        "halfnorm->freq - convert from half normalised frequency [0 - 1 is 0 - nyquist] to frequency in Hz.  "
        "freq->norm  - convert from frequency in Hz to normalised frequency [0 - 1 is 0 - sample rate]. "
        "freq->halfnorm - convert from frequency in Hz to hald normalised frequency [0 - 1 is 0 - nyquist].");
}

// Process

void FrameLib_SampleRate::process()
{
    unsigned long sizeIn, sizeOut;
    
    double *input = getInput(0, &sizeIn);
    
    Modes mode = (Modes) mParameters.getInt(kMode);
    bool outputValue = mode == kValue || mode == kNyquist;
    
    sizeOut = outputValue ? 1 : sizeIn;
    requestOutputSize(0, sizeOut);
    allocateOutputs();
    
    double *output = getOutput(0, &sizeOut);
    
    double conversionFactor = 1.0;
    
    switch(mode)
    {
        case kValue:            conversionFactor = 1.0;                         break;
        case kNyquist:          conversionFactor = 0.5;                         break;
        case kSampsToMS:        conversionFactor = 1000.0 / mSamplingRate;      break;
        case kSampsToSecs:      conversionFactor = 1.0 / mSamplingRate;         break;
        case kMSToSamps:        conversionFactor = mSamplingRate / 1000.0;      break;
        case kSecsToSamps:      conversionFactor = mSamplingRate;               break;
        case kNormToFreq:       conversionFactor = mSamplingRate;               break;
        case kHalfNormToFreq:   conversionFactor = mSamplingRate / 2.0;         break;
        case kFreqToNorm:       conversionFactor = 1.0 / mSamplingRate;         break;
        case kFreqToHalfNorm:   conversionFactor = 2.0 / mSamplingRate;         break;
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