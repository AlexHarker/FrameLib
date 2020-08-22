
#ifndef FRAMELIB_VECTOR_TEMPLATE_H
#define FRAMELIB_VECTOR_TEMPLATE_H

#include "FrameLib_DSP.h"

enum FrameLib_Vector_Defaults { kDefaultZero, kDefaultOne, kDefaultPosInf, kDefaultNegInf, kAlwaysCalculate };

// Vector Base

template <unsigned long nParams = 0, FrameLib_Vector_Defaults emptyDefault = kDefaultZero>
class FrameLib_VectorBase : public FrameLib_Processor
{
    enum ParameterList { kEmptyMode = nParams, kDefault };
    enum EmptyModes { kUseEmpty, kUseDefault };
    
public:
    
    // Constructor
    
    FrameLib_VectorBase(FrameLib_Context context, FrameLib_Proxy *proxy, FrameLib_Parameters::Info *info)
    : FrameLib_Processor(context, proxy, info, 1, 1)
    {}
    
    std::string inputInfo(unsigned long idx, bool verbose) override     { return "Input"; }
    std::string outputInfo(unsigned long idx, bool verbose) override    { return "Result"; }

protected:
    
    // Info
    
    std::string formatObjectInfo(const char *basic, bool verbose)
    {
        if (emptyDefault == kAlwaysCalculate)
            return formatInfo("#: The output is a single value.",
                              "#.", basic, verbose);
        else
            return formatInfo("#: The output is a single value. "
                              "When the input frame is empty then the output is determined by the empty parameter.",
                              "#.", basic, verbose);
    }
    
    static void addDefaultParameterInfo(FrameLib_Parameters::Info& info)
    {
        info.add("Sets the mode used when dealing with empty frames: "
                 "empty - output an empty frame. "
                 "default - output the default value (see the default parameter).");
        info.add("Sets the default output value.");
    }
    
    void addDefaultParameters()
    {
        if (emptyDefault != kAlwaysCalculate)
        {
            double defaultValue = 0.0;
            
            mParameters.addEnum(kEmptyMode, "empty", nParams);
            mParameters.addEnumItem(kUseEmpty, "empty");
            mParameters.addEnumItem(kUseDefault, "default");
            mParameters.setInstantiation();
            
            switch (emptyDefault)
            {
                case kDefaultOne:       defaultValue = 1.0;                                         break;
                case kDefaultPosInf:    defaultValue = std::numeric_limits<double>::infinity();     break;
                case kDefaultNegInf:    defaultValue = -std::numeric_limits<double>::infinity();    break;
                default:                                                                            break;
            }
            
            mParameters.addDouble(kDefault, "default", defaultValue, nParams + 1);
            mParameters.setInstantiation();
        }
    }
    
private:
    
    // Process
    
    virtual double compute(const double *input, size_t size) = 0;
    
    void process() override
    {
        unsigned long sizeIn, sizeOut;
        const double *input = getInput(0, &sizeIn);
        
        bool alwaysCalculate = emptyDefault == kAlwaysCalculate;
        bool calculate = (sizeIn != 0) || alwaysCalculate;
        bool useDefault = !alwaysCalculate && mParameters.getEnum<EmptyModes>(kEmptyMode) == kUseDefault;
        
        requestOutputSize(0, (calculate || useDefault) ? 1 : 0);
        allocateOutputs();
            
        double *output = getOutput(0, &sizeOut);
        
        if (sizeOut)
        {
            if (calculate)
                output[0] = compute(input, sizeIn);
            else
                output[0] = mParameters.getValue(kDefault);
        }
    }
};

// Vector (Function Version)

template <double func(const double *, size_t), FrameLib_Vector_Defaults emptyDefault = kDefaultZero>
class FrameLib_Vector final : public FrameLib_VectorBase<0, emptyDefault>
{
    using Base = FrameLib_VectorBase<0, emptyDefault>;
 
    // Parameter Enums and Info
    
    struct ParameterInfo : public FrameLib_Parameters::Info
    {
        ParameterInfo() { Base::addDefaultParameterInfo(*this); }
    };
    
public:
    
    FrameLib_Vector(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : Base(context, proxy, getParameterInfo())
    {
        this->addDefaultParameters();
        
        this->mParameters.set(serialisedParameters);
    }
    
    std::string objectInfo(bool verbose) override
    {
        std::string basic = this->formatInfo("Calculates the # of the input frame", getOpString());
        
        return this->formatObjectInfo(basic.c_str(), verbose);
    }
    
private:
    
    double compute(const double *input, size_t size) override
    {
        return func(input, size);
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
