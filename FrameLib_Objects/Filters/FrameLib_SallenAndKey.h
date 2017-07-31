
#ifndef FRAMELIB_SALLENANDKEY_H
#define FRAMELIB_SALLENANDKEY_H

#include "FrameLib_DSP.h"
#include "FrameLib_Filter_Constants.h"

// FIX - All filters to templates
// FIX - time varying params

class FrameLib_SallenAndKey : public FrameLib_Processor
{
    // Filter Class

    class SallenAndKey
    {
        
    public:
        
        SallenAndKey() : k(0.0), a0(0.0), a1(0.0), a2(0.0), a3(0.0), a4(0.0), a5(0.0), v1(0.0), v2(0.0), ic1eq(0.0), ic2eq(0.0) {}
        
        // Reset

        void reset()            { ic1eq = ic2eq = 0.0; }

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

        double k, a0, a1, a2, a3, a4, a5, v1, v2, ic1eq, ic2eq;
    };
    
    // Parameter Enums and Info

    enum ParameterList { kFreq, kReson, kMode };
    enum Modes { kLPF, kBPF, kHPF };

public:
	
    // Constructor
    
    FrameLib_SallenAndKey(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    
private:
    
    // Process
    
    void process();
};

#endif
