

#ifndef FRAMELIB_BINARY_TEMPLATE_H
#define FRAMELIB_BINARY_TEMPLATE_H

#include "FrameLib_DSP.h"
#include "FrameLib_Info.h"
#include <functional>

// OPT - vectorise where appropriate

// Binary Operator

template <typename Op> class FrameLib_BinaryOp : public FrameLib_Processor
{
    // Parameter Enums and Info
    
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
    
    // Constructor
    
    FrameLib_BinaryOp(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, getParameterInfo(), 2, 1)
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
    
    // Info
    
    std::string objectInfo(bool verbose)
    {
        return getInfo("#: Calculation is performed on pairs of values in turn. The result is an output frame at least as long as the smaller of the two inputs. "
                       "When frames mismatch in size the result depends on the setting of the mode parameter. Either or both inputs may be set to trigger output.",
                       "#.", getDescriptionString(), verbose);
    }
    
    std::string inputInfo(unsigned long idx, bool verbose)      { return idx ? "Right Operand" : "Left Operand"; }
    std::string outputInfo(unsigned long idx, bool verbose)     { return "Result"; }
    
protected:
    
    // Process
    
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
    
    // Description (specialise/override to change description)
    
    virtual const char *getDescriptionString() { return "Binary Operator - No operator info available"; }

    ParameterInfo *getParameterInfo()
    {
        static ParameterInfo info;
        return &info;
    }
    
    // Data
    
    double mPadValue;
    Modes mMode;
};

// Binary (Function Version)

// Binary Functor

template <double func(double, double)> struct Binary_Functor
{
    double operator()(double x, double y) { return func(x, y); }
};

template <double func(double, double)> class FrameLib_Binary : public FrameLib_BinaryOp<Binary_Functor<func> >
{
    
public:
    
    // Constructor
    
    FrameLib_Binary(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner)
    : FrameLib_BinaryOp < Binary_Functor<func> > (context, serialisedParameters, owner) {}

private:
    
    // Description (specialise/override to change description)

    virtual const char *getDescriptionString() { return "Binary Operator - No operator info available"; }
};

#endif
