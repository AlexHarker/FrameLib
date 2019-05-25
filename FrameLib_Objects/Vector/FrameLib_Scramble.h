
#ifndef FRAMELIB_SCRAMBLE_H
#define FRAMELIB_SCRAMBLE_H

#include "FrameLib_DSP.h"
#include <random>

class FrameLib_Scramble final : public FrameLib_Processor
{
    
public:

    // Constructor

    FrameLib_Scramble(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;

private:
    
    // Process
    void process() override;
    std::random_device rd;
    std::mt19937 g(rd());
};

#endif
