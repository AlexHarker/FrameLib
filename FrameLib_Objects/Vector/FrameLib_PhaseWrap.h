
#ifndef FRAMELIB_PhaseWrap_H
#define FRAMELIB_PhaseWrap_H

#include "FrameLib_DSP.h"

class FrameLib_PhaseWrap final : public FrameLib_Processor
{
    
public:
	
    // Constructor

    FrameLib_PhaseWrap(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;

private:
    
    // Process
    
    void process() override;
};

#endif
