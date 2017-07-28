
#ifndef FRAMELIB_SAMPLERATE_H
#define FRAMELIB_SAMPLERATE_H

#include "FrameLib_DSP.h"

class FrameLib_SampleRate : public FrameLib_Processor
{
    enum ParameterList { kMode };
    enum Modes { kValue, kNyquist, kSampsToMS, kSampsToSecs, kMSToSamps, kSecsToSamps, kNormToFreq, kHalfNormToFreq, kFreqToNorm, kFreqToHalfNorm };

public:
    
    FrameLib_SampleRate (FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, 1, 1)
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
        
        mParameters.set(serialisedParameters);
    }
    
private:
    
    void process()
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
};

#endif
