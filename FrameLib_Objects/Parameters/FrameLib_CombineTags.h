
#ifndef FRAMELIB_COMBINETAGS_H
#define FRAMELIB_COMBINETAGS_H

#include "FrameLib_DSP.h"

class FrameLib_CombineTags final : public FrameLib_Processor
{
    static constexpr int maxNumIns = 10;
    
    // Parameter Enums and Info

    enum ParameterList { kNumIns };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_CombineTags(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;
    
private:
    
    // Process
    
    void process() override;
    
    // Data
    
    unsigned long mNumIns;
    
    static ParameterInfo sParamInfo;
};

#endif
