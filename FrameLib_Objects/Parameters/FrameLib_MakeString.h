
#ifndef FRAMELIB_MAKESTRING_H
#define FRAMELIB_MAKESTRING_H

#include "FrameLib_DSP.h"

class FrameLib_MakeString final : public FrameLib_Processor
{
    const static int maxNumItems = 10;
    
    // Parameter Enums and Info

    enum ParameterList { kNumItems, kTag, kTriggers, kPadding, kItems };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_MakeString(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;
    
private:
    
    // Input Number Helper

    long getInputNumber(unsigned long idx);
    
    // Update and Process
    
    void update() override;
    void process() override;
    
    // Data
    
    long mInputs[maxNumItems];
    unsigned long mNumItems;
    unsigned long mNumIns;
    
    static ParameterInfo sParamInfo;
};

#endif
