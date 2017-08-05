
#ifndef FRAMELIB_TIME_H
#define FRAMELIB_TIME_H

#include "FrameLib_DSP.h"

class FrameLib_Time : public FrameLib_Processor
{
    // Parameter Enums and Info

    enum ParameterList { kUnits };
    enum Units { kSamples, kMS, kSeconds };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_Time(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    
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
