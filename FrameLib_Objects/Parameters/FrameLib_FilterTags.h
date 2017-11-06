
#ifndef FRAMELIB_FILTERTAGS_H
#define FRAMELIB_FILTERTAGS_H

#include "FrameLib_DSP.h"

class FrameLib_FilterTags : public FrameLib_Processor
{
    const static int maxNumFilters = 10;
    
    // Parameter Enums and Info

    enum ParameterList { kNumFilters, kFilters };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_FilterTags(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    
    // Info
    
    std::string objectInfo(bool verbose);
    std::string inputInfo(unsigned long idx, bool verbose);
    std::string outputInfo(unsigned long idx, bool verbose);
    
private:
    
    // Filter
    
    bool filter(FrameLib_Parameters::Serial::Iterator &it);
    
    // Process
    
    void process();
    
    // Data
    
    unsigned long mNumFilters;
    
    static ParameterInfo sParamInfo;
};

#endif
