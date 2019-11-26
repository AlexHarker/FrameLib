
#ifndef FRAMELIB_PERCENTILE_H
#define FRAMELIB_PERCENTILE_H

#include "FrameLib_Vector_Template.h"

class FrameLib_Percentile final : public FrameLib_VectorBase<1>
{
    using Base = FrameLib_VectorBase<1>;
    
    // Parameter Enums and Info

    enum ParameterList { kPercentile };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_Percentile(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose) override;
 
private:

    // Compute

    double compute(const double *input, size_t size) override;
    
    // Data
    
    static ParameterInfo sParamInfo;
};

#endif
