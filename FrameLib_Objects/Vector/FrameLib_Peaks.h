
#ifndef FRAMELIB_PEAKS_H
#define FRAMELIB_PEAKS_H

#include "FrameLib_DSP.h"

class FrameLib_Peaks final : public FrameLib_Processor
{

public:
    
    // Constructor
    
    FrameLib_Peaks(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;

private:
    
    // Helpers
    
    double logValue(double val);
    void refinePeak(double& pos, double& amp, double posUncorrected, double vm1, double v0, double vp1);
    
    // Process
    
    void process() override;
};

#endif
