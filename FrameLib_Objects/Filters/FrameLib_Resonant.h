
#ifndef FRAMELIB_RESONANT_H
#define FRAMELIB_RESONANT_H

#include "FrameLib_DSP.h"
#include "FrameLib_Filter_Constants.h"

// FIX - All filters to templates / time varying params

class FrameLib_Resonant final : public FrameLib_Processor
{
    // Filter Class

    class Resonant
    {
        
    public:
        
        Resonant() : scl(0.0), r2(0.0), y1(0.0), y2(0.0) {}
        
        // Reset
        
        void reset()                { y1 = y2 = 0.0; }
        
        // Filter Types
        
        double HPF(double x)        { return x - calculateFilter(x); }
        double LPF(double x)        { return calculateFilter(x); }
        
        // Set Parameters
        
        void setParams(double freq, double reson, double samplingRate);
        
    private:
        
        // Filter Calculation

        double calculateFilter(double x);
        
        // Coefficients and Memories

        double scl, r2, y1, y2;
    };
    
    // Parameter Enums and Info

    enum ParameterList { kFreq, kReson, kMode };
    enum Modes { kLPF, kHPF };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:

    // Constructor
    
    FrameLib_Resonant(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
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
