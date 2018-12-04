
#ifndef FRAMELIB_ONEPOLEZERO_H
#define FRAMELIB_ONEPOLEZERO_H

#include "FrameLib_DSP.h"
#include "FrameLib_Filter_Constants.h"

// FIX - All filters to templates
// FIX - time varying params

class FrameLib_OnePoleZero final : public FrameLib_Processor
{
    // Filter Class

    class OnePoleZero
    {
        
    public:
        
        OnePoleZero() : a0(0.0), a1(0.0), r1(0.0) {}
        
        // Reset

        void reset()                { r1 = 0.0; }
        
        // Filter Types

        double HPF(double x)        { return x - calculateFilter(x); }
        double LPF(double x)        { return calculateFilter(x); }

        // Set Parameters
        
        void setParams(double freq, double samplingRate);
        
    private:
        
        // Filter Calculation

        double calculateFilter(double x);
        
        // Coefficients and Memories

        double a0, a1, r1;
    };
    
    // Parameter Enums and Info

    enum ParameterList { kFreq, kMode };
    enum Modes { kLPF, kHPF };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:

    // Constructor
    
    FrameLib_OnePoleZero(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;
    
private:
    
    // Process

    void process() override;
    
    // Data
    
    static ParameterInfo sParamInfo;
};

#endif
