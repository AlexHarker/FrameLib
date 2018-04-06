
#ifndef FRAMELIB_ONEPOLE_H
#define FRAMELIB_ONEPOLE_H

#include "FrameLib_DSP.h"
#include "FrameLib_Filter_Constants.h"

// FIX - All filters to templates
// FIX - time varying params

class FrameLib_OnePole : public FrameLib_Processor
{
    // Filter Class

    class OnePole
    {
        
    public:
        
        OnePole() : f0(0.0), y1(0.0) {}
        
        // Reset

        void reset()            { y1 = 0.0; }

        // Filter Types
        
        double HPF(double x)    { return x - calculateFilter(x); }
        double LPF(double x)    { return calculateFilter(x); }
        
        // Set Parmaeters
        
        void setParams(double freq, double samplingRate)    { f0 = sin((freq * FILTER_TWO_PI) / samplingRate); }
        
    private:
        
        // Filter Calculation
        
        double calculateFilter(double x);
        
        // Coefficients and Memories

        double f0, y1;
    };
    
    // Parameter Enums and Info

    enum ParameterList { kFreq, kMode };
    enum Modes { kLPF, kHPF };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
	
    // Constructor
    
    FrameLib_OnePole(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose);
    std::string inputInfo(unsigned long idx, bool verbose);
    std::string outputInfo(unsigned long idx, bool verbose);
    
private:
    
    // Process
    
    void process();
    
    // Data
    
    static ParameterInfo sParamInfo;
};

#endif
