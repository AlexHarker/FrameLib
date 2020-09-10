#ifndef FrameLib_NanFilter_h
#define FrameLib_NanFilter_h

#include "FrameLib_DSP.h"

class FrameLib_NanFilter final : public FrameLib_Processor
{
    // Parameter Enums and Info

    enum ParameterList { kMode, kValue };
    enum Modes { kReplace, kRemove };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };
    
public:
    
    // Constructor
    
    FrameLib_NanFilter(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
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

#endif /* FrameLib_NanFilter_h */
