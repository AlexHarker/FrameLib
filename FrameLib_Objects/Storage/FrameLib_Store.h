
#ifndef FRAMELIB_STORE_H
#define FRAMELIB_STORE_H

#include "FrameLib_DSP.h"

class FrameLib_Store final : public FrameLib_Processor
{
    // Parameter Enums and Info

    enum ParameterList { kName };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:

    // Constructor

    FrameLib_Store(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;
    
    // Stream Awareness
    
    void setStream(void *streamOwner, unsigned long stream) override;
    
private:
    
    // Object Reset and Process
    
    void objectReset() override;
    void process() override;
    
    // Data
    
    FrameLib_ContextAllocator::StoragePtr mStorage;
    
    static ParameterInfo sParamInfo;
    
    std::string mName;
};

#endif
