
#ifndef FRAMELIB_TIMER_H
#define FRAMELIB_TIMER_H

#include "FrameLib_DSP.h"

class FrameLib_Timer : public FrameLib_Processor
{
    // Parameter Enums and Info

    enum ParameterList { kUnits };
    enum Units { kSamples, kMS, kSeconds };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_Timer(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose);
    std::string inputInfo(unsigned long idx, bool verbose);
    std::string outputInfo(unsigned long idx, bool verbose);
    
private:
    
    // Calculate Multiplier
    
    void calculateMultiplier();
    
    // Object Reset
    
    void objectReset() { calculateMultiplier(); }
    
    // Update and Process
    
    void update();
    void process();
    
    // Data

    FrameLib_TimeFormat mMultiplier;

    static ParameterInfo sParamInfo;
};

#endif
