
#include "FrameLib_NanFilter.h"

// Constructor

FrameLib_NanFilter::FrameLib_NanFilter(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, &sParamInfo, 2, 1)
{
    mParameters.addEnum(kMode, "mode", 0);
    mParameters.addEnumItem(kReplace, "replace");
    mParameters.addEnumItem(kRemove, "remove");

    mParameters.addDouble(kValue, "value", 0.0, 1);
    
    mParameters.set(serialisedParameters);
    
    setParameterInput(1);
}

// Info

std::string FrameLib_NanFilter::objectInfo(bool verbose)
{
    return formatInfo("Filter NaNs from the input either by replacing, or removing them. "
                      "The length of the output depend on the mode parameter and will be no longer than the input. "
                      "The value for replacement is settable using the value parameter.",
                      "Filter NaNs from the input either by replacing them, or removing them.", verbose);
}

std::string FrameLib_NanFilter::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return parameterInputInfo(verbose);
    else
        return "Input";
}

std::string FrameLib_NanFilter::outputInfo(unsigned long idx, bool verbose)
{
    return "Output";
}

// Parameter Info

FrameLib_NanFilter::ParameterInfo FrameLib_NanFilter::sParamInfo;

FrameLib_NanFilter::ParameterInfo::ParameterInfo()
{
    add("Sets whether NaNs are replaced or removed." );
    add("Sets the value with which to replace NaNs when in replace mode.");
}

// Process

void FrameLib_NanFilter::process()
{
    unsigned long sizeIn;
    unsigned long sizeOut = 0;
    
    Modes mode = (Modes) mParameters.getInt(kMode);
    double value = mParameters.getValue(kValue);
    
    const double *input = getInput(0, &sizeIn);
    
    switch (mode)
    {
        case kReplace:
        {
            requestOutputSize(0, sizeIn);
            allocateOutputs();
            
            double* output = getOutput(0, &sizeOut);
                
            for (unsigned long i = 0; i < sizeOut; i++)
                output[i] = std::isnan(input[i]) ? value : input[i];
            
            break;
        }
        
        case kRemove:
        {
            double* temp = alloc<double>(sizeIn);
            sizeIn = temp ? sizeIn : 0;
            
            for (unsigned long i = 0; i < sizeIn; i++)
            {
                if (!std::isnan(input[i]))
                    temp[sizeOut++] = input[i];
            }
            
            requestOutputSize(0, sizeOut);
            allocateOutputs();
            
            double *output = getOutput(0, &sizeOut);
            copyVector(output, temp, sizeOut);
            dealloc(temp);
            
            break;
        }
    }
}
