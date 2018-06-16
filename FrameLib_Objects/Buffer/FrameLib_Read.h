
#include "FrameLib_DSP.h"
#include "../../FrameLib_Dependencies/TableReader.hpp"

// FIX - consider adding anti-alising later....

class FrameLib_Read final : public FrameLib_Processor
{
    // Parameter Info and Enums
    
    enum ParameterList { kBuffer, kChannel, kMode, kUnits };
    enum Modes { kHermite, kBSpline, kLagrange, kLinear, kNone };
    enum Units { kMS, kSeconds, kSamples };

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
        
        virtual void read(double *output, const double *positions, unsigned long size, long chan, InterpType interpType) = 0;
    };
    
    // Constructor
    
    FrameLib_Read(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;

private:
    
    // Process
    
    void update() override { if (mProxy) mProxy->update(mParameters.getString(kBuffer)); }
    void process() override;
    
    // Data
    
    long mChan;
    Modes mMode;
    Units mUnits;
    Proxy *mProxy;

    static ParameterInfo sParamInfo;
};
