
#ifndef FRAMELIB_MAP_H
#define FRAMELIB_MAP_H

#include "FrameLib_DSP.h"

class FrameLib_Map : public FrameLib_Processor
{
    enum AttributeList {kMode, kInLo, kInHi, kOutLo, kOutHi, kClip};
    enum Modes {kLinear, kLog, kExp, kDB, kInvDB, kTranspose, kInvTranspose};
    enum ScaleMode {kScaleLinear, kScaleExp, kScaleLog};

public:
    
    FrameLib_Map (DSPQueue *queue, FrameLib_Attributes::Serial *serialisedAttributes, void *owner) : FrameLib_Processor(queue, 2, 1)
    {
        mAttributes.addEnum(kMode, "mode", 0);
        mAttributes.addEnumItem(kLinear, "linear");
        mAttributes.addEnumItem(kLog, "log");
        mAttributes.addEnumItem(kExp, "exp");
        mAttributes.addEnumItem(kDB, "db");
        mAttributes.addEnumItem(kInvDB, "invdb");
        mAttributes.addEnumItem(kTranspose, "transpose");
        mAttributes.addEnumItem(kInvTranspose, "invtranspose");
        
        mAttributes.addDouble(kInLo, "inlo", 0., 1);
        mAttributes.addDouble(kInHi, "inhi", 1., 2);
        mAttributes.addDouble(kOutLo, "outlo", 0., 3);
        mAttributes.addDouble(kOutHi, "outhi", 1., 4);
        
        mAttributes.addBool(kClip, "clip", TRUE, 5);
        
        mAttributes.set(serialisedAttributes);
        
        inputMode(1, TRUE, FALSE, FALSE);
        
        setScaling();
    }
    
private:
    
    void setScaling()
    {
        double inLo = mAttributes.getValue(kInLo);
        double inHi = mAttributes.getValue(kInHi);
        double outLo = mAttributes.getValue(kOutLo);
        double outHi = mAttributes.getValue(kOutHi);
        
        switch ((Modes) mAttributes.getInt(kMode))
        {
            case kLinear:
                mMode = kScaleLinear;
                break;
            case kLog:
                mMode = kScaleLog;
                break;
            case kExp:
                mMode = kScaleExp;
                break;
            case kDB:
                mMode = kScaleExp;
                outLo = pow(10, outLo / 20.);
                outHi = pow(10, outHi / 20.);
                break;
            case kInvDB:
                mMode = kScaleLog;
                inLo = pow(10, inLo / 20.);
                inHi = pow(10, inHi / 20.);
                break;
            case kTranspose:
                mMode = kScaleExp;
                outLo = pow(2, outLo / 12.);
                outHi = pow(2, outHi / 12.);
                break;
            case kInvTranspose:
                mMode = kScaleLog;
                inLo = pow(2, inLo / 12.);
                inHi = pow(2, inHi / 12.);
                break;
        }
        
        mMin = outLo < outHi ? outLo : outHi;
        mMax = outLo < outHi ? outHi : outLo;
        
        // Calculate simplified linear scaling values
        
        switch (mMode)
        {
            case kScaleLinear:
                break;
            
            case kScaleLog:
                inLo = log(inLo);
                inHi = log(inHi);
                break;
                
            case kScaleExp:
                outLo = log(outLo);
                outHi = log(outHi);
                break;
        }
        
        double mul = (inLo == inHi) ? 0.0 : (outHi - outLo) / (inHi - inLo);
        double sub = (inLo * mul) - outLo;
        
        mMul = mul;
        mSub = sub;
    }
    
protected:
    
    void update()
    {
        FrameLib_Attributes::Serial *serialised = getInput(1);
        
        if (serialised)
            mAttributes.set(serialised);
       
        setScaling();
    }
    
    void process()
    {
        unsigned long size;
        double *input = getInput(0, &size);
        
        requestOutputSize(0, size);
        allocateOutputs();
        
        double *output = getOutput(0, &size);
        
        double mul = mMul;
        double sub = mSub;
        
        switch (mMode)
        {
            case kScaleLinear:
                
                for (unsigned long i = 0; i < size; i++)
                    output[i] = (input[i] * mul) - sub;
                break;
        
            case kScaleLog:
                for (unsigned long i = 0; i < size; i++)
                    output[i] = log(input[i]) * mul - sub;
                break;
                
            case kScaleExp:
                for (unsigned long i = 0; i < size; i++)
                    output[i] = exp((input[i] * mul) - sub);
                break;
        }
        
        if (mAttributes.getBool(kClip))
        {
            double min = mMin;
            double max = mMax;
            
            for (unsigned long i = 0; i < size; i++)
                output[i] = (output[i] > max) ? max : (output[i] < min) ? min : output[i];
        }
            
    }
    
private:
    
    ScaleMode mMode;
    
    double mMul;
    double mSub;
    double mMin;
    double mMax;
};

#endif
