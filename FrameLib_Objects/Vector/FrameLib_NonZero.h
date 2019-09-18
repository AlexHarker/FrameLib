
#ifndef FRAMELIB_NONZERO_H
#define FRAMELIB_NONZERO_H

#include "FrameLib_DSP.h"

class FrameLib_NonZero final : public FrameLib_Processor
{
public:

    // Constructor

    FrameLib_NonZero(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;

private:
    
    // Process
    
    void process() override;
};

#endif
