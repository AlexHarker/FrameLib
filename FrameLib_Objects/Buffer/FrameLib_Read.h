
#ifndef FRAMELIB_READ_H
#define FRAMELIB_READ_H

#include "FrameLib_DSP.h"
#include "../../FrameLib_Dependencies/TableReader.hpp"

// FIX - consider adding anti-alising later....

class FrameLib_Read final : public FrameLib_Processor
{
    // Parameter Info and Enums
    
    enum ParameterList { kBuffer, kChannel, kUnits, kInterpolation, kEdges, kBound };
    enum Interpolation { kNone, kLinear, kHermite, kBSpline, kLagrange };
    enum Units { kSamples, kMS, kSeconds, kNormalised };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    struct Proxy : virtual FrameLib_Proxy
    {
        // Called when the buffer name changes
        
        virtual void update(const char *name) = 0;
        
        // Called prior and after reading
        
        // acquire - should return the length in samples (or zero for an invalid buffer) / sampling rate should be set to that of the buffer if one exists
        
        virtual void acquire(unsigned long& length, double& samplingRate) = 0;
        virtual void release() = 0;
        
        // Read
        
        virtual void read(double *output, const double *positions, unsigned long size, long chan, InterpType interp, EdgeType edges, bool bound) = 0;
        
        // Clone (we need unique instances per object for threading reasons)
        
        virtual Proxy *clone() const = 0;
    };
    
    // Constructor
    
    FrameLib_Read(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;

private:
    
    // Process
    
    void update() override;
    void process() override;
    
    // Data
    
    long mChan;
    Interpolation mInterpolation;
    Units mUnits;
    std::unique_ptr<Proxy> mProxy;

    static ParameterInfo sParamInfo;
};

#endif
