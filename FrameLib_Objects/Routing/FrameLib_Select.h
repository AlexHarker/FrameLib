
#ifndef FRAMELIB_SELECT_H
#define FRAMELIB_SELECT_H

#include "FrameLib_DSP.h"

class FrameLib_Select final : public FrameLib_Processor
{
    // Parameter Enums and Info

    enum ParameterList { kNumIns, kActiveIn };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_Select(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;
    
private:
    
    // Update and Process
    
    void update() override;
    void process() override;
    
    // Data
    
    long mNumIns;
    long mActiveIn;
    
    // Data
    
    static ParameterInfo sParamInfo;
};

#endif
