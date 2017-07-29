
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
        
        inline double calculateFilter(double x);
        
        // Coefficients and Memories

        double f0, y1;
    };
    
    // Parameter Enums and Info

    enum ParameterList { kFreq, kMode };
    enum Modes { kLPF, kHPF };

public:
	
    // Constructor
    
    FrameLib_OnePole(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    
private:
    
    // Update and Process
    
    void update();
    void process();
};

#endif