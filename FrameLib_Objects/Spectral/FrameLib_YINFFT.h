
#ifndef FRAMELIB_YINFFT_H
#define FRAMELIB_YINFFT_H

#include "FrameLib_DSP.h"

class FrameLib_YINFFT final : public FrameLib_Processor
{
    enum ParameterList { kMaxLength, kMinFrequency, kMaxFrequency };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_YINFFT(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    ~FrameLib_YINFFT() override;
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;
    
private:
    
    // Process
    
    void process() override;
    
    // Data
    
    static ParameterInfo sParamInfo;
    struct Implementation;
    std::unique_ptr<Implementation> mImplementation;
    unsigned long mMaxLength;
};

#endif
