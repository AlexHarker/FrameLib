

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

template <typename Op> class FrameLib_UnaryOp : public FrameLib_Processor, private FrameLib_Info
{
    
public:
    
    FrameLib_UnaryOp(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, 1, 1) {}
    
    const char *objectInfo(bool verbose)
    {
        return getInfo("Calculates the # of each value in the input frame: The result is a frame of the same size as the input.",
                       "Calculates the # of each value in the input frame.", getOpString(), verbose);
    }

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
        
        for (unsigned long i = 0; i < size; i++)
            output[i] = Op()(input[i]);
    }
    
    virtual const char *getOpString() { return "<unary operation>"; }
};

// Unary (Function Version)

template <double func(double)> class FrameLib_Unary : public FrameLib_UnaryOp < Unary_Functor<func> >
{
public:
    FrameLib_Unary(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner)
    : FrameLib_UnaryOp < Unary_Functor<func> > (context, serialisedParameters, owner) {}
private:
    virtual const char *getOpString() { return "<unary operation>"; }
};

// ************************************************************************************** //

// Binary Operator

template <typename Op> class FrameLib_BinaryOp : public FrameLib_Processor, private FrameLib_Info
{
    struct ParameterInfo : public FrameLib_Parameters::Info
    {
        ParameterInfo()
        {
            add("Sets the mode used when dealing with mismatched input lengths: "
                "wrap - the smaller input is read modulo against the larger input. "
                "shrink - the output length is set to the size of the smaller input. "
                "padin - the smaller input is padded prior to calculation to match the size of the larger input. "
                "padout - the output is padded to match the size of the larger input.");
            add("Sets which input(s) trigger output.");
            add("Sets the value used for padding (for either padin or padout modes).");
        }
    };
    
    enum ParameterList { kMode, kTriggers, kPadding };
    enum Modes { kWrap, kShrink, kPadIn, kPadOut };
    enum TriggerModes { kBoth, kLeft, kRight };
    
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
        
        mParameters.setInfo(getParameterInfo());
                            
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
        return getInfo("#: Calculation is performed on pairs of values in turn. The result is an output frame at least as long as the smaller of the two inputs. "
                       "When frames mismatch in size the result depends on the setting of the mode parameter. Either or both inputs may be set to trigger output.",
                       "#.", getDescriptionString(), verbose);
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
        
        for (unsigned long i = 0; i < sizeCommon; i++)
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
                           for (unsigned long i = 1; i < sizeOut; i++)
                               output[i] = Op()(input1[i], value);
                       }
                       else
                       {
                           for (unsigned long i = sizeCommon; i < sizeOut;)
                               for (unsigned long j = 0; j < sizeIn2; i++, j++)
                                output[i] = Op()(input1[i], input2[j]);
                       }
                    }
                    else
                    {
                        if (sizeIn1 == 1)
                        {
                            double value = input1[0];
                            for (unsigned long i = 1; i < sizeOut; i++)
                                output[i] = Op()(value, input2[i]);
                        }
                        else
                        {
                            for (unsigned long i = sizeCommon; i < sizeOut;)
                                for (unsigned long j = 0; j < sizeIn1; i++, j++)
                                    output[i] = Op()(input1[j], input2[i]);
                        }
                    }
                    break;
                    
                case kPadIn:
                    
                    if (sizeIn1 > sizeIn2)
                    {
                        for (unsigned long i = sizeCommon; i < sizeOut; i++)
                            output[i] = Op()(input1[i], defaultValue);
                    }
                    else
                    {
                        for (unsigned long i = sizeCommon; i < sizeOut; i++)
                            output[i] = Op()(defaultValue, input2[i]);
                    }
                    break;
                    
                case kPadOut:
                    
                    for (unsigned long i = sizeCommon; i < sizeOut; i++)
                        output[i] = defaultValue;
                    break;
            }
        }
    }
    
private:
    
    virtual const char *getDescriptionString() { return "Binary Operator - No operator info available"; }

    ParameterInfo *getParameterInfo()
    {
        static ParameterInfo info;
        
        return &info;
    }
    
    double mPadValue;
    Modes mMode;
};

// Binary (Function Version)

template <double func(double, double)> class FrameLib_Binary : public FrameLib_BinaryOp < Binary_Functor<func> >
{
public:
    FrameLib_Binary(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner)
    : FrameLib_BinaryOp < Binary_Functor<func> > (context, serialisedParameters, owner) {}
private:
    virtual const char *getDescriptionString() { return "Binary Operator - No operator info available"; }
};

// ************************************************************************************** //

// Vector

template <double func(double *, unsigned long) > class FrameLib_Vector : public FrameLib_Processor, private FrameLib_Info
{
    
public:
    
    FrameLib_Vector(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, 1, 1) {}
    
    const char *objectInfo(bool verbose)
    {
        return getInfo("Calculates the # of the input frame: The result is a single value.",
                       "Calculates the # of the input frame.", getOpString(), verbose);
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
    
    const char *getOpString() { return "<vector operation>"; }
};

#endif
