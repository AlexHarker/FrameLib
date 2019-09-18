
#ifndef FRAMELIB_0DFSVF_H
#define FRAMELIB_0DFSVF_H

#include "FrameLib_DSP.h"
#include "FrameLib_Filter_Constants.h"

// FIX - All filters to templates
// FIX - time varying params

class FrameLib_0dfSVF final : public FrameLib_Processor
{
    // Filter Class
    
    class ZeroDelayFeedbackSVF
    {
        
        // Thanks to Oli Larkin for this code
        
    public:
        
        ZeroDelayFeedbackSVF() : lp(0.0), bp(0.0), hp(0.0), r(0.0), g(0.0), s1(0.0), s2(0.0) {}
        
        // Reset
        
        void reset()            { s1 = s2 = 0.0; }
        
        // Filter Types
        
        double HPF(double x);
        double BPF(double x);
        double LPF(double x);

        // Set Parameters
        
        void setParams(double freq, double resonance, double samplingRate);
        
    private:
        
        // Filter Calculation
        
        void calculateFilter(double x);
        
        // Coefficients and Memories
        
        double lp, bp, hp, r, g, s1, s2;
    };
    
    // Parameter Enums and Info

    enum ParameterList { kFreq, kReson, kMode };
    enum Modes { kLPF, kBPF, kHPF };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:

    FrameLib_0dfSVF(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
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
