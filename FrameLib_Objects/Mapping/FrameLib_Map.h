
#ifndef FRAMELIB_MAP_H
#define FRAMELIB_MAP_H

#include "FrameLib_DSP.h"
#include "FrameLib_Scaling_Functions.h"

class FrameLib_Map final : public FrameLib_Processor
{
    // Parameter Enums and Info

    enum ParameterList { kMode, kInLo, kInHi, kOutLo, kOutHi, kClip, kExponent };
    enum Modes { kLinear, kLog, kExp, kPow, kDB, kInvDB, kTranspose, kInvTranspose };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };
    
public:
    
    // Constructor

    FrameLib_Map(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;

private:
    
    // Helpers
    
    void setScaling();
    
    // Update and Process
    
    void update() override;
    void process() override;
    
    // Data
        
    static ParameterInfo sParamInfo;
    
    FrameLib_VariClipScaler mScaler;
};

#endif
