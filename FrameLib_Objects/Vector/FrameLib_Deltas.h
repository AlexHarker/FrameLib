
#ifndef FRAMELIB_DELTAS_H
#define FRAMELIB_DELTAS_H

#include "FrameLib_DSP.h"

class FrameLib_Deltas final : public FrameLib_Processor
{
    // Parameter Enums and Info

    enum ParameterList { kMode, kDirection, kFillPosition, kPadding };
    enum Modes { kPadIn, kPadOut, kWrap, kDrop };
    enum Directions { kForward, kBackward };
    enum PaddingLocations { kStart, kEnd };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_Deltas(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
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