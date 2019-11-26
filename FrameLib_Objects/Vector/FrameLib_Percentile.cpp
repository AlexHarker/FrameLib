
#include "FrameLib_Percentile.h"
#include "FrameLib_Sort_Functions.h"

// Constructor

FrameLib_Percentile::FrameLib_Percentile(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : Base(context, proxy, &sParamInfo)
{
    mParameters.addDouble(kPercentile, "percentile", 50.0, 0);
    mParameters.setClip(0.0, 100.0);
    
    addDefaultParameters();
    
    mParameters.set(serialisedParameters);
    
    addParameterInput();
}

// Info

std::string FrameLib_Percentile::objectInfo(bool verbose)
{
    const char *basic = "Calculates any percentile (for example the median at 50%) of the input frame";
    
    return formatObjectInfo(basic, verbose);
}

// Parameter Info

FrameLib_Percentile::ParameterInfo FrameLib_Percentile::sParamInfo;

FrameLib_Percentile::ParameterInfo::ParameterInfo()
{
    add("Sets the percentile to calculate [0-100].");
    Base::addDefaultParameterInfo(*this);
}

// Compute

double FrameLib_Percentile::compute(const double *input, size_t size)
{
    double result = 0.0;
    double *temp = alloc<double>(size + 1);
    
    if (temp)
    {
        sortAscending(temp, input, size);
        
        // Copy last value
        
        temp[size] = temp[size - 1];
        
        // Linearly interpolate output
        
        double position = (mParameters.getValue(kPercentile) * (size - 1) / 100.0);
        unsigned long idx = truncToUInt(position);
        double fract = position - idx;
        
        result = temp[idx] + fract * (temp[idx + 1] - temp[idx]);
    }
    
    dealloc(temp);
    
    return result;
}
