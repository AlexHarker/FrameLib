
#ifndef FRAMELIB_BINARY_TEMPLATE_H
#define FRAMELIB_BINARY_TEMPLATE_H

#include "FrameLib_DSP.h"

// Binary Operator

template <typename Op>
class FrameLib_BinaryOp final : public FrameLib_Processor
{
    // Parameter Enums and Info
    
    struct ParameterInfo : public FrameLib_Parameters::Info
    {
        ParameterInfo()
        {
            add("Sets the mode used when dealing with mismatched input lengths: "
                "wrap - the shorter input is read modulo against the longer input. "
                "shrink - the output length is set to that of the shorter input. "
                "pad_in - the shorter input is padded prior to calculation to match the longer input. "
                "pad_out - the output is padded to match the length of the longer input.");
            add("Sets which inputs trigger output.");
            add("Sets the value used for padding (for either pad_in or pad_out modes).");
        }
    };
    
    enum ParameterList { kMismatchMode, kTriggers, kPadding };
    enum MismatchModes { kWrap, kShrink, kPadIn, kPadOut };
    enum TriggerModes { kBoth, kLeft, kRight };
    
public:
    
    // Constructor
    
    FrameLib_BinaryOp(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
    : FrameLib_Processor(context, proxy, getParameterInfo(), 2, 1)
    {
        mParameters.addEnum(kMismatchMode, "mismatch");
        mParameters.addEnumItem(kWrap, "wrap");
        mParameters.addEnumItem(kShrink, "shrink");
        mParameters.addEnumItem(kPadIn, "pad_in");
        mParameters.addEnumItem(kPadOut, "pad_out");
        mParameters.setInstantiation();

        mParameters.addEnum(kTriggers, "trigger_ins");
        mParameters.addEnumItem(kBoth, "both");
        mParameters.addEnumItem(kLeft, "left");
        mParameters.addEnumItem(kRight, "right");
        mParameters.setInstantiation();

        mParameters.addDouble(kPadding, "pad", 0.0);
        mParameters.setInstantiation();

        mParameters.set(serialisedParameters);
                                    
        mMismatchMode = mParameters.getEnum<MismatchModes>(kMismatchMode);
        mPadValue = mParameters.getValue(kPadding);
        
        TriggerModes triggers = mParameters.getEnum<TriggerModes>(kTriggers);
        
        if (triggers == kLeft)
            setInputMode(1, false, false, false);
        if (triggers == kRight)
            setInputMode(0, false, false, false);
    }
    
    // Info
    
    std::string objectInfo(bool verbose) override
    {
        return formatInfo("#: Calculation is performed on pairs of values in turn. "
                          "The output is a frame at least as long as the shorter of the two inputs. "
                          "If inputs mismatch in length the result depends on the mismatch parameter. "
                          "Either or both inputs may be set to trigger output.",
                          "#.", getDescriptionString(), verbose);
    }
    
    std::string inputInfo(unsigned long idx, bool verbose) override     { return idx ? "Right Operand" : "Left Operand"; }
    std::string outputInfo(unsigned long idx, bool verbose) override    { return "Output"; }
    
private:
    
    // Process
    
    void process() override
    {
        MismatchModes mode = mMismatchMode;
        Op op;
        
        unsigned long sizeIn1, sizeIn2, sizeCommon, sizeOut;
        
        const double *input1 = getInput(0, &sizeIn1);
        const double *input2 = getInput(1, &sizeIn2);
        double defaultValue = mPadValue;
        
        // Get common size
        
        sizeCommon = std::min(sizeIn1, sizeIn2);
        
        // Calculate output size by mode
        
        switch (mode)
        {
            case kShrink:
                sizeOut = sizeCommon;
                break;
            default:
                sizeOut = std::max(sizeIn1, sizeIn2);
                if (mode == kWrap)
                    sizeOut = sizeIn1 && sizeIn2 ? sizeOut : 0;
                break;
        }
        
        // Allocate output
        
        requestOutputSize(0, sizeOut);
        allocateOutputs();
        double *output = getOutput(0, &sizeOut);
        sizeCommon = std::min(sizeCommon, sizeOut);
        
        if (!sizeOut)
            return;
        
        // Do first part
        
        for (unsigned long i = 0; i < sizeCommon; i++)
            output[i] = op(input1[i], input2[i]);
        
        // Clean up if sizes don't match
        
        if (sizeIn1 != sizeIn2)
        {
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
                               output[i] = op(input1[i], value);
                       }
                       else
                       {
                           for (unsigned long i = sizeCommon; i < sizeOut;)
                               for (unsigned long j = 0; j < sizeIn2 && i < sizeOut; i++, j++)
                                output[i] = op(input1[i], input2[j]);
                       }
                    }
                    else
                    {
                        if (sizeIn1 == 1)
                        {
                            double value = input1[0];
                            for (unsigned long i = 1; i < sizeOut; i++)
                                output[i] = op(value, input2[i]);
                        }
                        else
                        {
                            for (unsigned long i = sizeCommon; i < sizeOut;)
                                for (unsigned long j = 0; j < sizeIn1 && i < sizeOut; i++, j++)
                                    output[i] = op(input1[j], input2[i]);
                        }
                    }
                    break;
                    
                case kPadIn:
                    
                    if (sizeIn1 > sizeIn2)
                    {
                        for (unsigned long i = sizeCommon; i < sizeOut; i++)
                            output[i] = op(input1[i], defaultValue);
                    }
                    else
                    {
                        for (unsigned long i = sizeCommon; i < sizeOut; i++)
                            output[i] = op(defaultValue, input2[i]);
                    }
                    break;
                    
                case kPadOut:
                    
                    for (unsigned long i = sizeCommon; i < sizeOut; i++)
                        output[i] = defaultValue;
                    break;
            }
        }
    }
        
    // Description (specialise to change description)
    
    const char *getDescriptionString() { return "Binary Operator - No operator info available"; }

    ParameterInfo *getParameterInfo()
    {
        static ParameterInfo info;
        return &info;
    }
    
    // Data
    
    double mPadValue;
    MismatchModes mMismatchMode;
};

// Binary Functor

template <double func(double, double)>
struct Binary_Functor
{
    double operator()(double x, double y) { return func(x, y); }
};

// Binary (Function Version)

template <double func(double, double)>
using FrameLib_Binary = FrameLib_BinaryOp<Binary_Functor<func>>;

#endif
