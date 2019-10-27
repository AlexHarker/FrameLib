
#ifndef FRAMELIB_VECTOR_TEMPLATE_H
#define FRAMELIB_VECTOR_TEMPLATE_H

#include "FrameLib_DSP.h"

enum FrameLib_Vector_EmptyMode { kDefaultZero, kDefaultOne, kDefaultPosInf, kDefaultNegInf, kAlwaysCalculate};

template <double func(const double *, size_t), FrameLib_Vector_EmptyMode emptyMode = kDefaultZero>
class FrameLib_Vector final : public FrameLib_Processor
{
    // Parameter Enums and Info
    
    struct ParameterInfo : public FrameLib_Parameters::Info
    {
        ParameterInfo()
        {
            add("Sets the mode used when dealing with empty frames: "
                "empty - output an empty frame. "
                "default - output the default value (see the default parameter).");
            add("Sets the default output value.");
        }
    };
    
    enum ParameterList { kEmptyMode, kDefault };
    enum EmptyModes { kUseEmpty, kUseDefault };
    
public:
    
    // Constructor
    
    FrameLib_Vector(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_Processor(context, proxy, getParameterInfo(), 1, 1)
    {
        if (emptyMode != kAlwaysCalculate)
        {
            double defaultValue = 0.0;
            
            mParameters.addEnum(kEmptyMode, "empty", 0);
            mParameters.addEnumItem(kUseEmpty, "empty");
            mParameters.addEnumItem(kUseDefault, "default");
            mParameters.setInstantiation();
        
            switch (emptyMode)
            {
                case kDefaultOne:       defaultValue = 1.0;                                         break;
                case kDefaultPosInf:    defaultValue = std::numeric_limits<double>::infinity();     break;
                case kDefaultNegInf:    defaultValue = -std::numeric_limits<double>::infinity();    break;
                default:                                                                            break;
            }
            
            mParameters.addDouble(kDefault, "default", defaultValue, 1);
            mParameters.setInstantiation();
        }
        
        mParameters.set(serialisedParameters);
    }
    
    // Info
    
    std::string objectInfo(bool verbose) override
    {
        if (emptyMode == kAlwaysCalculate)
            return formatInfo("Calculates the # of the input frame: The output is a single value.",
                       "Calculates the # of the input frame.", getOpString(), verbose);
        else
            return formatInfo("Calculates the # of the input frame: The output is a single value. "
                              "When the input frame is empty then the output is determined by the empty_mode parameter.",
                              "Calculates the # of the input frame.", getOpString(), verbose);
    }
    
    std::string inputInfo(unsigned long idx, bool verbose) override     { return "Input"; }
    std::string outputInfo(unsigned long idx, bool verbose) override    { return "Result"; }

private:
    
    // Process
    
    void process() override
    {
        unsigned long sizeIn, sizeOut;
        const double *input = getInput(0, &sizeIn);
        
        bool calculate = (sizeIn != 0) || (emptyMode == kAlwaysCalculate);
        bool useDefault = (emptyMode != kAlwaysCalculate) && mParameters.getInt(kEmptyMode);
        
        requestOutputSize(0, (calculate || useDefault) ? 1 : 0);
        allocateOutputs();
            
        double *output = getOutput(0, &sizeOut);
            
        if (sizeOut)
        {
            if (calculate)
                output[0] = func(input, sizeIn);
            else
                output[0] = mParameters.getValue(kDefault);
        }
    }
    
    // Operator Description (specialise to change description)

    const char *getOpString() { return "<vector operation>"; }
    
    ParameterInfo *getParameterInfo()
    {
        static ParameterInfo info;
        return &info;
    }
};

#endif
