
#ifndef _##MAX_CLASS_NAME_UPPER##__H
#define _##MAX_CLASS_NAME_UPPER##__H

#include "FrameLib_DSP.h"

class _##OBJECT_CLASS##_ final : public FrameLib_Processor
{
    enum ParameterList { };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    _##OBJECT_CLASS##_(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
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
