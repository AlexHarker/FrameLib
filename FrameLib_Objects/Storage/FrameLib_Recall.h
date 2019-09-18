
#ifndef FRAMELIB_RECALL_H
#define FRAMELIB_RECALL_H

#include "FrameLib_DSP.h"

class FrameLib_Recall final : public FrameLib_Processor
{
    // Parameter Enums and Info

    enum ParameterList { kName };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor / Destructor

    FrameLib_Recall(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    ~FrameLib_Recall();
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;
    
    // Stream Awareness
    
    void setStream(void *streamOwner, unsigned long stream) override;

private:
    
    // Process

    void process() override;
    
    // Data
    
    FrameLib_ContextAllocator::Storage *mStorage;
        
    static ParameterInfo sParamInfo;
    
    std::string mName;
};

#endif
