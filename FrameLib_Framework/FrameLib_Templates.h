

#ifndef FRAMELIB_TEMPLATES_H
#define FRAMELIB_TEMPLATES_H

#include "FrameLib_DSP.h"
#include <functional>

// FIX - vectorise where appropriate
// FIX - default modes (situation dependent
// FIX - reconsider fixed values across framework
// FIX - fix function versions

// Unary (Operator Version)

template <typename Op> class FrameLib_UnaryOp : public FrameLib_Processor
{
    
public:
    
    FrameLib_UnaryOp(DSPQueue *queue, FrameLib_Attributes::Serial *serialisedAttributes) : FrameLib_Processor(queue, 1, 1)
    {
    }
    
protected:
    
    void process()
    {
        unsigned long size;
        double *input = getInput(0, &size);
        
        requestOutputSize(0, size);
        allocateOutputs();

        double *output = getOutput(0, &size);
        
        for (unsigned int i = 0; i < size; i++)
            output[i] = Op()(input[i]);
    }
};

// Unary (Function Version)

template < double func(double)> struct Unary_Functor
{
public:
    double operator()(double x) { return func(x); }
};

template <double func(double)> class FrameLib_Unary : public FrameLib_UnaryOp < Unary_Functor<func> >
{
public:
    FrameLib_Unary(FrameLib_Processor::DSPQueue *queue, FrameLib_Attributes::Serial *serialisedAttributes)
    : FrameLib_UnaryOp < Unary_Functor<func> > (queue, serialisedAttributes) {}
};

// Binary Operator

template <typename Op> class FrameLib_BinaryOp : public FrameLib_Processor
{
    enum AttributeList {kVals, kMode, kTriggers, kPadding};
    enum Modes {kWrap, kShrink, kPadIn, kPadOut};
    enum TriggerModes {kBoth, kLeft, kRight};
    
public:
    
    FrameLib_BinaryOp(DSPQueue *queue, FrameLib_Attributes::Serial *serialisedAttributes) : FrameLib_Processor(queue, 2, 1)
    {
        mAttributes.addVariableArray(kVals, "vals", 0.0, 4096, 1, 0);
        
        mAttributes.addEnum(kMode, "mode");
        mAttributes.addEnumItem(kWrap, "wrap");
        mAttributes.addEnumItem(kShrink, "shrink");
        mAttributes.addEnumItem(kPadIn, "padin");
        mAttributes.addEnumItem(kPadOut, "padout");
        
        mAttributes.addEnum(kTriggers, "triggers");
        mAttributes.addEnumItem(kBoth, "both");
        mAttributes.addEnumItem(kLeft, "left");
        mAttributes.addEnumItem(kRight, "right");

        mAttributes.addDouble(kPadding, "padding", 0.0);
        
        mAttributes.set(serialisedAttributes);
        
        mMode = (Modes) mAttributes.getInt(kMode);
        mPadValue = mAttributes.getValue(kPadding);
        
        TriggerModes triggers = (TriggerModes) mAttributes.getInt(kTriggers);
        
        if (triggers == kLeft)
            inputMode(1, FALSE, FALSE, FALSE);
        if (triggers == kRight)
            inputMode(0, FALSE, FALSE, FALSE);
    }
    
protected:
    
    void process()
    {
        Modes mode = mMode;
        
        unsigned long sizeIn1, size_in2, sizeCommon, sizeOut;
        
        double *input1 = isConnected(0) ? getInput(0, &sizeIn1) : mAttributes.getArray(kVals, &sizeIn1);
        double *input2 = isConnected(1) ? getInput(1, &size_in2) : mAttributes.getArray(kVals, &size_in2);
        double defaultValue = mPadValue;
        double *output;
        
        // Get common size
        
        sizeCommon = sizeIn1 < size_in2 ? sizeIn1 : size_in2;
        
        // Calculate output size by mode
        
        switch (mode)
        {
            case kShrink:
                sizeOut = sizeCommon;
                break;
            default:
                sizeOut = sizeIn1 > size_in2 ? sizeIn1 : size_in2;
                if (mode == kWrap)
                    sizeOut = sizeIn1 && size_in2 ? sizeOut : 0;
                break;
        }
        
        // Allocate output
        
        requestOutputSize(0, sizeOut);
        allocateOutputs();
        output = getOutput(0, &sizeOut);
        sizeCommon = sizeCommon > sizeOut ? sizeOut : sizeCommon;
        
        // Do first part
        
        for (unsigned int i = 0; i < sizeCommon; i++)
            output[i] = Op()(input1[i], input2[i]);
        
        // Clean up if sizes don't match
        
        if (sizeIn1 != size_in2)
        {
            if (mode == kWrap)
            {
                if (!sizeOut)
                    return;
                
                if (sizeOut == 1)
                {
                    mode = kPadIn;
                    defaultValue = (sizeIn1 == 1) ? input1[0] : input2[0];
                }
            }
            
            switch (mode)
            {
                case kShrink:
                    break;
                
                case kWrap:
                
                    if (sizeIn1 > size_in2)
                    {
                       for (unsigned int i = sizeCommon; i < sizeOut;)
                            for (unsigned int j = 0; j < size_in2; i++, j++)
                                output[i] = Op()(input1[i], input2[j]);
                    }
                    else
                    {
                        for (unsigned int i = sizeCommon; i < sizeOut;)
                            for (unsigned int j = 0; j < sizeIn1; i++, j++)
                                output[i] = Op()(input1[j], input2[i]);
                    }
                    break;
                    
                case kPadIn:
                    
                    if (sizeIn1 > size_in2)
                    {
                        for (unsigned int i = sizeCommon; i < sizeOut; i++)
                            output[i] = Op()(input1[i], defaultValue);
                    }
                    else
                    {
                        for (unsigned int i = sizeCommon; i < sizeOut; i++)
                            output[i] = Op()(defaultValue, input2[i]);
                    }
                    break;
                    
                case kPadOut:
                    
                    for (unsigned int i = sizeCommon; i < sizeOut; i++)
                        output[i] = defaultValue;
                    break;
            }
        }
    }
    
private:
    
    double mPadValue;
    Modes mMode;
};

// Binary (Function Version)

template < double func(double, double)> struct Binary_Functor
{
public:
    double operator()(double x, double y) { return func(x, y); }
};

template <double func(double, double)> class FrameLib_Binary : public FrameLib_BinaryOp < Binary_Functor<func> >
{
public:
    FrameLib_Binary(FrameLib_Processor::DSPQueue *queue, FrameLib_Attributes::Serial *serialisedAttributes)
    : FrameLib_BinaryOp < Binary_Functor<func> > (queue, serialisedAttributes) {}
};

// Vector

template <double func(double *, unsigned long) > class FrameLib_Vector : public FrameLib_Processor
{
    
public:
    
    FrameLib_Vector (DSPQueue *queue, FrameLib_Attributes::Serial *serialisedAttributes) : FrameLib_Processor(queue, 1, 1)
    {
    }
    
protected:
    
    void process()
    {
        unsigned long sizeIn, sizeOut;
        
        requestOutputSize(0, 1);
        allocateOutputs();
        
        double *output = getOutput(0, &sizeOut);
        double *input = getInput(0, &sizeIn);
        
        if (sizeOut)
            output[0] = func(input, sizeIn);
    }
};


#endif
