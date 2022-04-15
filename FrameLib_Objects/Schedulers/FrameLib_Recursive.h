
#ifndef FRAMELIB_RECURSIVE_H
#define FRAMELIB_RECURSIVE_H

#include "FrameLib_DSP.h"

class FrameLib_Recursive final : public FrameLib_Scheduler
{
public:
    
    class Time final : public FrameLib_Processor
    {
    public:
        
        // Constructor
        
        Time(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
        
        // Info
        
        std::string objectInfo(bool verbose) override;
        std::string inputInfo(unsigned long idx, bool verbose) override;
        
    private:
        
        // Object Reset
        
        void objectReset() override;
        
        // Process
        
        void process() override;
        
        // Data
        
        FrameLib_Recursive *mOwner;
    };
    
    // Parameter Enums and Info

    enum ParameterList { kUnits, kAutoStart };
    enum Units { kSamples, kMS, kSeconds, kHz };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_Recursive(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;
    
private:
    
    // Calculate Interval
    
    FrameLib_TimeFormat calculateInterval();
    
    // Object Reset
    
    void objectReset() override;
    
    // Schedule
    
    SchedulerInfo schedule(bool newFrame, bool noAdvance) override;
    
    // Data
    
    double mInterval;
    
    static ParameterInfo sParamInfo;
};

#endif
