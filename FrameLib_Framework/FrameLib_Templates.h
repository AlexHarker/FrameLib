

#ifndef FRAMELIB_TEMPLATES_H
#define FRAMELIB_TEMPLATES_H

#include "FrameLib_DSP.h"
#include <functional>

// FIX - vectorise where appropriate
// FIX - default modes (situation dependent)
// FIX - reconsider fixed values across framework
// FIX - fix function versions (how are they broken??)

// Unary (Operator Version)

template <typename Op> class FrameLib_UnaryOp : public FrameLib_Processor
{
    
public:
    
    FrameLib_UnaryOp(FrameLib_Context context, FrameLib_Attributes::Serial *serialisedAttributes, void *owner) : FrameLib_Processor(context, 1, 1)
    {}
    
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
    FrameLib_Unary(FrameLib_Context context, FrameLib_Attributes::Serial *serialisedAttributes, void *owner)
    : FrameLib_UnaryOp < Unary_Functor<func> > (context, serialisedAttributes, owner) {}
};

// Binary Operator

template <typename Op> class FrameLib_BinaryOp : public FrameLib_Processor
{
    enum AttributeList {kMode, kTriggers, kPadding};
    enum Modes {kWrap, kShrink, kPadIn, kPadOut};
    enum TriggerModes {kBoth, kLeft, kRight};
    
public:
    
    FrameLib_BinaryOp(FrameLib_Context context, FrameLib_Attributes::Serial *serialisedAttributes, void *owner) : FrameLib_Processor(context, 2, 1)
    {
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
        
        unsigned long sizeIn1, sizeIn2, sizeCommon, sizeOut;
        
        double *input1 = getInput(0, &sizeIn1);
        double *input2 = getInput(1, &sizeIn2);
        double defaultValue = mPadValue;
        double *output;
        
        // Get common size
        
        sizeCommon = sizeIn1 < sizeIn2 ? sizeIn1 : sizeIn2;
        
        // Calculate output size by mode
        
        switch (mode)
        {
            case kShrink:
                sizeOut = sizeCommon;
                break;
            default:
                sizeOut = sizeIn1 > sizeIn2 ? sizeIn1 : sizeIn2;
                if (mode == kWrap)
                    sizeOut = sizeIn1 && sizeIn2 ? sizeOut : 0;
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
        
        if (sizeIn1 != sizeIn2)
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
                
                    if (sizeIn1 > sizeIn2)
                    {
                        // FIX - consider local memory for small wrapped vectors and/or faster mode switching (e.g. through function pointers / single switch etc.)
                       if (sizeIn2 == 1)
                       {
                           double value = input2[0];
                           for (unsigned int i = 1; i < sizeOut; i++)
                               output[i] = Op()(input1[i], value);
                       }
                       else
                       {
                           for (unsigned int i = sizeCommon; i < sizeOut;)
                               for (unsigned int j = 0; j < sizeIn2; i++, j++)
                                output[i] = Op()(input1[i], input2[j]);
                       }
                    }
                    else
                    {
                        if (sizeIn1 == 1)
                        {
                            double value = input1[0];
                            for (unsigned int i = 1; i < sizeOut; i++)
                                output[i] = Op()(value, input2[i]);
                        }
                        else
                        {
                            for (unsigned int i = sizeCommon; i < sizeOut;)
                                for (unsigned int j = 0; j < sizeIn1; i++, j++)
                                    output[i] = Op()(input1[j], input2[i]);
                        }
                    }
                    break;
                    
                case kPadIn:
                    
                    if (sizeIn1 > sizeIn2)
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
    FrameLib_Binary(FrameLib_Context context, FrameLib_Attributes::Serial *serialisedAttributes, void *owner)
    : FrameLib_BinaryOp < Binary_Functor<func> > (context, serialisedAttributes, owner) {}
};

// Vector

template <double func(double *, unsigned long) > class FrameLib_Vector : public FrameLib_Processor
{
    
public:
    
    FrameLib_Vector (FrameLib_Context context, FrameLib_Attributes::Serial *serialisedAttributes, void *owner) : FrameLib_Processor(context, 1, 1)
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
