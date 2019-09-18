
#ifndef FRAMELIB_FILTERTAGS_H
#define FRAMELIB_FILTERTAGS_H

#include "FrameLib_DSP.h"

class FrameLib_FilterTags final : public FrameLib_Processor
{
    const static int maxNumFilters = 10;
    
    // Parameter Enums and Info

    enum ParameterList { kNumFilters, kFilters };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_FilterTags(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;
    
private:
    
    // Filter
    
    int filterIndex(FrameLib_Parameters::Serial::Iterator &it);
    
    // Process
    
    void process() override;
    
    // Data
    
    unsigned long mNumFilters;
    
    static ParameterInfo sParamInfo;
};

#endif
