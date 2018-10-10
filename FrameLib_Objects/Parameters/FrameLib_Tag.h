
#ifndef FRAMELIB_TAG_H
#define FRAMELIB_TAG_H

#include "FrameLib_DSP.h"

class FrameLib_Tag final : public FrameLib_Processor
{
    typedef FrameLib_Parameters::Serial Serial;

    const static int maxNumIns = 10;
    
    // Parameter Enums and Info

    enum ParameterList { kNumIns, kEmptyMode, kNames };
    enum EmptyMode { kIgnore, kReset };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_Tag(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;
    
private:
    
    // Process
    
    void process() override;
    
    // Data
    
    unsigned long mNumIns;
    
    static ParameterInfo sParamInfo;
};

#endif
