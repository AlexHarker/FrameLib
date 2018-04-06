
#ifndef FRAMELIB_RECALL_H
#define FRAMELIB_RECALL_H

#include "FrameLib_DSP.h"

class FrameLib_Recall : public FrameLib_Processor
{
    // Parameter Enums and Info

    enum ParameterList { kName };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor / Destructor

    FrameLib_Recall(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    ~FrameLib_Recall();
    
    // Info
    
    std::string objectInfo(bool verbose);
    std::string inputInfo(unsigned long idx, bool verbose);
    std::string outputInfo(unsigned long idx, bool verbose);
    
    // Stream Awareness
    
    virtual void setStream(unsigned long stream);

private:
    
    // Process

    void process();
    
    // Data
    
    FrameLib_LocalAllocator::Storage *mStorage;
        
    static ParameterInfo sParamInfo;
    
    std::string mName;
};

#endif
