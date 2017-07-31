
#ifndef FRAMELIB_0DFSVF_H
#define FRAMELIB_0DFSVF_H

#include "FrameLib_DSP.h"
#include "FrameLib_Filter_Constants.h"

// FIX - All filters to templates
// FIX - time varying params

class FrameLib_0dfSVF : public FrameLib_Processor
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

public:
	
    FrameLib_0dfSVF(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    
private:
    
    // Process
    
    void process();
};

#endif
