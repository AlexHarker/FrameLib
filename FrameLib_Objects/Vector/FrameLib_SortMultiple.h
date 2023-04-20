
#ifndef FRAMELIB_SORTMULTIPLE_H
#define FRAMELIB_SORTMULTIPLE_H

#include "FrameLib_DSP.h"

class FrameLib_SortMultiple final : public FrameLib_Processor
{
    const static int maxNumIO = 10;

    // Parameter Enums and Info

    enum ParameterList { kNumIO, kSortInput, kOrder, kMismatch, kPadding };
    enum Orders { kUp, kDown };
    enum MismatchModes { kPad, kShrink, kExtend};

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_SortMultiple(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;

private:
    
    // Process

    void process() override;
    
    // Data
    
    static ParameterInfo sParamInfo;
    long mNumIO;
};

#endif
