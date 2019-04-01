#ifndef FRAMELIB_CLOSEST_H
#define FRAMELIB_CLOSEST_H

#include "FrameLib_DSP.h"

class FrameLib_Closest final : public FrameLib_Processor
{
    enum ParameterList {kMatches};

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:

    // Constructor

    FrameLib_Closest(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
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
