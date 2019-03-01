#include "FrameLib_NanFilter.h"
// Constructor

FrameLib_NanFilter::FrameLib_NanFilter(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_Processor(context, proxy, &sParamInfo, 2, 1)
{
    mParameters.addEnum(kMode, "mode");
    mParameters.addEnumItem(kReplace, "replace");
    mParameters.addEnumItem(kRemove, "remove");

    mParameters.addDouble(kValue, "value",0);
    
    mParameters.set(serialisedParameters);
    setParameterInput(1);
}

// Info

std::string FrameLib_NanFilter::objectInfo(bool verbose)
{
    return formatInfo("Replaces or removes NaNs in a vector",
                      "Replaces or removes NaNs in a vector", verbose);
}

std::string FrameLib_NanFilter::inputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Input Frames", "Input Frames", verbose);
}

std::string FrameLib_NanFilter::outputInfo(unsigned long idx, bool verbose)
{
    return "Vector with NaNs replaced";
}

// Parameter Info

FrameLib_NanFilter::ParameterInfo FrameLib_NanFilter::sParamInfo;

FrameLib_NanFilter::ParameterInfo::ParameterInfo()
{
    add("Set whether to replace or remove NaN values" );
    add("Set the value with which to replace NaN values");
}

// Process

void FrameLib_NanFilter::process()
{
    unsigned long sizeIn;
    unsigned long sizeOut = 0;
    Modes mode = (Modes) mParameters.getInt(kMode);
    double replacement = mParameters.getValue(kValue);
    const double *input = getInput(0, &sizeIn);
    
    switch(mode)
    {
        case Modes::kReplace:
        {
            requestOutputSize(0, sizeIn);
            if (allocateOutputs())
            {
                double* output = getOutput(0, &sizeOut);
                
                std::transform(input, input+sizeOut, output, [&replacement](double val)
                {
                    return std::isnan(val) ? replacement : val;
                });
            }
            break;
        }
        
        default: //remove
        {
            double* tmp = alloc<double>(sizeIn);
            
            double* tmp_end = std::copy_if(input, input + sizeIn, tmp,[](double val){
                return !std::isnan(val);
            });
            
            requestOutputSize(0, (size_t)(tmp_end - tmp));
            if(allocateOutputs())
            {
                double* output = getOutput(0, &sizeOut);
                copyVector(output, tmp, sizeOut); 
            }
            dealloc(tmp);
        }
    }
}
