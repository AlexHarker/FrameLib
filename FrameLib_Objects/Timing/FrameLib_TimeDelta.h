
#ifndef FRAMELIB_TIMEDELTA_H
#define FRAMELIB_TIMEDELTA_H

#include "FrameLib_DSP.h"

class FrameLib_TimeDelta : public FrameLib_Processor
{
    // Parameter Enums and Info

    enum ParameterList { kUnits };
    enum Units { kSamples, kMS, kSeconds };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_TimeDelta(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose);
    std::string inputInfo(unsigned long idx, bool verbose);
    std::string outputInfo(unsigned long idx, bool verbose);
    
private:
    
    // Calculate Multiplier
    
    void calculateMultiplier();
    
    // Object Reset
    
    void objectReset();
    
    // Update and Process
    
    void update();
    void process();
    
    // Data

    FrameLib_TimeFormat mMultiplier;
    FrameLib_TimeFormat mLastTime;
    
    static ParameterInfo sParamInfo;
};

#endif
