
#ifndef FRAMELIB_LOOKUP_H
#define FRAMELIB_LOOKUP_H

#include "FrameLib_DSP.h"
#include "../../FrameLib_Dependencies/TableReader.hpp"

class FrameLib_Lookup final : public FrameLib_Processor
{
    enum ParameterList { kScale, kEdges, kBound, kInterpolation };
    enum Scales { kSamples, kNormalised, kBipolar };
    enum Interpolation { kNone, kLinear, kHermite, kBSpline, kLagrange };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

    struct Fetcher : table_fetcher<double>
    {
        Fetcher(const double *data, intptr_t size) : table_fetcher(size, 1.0), mData(data) {}
        
        double operator()(intptr_t offset) { return mData[offset]; }
        
        const double *mData;
    };
    
public:
    
    // Constructor

    FrameLib_Lookup(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;

private:
    
    // Process
    
    void process() override;
    
    // Data
    
    static ParameterInfo sParamInfo;
};

#endif
