
#ifndef FRAMELIB_INFO_H
#define FRAMELIB_INFO_H

#include "FrameLib_DSP.h"
#include "../../FrameLib_Dependencies/TableReader.hpp"

class FrameLib_Info final : public FrameLib_Processor
{
    // Parameter Info and Enums
    
    enum ParameterList { kBuffer, kUnits };
    enum Units { kMS, kSeconds, kSamples };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };
    
public:
    
    struct Proxy : virtual FrameLib_Proxy
    {
        // Called when the buffer name changes
        
        virtual void update(const char *name) = 0;
        
        // Called prior and after reading
        
        virtual void acquire(unsigned long& length, double& samplingRate, unsigned long& chans) = 0;
        virtual void release() = 0;
        
        // Clone (we need unique instances per object for threading reasons)
        
        virtual Proxy *clone() const = 0;
    };
    
    // Constructor
    
    FrameLib_Info(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;
    
private:
    
    // Process
    
    void update() override;
    void process() override;
    
    // Data
    
    std::unique_ptr<Proxy> mProxy;
    Units mUnits;
    
    static ParameterInfo sParamInfo;
};

#endif
