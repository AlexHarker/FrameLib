
#ifndef FRAMELIB_SALLENANDKEY_H
#define FRAMELIB_SALLENANDKEY_H

#include "FrameLib_DSP.h"
#include "FrameLib_Filter_Constants.h"

// FIX - All filters to templates / time varying params

class FrameLib_SallenAndKey final : public FrameLib_Processor
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

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:

    // Constructor
    
    FrameLib_SallenAndKey(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
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
