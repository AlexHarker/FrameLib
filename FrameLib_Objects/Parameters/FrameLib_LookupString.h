
#ifndef FRAMELIB_LOOKUPSTRING_H
#define FRAMELIB_LOOKUPSTRING_H

#include "FrameLib_DSP.h"

class FrameLib_LookupString final : public FrameLib_Processor
{
    typedef FrameLib_Parameters::Serial Serial;

    const static int maxNumItems = 32;
    
    // Parameter Enums and Info

    enum ParameterList { kNumItems, kTag, kEmptyMode, kItems };
    enum EmptyModes { kIgnore, kReset };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_LookupString(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;
    
private:
    
    // Process
    
    void process() override;
    
    // Data
    
    unsigned long mNumItems;
    
    static ParameterInfo sParamInfo;
};

#endif
