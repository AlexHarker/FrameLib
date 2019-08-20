
#ifndef FRAMELIB_ROUTE_H
#define FRAMELIB_ROUTE_H

#include "FrameLib_DSP.h"

class FrameLib_Route : public FrameLib_Block
{
    // Parameter Enums and Info
    
    enum ParameterList { kNumOuts, kActiveOut };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };
    
    // Internal Valve Class
    
    class Valve final : public FrameLib_Processor
    {
        // Parameter Enums and Info
        
        enum ParameterList { kActiveValve };
        
    public:
        
        // Constructor
        
        Valve(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy, long num);

    private:
        
        // Update and Process
        
        void update() override;
        void process() override;
        
        // Data
        
        long mValveNumber;
        long mActiveValve;
    };

public:
    
    // Constructor
    
    FrameLib_Route(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Object Type
    
    static ObjectType getType() { return kProcessor; }
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;
    
    // Connection Types
    
    FrameType inputType(unsigned long idx) const override   { return kFrameAny; }
    FrameType outputType(unsigned long idx) const override  { return kFrameAny; }
    
    // N.B. - Nothing can be acheived by setting a fixed input, so ignore this
    
    void setFixedInput(unsigned long idx, const double *input, unsigned long size) override {}
    const double *getFixedInput(unsigned long idx, unsigned long *size) override { return getEmptyFixedInput(idx, size); }

    // Audio Processing
    
    void blockUpdate(const double * const *ins, double **outs, unsigned long blockSize, FrameLib_AudioQueue& notifier) override {}
    void blockUpdate(const double * const *ins, double **outs, unsigned long blockSize) override {}
    void reset(double samplingRate, unsigned long maxBlockSize) override;
    
    const FrameLib_Parameters *getParameters() const override                                { return &mParameters; }

    // Ordering Connections
    
    void autoOrderingConnections() override {};
    void clearAutoOrderingConnections() override {};
    
private:
    
    // Data
    
    long mNumOuts;
    
    FrameLib_OwnedList<FrameLib_DSP> mValves;
    FrameLib_Parameters mParameters;
    static ParameterInfo sParamInfo;
};

#endif
