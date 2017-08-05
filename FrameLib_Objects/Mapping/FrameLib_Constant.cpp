
#include "FrameLib_Constant.h"
#include <cmath>

FrameLib_Constant::FrameLib_Constant(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, &sParamInfo, 2, 1)
{
    mParameters.addEnum(kConstant, "constant", 0);
    mParameters.addEnumItem(kPI, "pi");
    mParameters.addEnumItem(kE, "e");
    mParameters.addEnumItem(kSqrt2, "sqrt2");
    mParameters.addEnumItem(kInvPI, "1/pi");
    mParameters.addEnumItem(kInvE, "1/e");
    mParameters.addEnumItem(kInvSqrt2, "1/sqrt2");
    
    mParameters.addDouble(kMultiplier, "multiplier", 1.0, 1);

    mParameters.set(serialisedParameters);
    
    setParameterInput(1);
}

// Info

std::string FrameLib_Constant::objectInfo(bool verbose)
{
    return getInfo("Outputs the specified useful contant: Output is a single value. The constant can be multiplied before output",
                   "Outputs the specified useful contant.", verbose);
}

std::string FrameLib_Constant::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return getInfo("Parameter Update - tagged input updates paramaeters", "Parameter Update", verbose);
    else
        return getInfo("Trigger Input - input frames generate output", "Trigger Input", verbose);
}

std::string FrameLib_Constant::outputInfo(unsigned long idx, bool verbose)
{
    return "Output Values";
}

// Parameter Info

FrameLib_Constant::ParameterInfo FrameLib_Constant::sParamInfo;

FrameLib_Constant::ParameterInfo::ParameterInfo()
{
    add("Sets the constant to output.");
    add("Sets the multiplier.");
}

// Process

void FrameLib_Constant::process()
{
    requestOutputSize(0, 1);
    
    if (allocateOutputs())
    {
        unsigned long size;
        double *output = getOutput(0, &size);
        
        double value = 1.0;
        
        switch ((Constants) mParameters.getInt(kConstant))
        {
            case kPI:           value = M_PI;           break;
            case kE:            value = M_E;            break;
            case kSqrt2:        value = M_SQRT2;        break;
            case kInvPI:        value = M_1_PI;         break;
            case kInvE:         value = 1.0/M_E;        break;
            case kInvSqrt2:     value = M_SQRT1_2;      break;
        }
        
        output[0] = value * mParameters.getValue(kMultiplier);
    }
}
