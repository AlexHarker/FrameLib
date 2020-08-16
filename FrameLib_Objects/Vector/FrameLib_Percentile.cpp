
#include "FrameLib_Percentile.h"
#include "FrameLib_Sort_Functions.h"

// Constructor

FrameLib_Percentile::FrameLib_Percentile(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: Base(context, proxy, &sParamInfo)
{
    mParameters.addDouble(kPercentile, "percentile", 50.0, 0);
    mParameters.setClip(0.0, 100.0);
    
    mParameters.addBool(kInterpolate, "interpolate", false, 1);
    
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
    add("Sets whether interpolation is used to estimate values between data points.");
    Base::addDefaultParameterInfo(*this);
}

// Compute

double FrameLib_Percentile::compute(const double *input, size_t size)
{
    double result = 0.0;
    double *temp = alloc<double>(size + 1);
    
    if (temp)
    {
        double position = (mParameters.getValue(kPercentile) * (size - 1) / 100.0);
        
        sortAscending(temp, input, static_cast<unsigned long>(size));
        
        // Copy last value
        
        temp[size] = temp[size - 1];
        
        if (mParameters.getBool(kInterpolate))
        {
            // Linearly interpolate output
        
            unsigned long idx = truncToUInt(position);
            double fract = position - idx;
        
            result = temp[idx] + fract * (temp[idx + 1] - temp[idx]);
        }
        else
            result = temp[roundToUInt(position)];
    }
    
    dealloc(temp);
    
    return result;
}
