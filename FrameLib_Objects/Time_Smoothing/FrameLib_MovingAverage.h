
#ifndef FRAMELIB_MOVINGAVERAGE_H
#define FRAMELIB_MOVINGAVERAGE_H

#include "FrameLib_DSP.h"
#include "FrameLib_PaddedVector.h"

class FrameLib_MovingAverage final : public FrameLib_Processor
{
    // Parameter Enums and Info
    
    enum ParameterList { kAlphaUp, kAlphaDown, kAverage, kDeviation };
    enum Modes { kUseDefault, kInputOnly };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor / Destructor
    
    FrameLib_MovingAverage(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    ~FrameLib_MovingAverage();
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;
    
private:

    // Object Reset
    
    void objectReset() override;
    
    // Process
    
    void process() override;
    
    // Data
    
    double *mAverageFrame;
    double *mVarianceFrame;
    unsigned long mFrameSize;
    
    FrameLib_TimeFormat mLastAvgResetTime;
    FrameLib_TimeFormat mLastDevResetTime;

    static ParameterInfo sParamInfo;
};

#endif
