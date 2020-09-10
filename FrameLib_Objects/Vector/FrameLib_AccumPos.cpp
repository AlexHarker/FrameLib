
#include "FrameLib_AccumPos.h"
#include "../../FrameLib_Dependencies/Statistics.hpp"

// Constructor

FrameLib_AccumPos::FrameLib_AccumPos(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: Base(context, proxy, &sParamInfo)
{
    mParameters.addDouble(kPercentage, "percentage", 50.0, 0);
    mParameters.setClip(0.0, 100.0);
    
    addDefaultParameters();
    
    mParameters.set(serialisedParameters);
    
    addParameterInput();
}

// Info

std::string FrameLib_AccumPos::objectInfo(bool verbose)
{
    const char *basic = "Calculates the position in a vector at which the sum reaches a given percentage of the total sum";
    
    return formatObjectInfo(basic, verbose);
}

// Parameter Info

FrameLib_AccumPos::ParameterInfo FrameLib_AccumPos::sParamInfo;

FrameLib_AccumPos::ParameterInfo::ParameterInfo()
{
    add("Sets the percentage of the sum used for calculation [0-100].");
    Base::addDefaultParameterInfo(*this);
}

// Compute

double FrameLib_AccumPos::compute(const double *input, size_t size)
{
    return statPDFPercentile(input, mParameters.getValue(kPercentage), size);
}
