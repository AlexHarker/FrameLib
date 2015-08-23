
#ifndef FRAMELIB_SAMPLERATE_H
#define FRAMELIB_SAMPLERATE_H

#include "FrameLib_DSP.h"

class FrameLib_SampleRate : public FrameLib_Processor
{
    enum AttributeList {kMode};
    enum Modes {kValue, kNyquist, kSampsToMS, kSampsToSecs, kMSToSamps, kSecsToSamps, kNormToFreq, kHalfNormToFreq, kFreqToNorm, kFreqToHalfNorm};

public:
    
    FrameLib_SampleRate (DSPQueue *queue, FrameLib_Attributes::Serial *serialisedAttributes) : FrameLib_Processor(queue, 1, 1)
    {
        mAttributes.addEnum(kMode, "mode", 0);
        mAttributes.addEnumItem(kValue, "value");
        mAttributes.addEnumItem(kNyquist, "nyquist");
        mAttributes.addEnumItem(kSampsToMS, "samples->ms");
        mAttributes.addEnumItem(kSampsToSecs, "samples->seconds");
        mAttributes.addEnumItem(kMSToSamps, "ms->samples");
        mAttributes.addEnumItem(kSecsToSamps, "seconds->samples");
        mAttributes.addEnumItem(kNormToFreq, "norm->freq");
        mAttributes.addEnumItem(kHalfNormToFreq, "halfnorm->freq");
        mAttributes.addEnumItem(kFreqToNorm, "freq->norm");
        mAttributes.addEnumItem(kFreqToHalfNorm, "freq->halfnorm");
        
        mAttributes.set(serialisedAttributes);
    }
    
protected:
    
    void process()
    {
        unsigned long sizeIn, sizeOut;
        
        double *input = getInput(0, &sizeIn);
        
        Modes mode = (Modes) mAttributes.getInt(kMode);
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
            for (unsigned int i = 0; i < sizeOut; i++)
                output[i] = input[i] * conversionFactor;
        }
    }
};

#endif
