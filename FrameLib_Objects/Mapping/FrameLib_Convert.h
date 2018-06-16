
#ifndef FRAMELIB_CONVERT_H
#define FRAMELIB_CONVERT_H

#include "FrameLib_DSP.h"
#include "FrameLib_Scaling_Functions.h"

class FrameLib_Convert final : public FrameLib_Processor, private FrameLib_ScaleConverter
{
    // Parameter Enums and Info

    enum ParameterList { kMode };
    enum Modes { kDBtoA, kAtoDB, kMtoF, kFtoM, kSemiToRatio, kRatioToSemi, kDegToRad, kRadToDeg };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };
        
public:
    
    // Constructor

    FrameLib_Convert(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
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
};

#endif
