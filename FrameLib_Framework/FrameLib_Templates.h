

#ifndef FRAMELIB_TEMPLATES_H
#define FRAMELIB_TEMPLATES_H

#include "FrameLib_DSP.h"
#include "FrameLib_Info.h"
#include <functional>

// OPT - vectorise where appropriate

// Unary Functor

template < double func(double)> struct Unary_Functor
{
public:
    double operator()(double x) { return func(x); }
};

// Binary Functor

template < double func(double, double)> struct Binary_Functor
{
public:
    double operator()(double x, double y) { return func(x, y); }
};

// ************************************************************************************** //

// Unary (Operator Version)

template <typename Op> class FrameLib_UnaryOp : public FrameLib_Processor
{
    
public:
    
    FrameLib_UnaryOp(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, 1, 1)
    {}
    
    const char *objectInfo(bool verbose)                        { return "Applies a unary operator to each value of a frame."; }
    const char *inputInfo(unsigned long idx, bool verbose)      { return "Input"; }
    const char *outputInfo(unsigned long idx, bool verbose)     { return "Result"; }

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

template <double func(double)> class FrameLib_Unary : public FrameLib_UnaryOp < Unary_Functor<func> >
{
public:
    FrameLib_Unary(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner)
    : FrameLib_UnaryOp < Unary_Functor<func> > (context, serialisedParameters, owner) {}
};

// ************************************************************************************** //

// Binary Operator

template <typename Op> class FrameLib_BinaryOp : public FrameLib_Processor, private FrameLib_Info
{
    enum ParameterList {kMode, kTriggers, kPadding};
    enum Modes {kWrap, kShrink, kPadIn, kPadOut};
    enum TriggerModes {kBoth, kLeft, kRight};
    
public:
    
    FrameLib_BinaryOp(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, 2, 1)
    {
        mParameters.addEnum(kMode, "mode");
        mParameters.addEnumItem(kWrap, "wrap");
        mParameters.addEnumItem(kShrink, "shrink");
        mParameters.addEnumItem(kPadIn, "padin");
        mParameters.addEnumItem(kPadOut, "padout");
        
        mParameters.addEnum(kTriggers, "triggers");
        mParameters.addEnumItem(kBoth, "both");
        mParameters.addEnumItem(kLeft, "left");
        mParameters.addEnumItem(kRight, "right");

        mParameters.addDouble(kPadding, "padding", 0.0);
        
        mParameters.set(serialisedParameters);
        
        mMode = (Modes) mParameters.getInt(kMode);
        mPadValue = mParameters.getValue(kPadding);
        
        TriggerModes triggers = (TriggerModes) mParameters.getInt(kTriggers);
        
        if (triggers == kLeft)
            inputMode(1, false, false, false);
        if (triggers == kRight)
            inputMode(0, false, false, false);
    }
    
    const char *objectInfo(bool verbose)
    {
        return getInfo("Applies a binary operator  to two input frames, one value at a time: When frames mismatch in size the result depends on the setting of the mode parameter. Either or both inputs may be set to trigger output.",
                       "Applies a binary operator to two input frames, one value at a time.", verbose);
    }
    
    const char *inputInfo(unsigned long idx, bool verbose)      { return idx ? "Right Operand" : "Left Operand"; }
    const char *outputInfo(unsigned long idx, bool verbose)     { return "Result"; }
    
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

template <double func(double, double)> class FrameLib_Binary : public FrameLib_BinaryOp < Binary_Functor<func> >
{
public:
    FrameLib_Binary(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner)
    : FrameLib_BinaryOp < Binary_Functor<func> > (context, serialisedParameters, owner) {}
};

// ************************************************************************************** //

// Vector

template <double func(double *, unsigned long) > class FrameLib_Vector : public FrameLib_Processor, private FrameLib_Info
{
    
public:
    
    FrameLib_Vector (FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, 1, 1)
    {
    }
    
    const char *objectInfo(bool verbose)
    {
        return getInfo("Applies a vector operation across an entire frame: The result is a single value.",
                       "Applies a vector operation across an entire frame.", verbose);
    }
    
    const char *inputInfo(unsigned long idx, bool verbose)      { return "Input"; }
    const char *outputInfo(unsigned long idx, bool verbose)     { return "Result"; }

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
