
#ifndef FRAMELIB_NOW_H
#define FRAMELIB_NOW_H

#include "FrameLib_DSP.h"

class FrameLib_Now final : public FrameLib_Processor
{
    // Parameter Enums and Info

    enum ParameterList { kUnits };
    enum Units { kSamples, kMS, kSeconds };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_Now(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;
    
private:
    
    // Calculate Multiplier
    
    void calculateMultiplier();
    
    // Object Reset
    
    void objectReset() override { calculateMultiplier(); }
    
    // Update and Process
    
    void update() override;
    void process() override;
    
    // Data

    FrameLib_TimeFormat mMultiplier;
    
    static ParameterInfo sParamInfo;
};

#endif
