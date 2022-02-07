
#ifndef FRAMELIB_DISPATCH_H
#define FRAMELIB_DISPATCH_H

#include "FrameLib_DSP.h"

class FrameLib_Dispatch : public FrameLib_Block
{
    // Parameter Enums and Info
    
    enum ParameterList { kNumIns, kNumOuts, kActiveIn1 };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

    // Internal valve class
    
    class Select final : public FrameLib_Processor
    {
        // Parameter Enums and Info
        
        enum ParameterList { kActiveIn };
        
    public:
        
        // Constructor
        
        Select(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy, long numIns, long num);

    private:
        
        // Update and Process
        
        void update() override;
        void process() override;
        
        // Data
        
        long mNumIns;
        long mActiveIn;
    };
    
    // Internal parameter class
    
    struct Parameters final : public FrameLib_Processor
    {
        Parameters(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
        
        FrameLib_Parameters& parameters()               { return mParameters; }
        const FrameLib_Parameters& parameters() const   { return mParameters; }

        void process() override {}
    };
    
public:
    
    static constexpr ObjectType sType = ObjectType::Processor;
    static constexpr bool sHandlesAudio = false;
    
    // Constructor
    
    FrameLib_Dispatch(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;
    
    // Connection Types
    
    FrameType inputType(unsigned long idx) const override       { return idx == getNumIns() - 1 ? FrameType::Tagged : FrameType::Any; }
    FrameType outputType(unsigned long idx) const override      { return FrameType::Any; }
    
    // N.B. - Nothing can be acheived by setting a fixed input, so ignore this
    
    void setFixedInput(unsigned long idx, const double *input, unsigned long size) override {}
    const double *getFixedInput(unsigned long idx, unsigned long *size) override { return getEmptyFixedInput(idx, size); }

    // Audio Processing
    
    uint64_t getBlockTime() const override { return 0; }
    void blockUpdate(const double * const *ins, double **outs, unsigned long blockSize, FrameLib_AudioQueue& notifier) override {}
    void blockUpdate(const double * const *ins, double **outs, unsigned long blockSize) override {}
    void reset(double samplingRate, unsigned long maxBlockSize) override;
    
    const FrameLib_Parameters *getParameters() const override { return &mParameterObject.parameters(); }

    // Ordering Connections
    
    void autoOrderingConnections() override {};
    void clearAutoOrderingConnections() override {};
    
private:
    
    // Data

    long mNumIns;
    long mNumOuts;
    
    Parameters mParameterObject;
    FrameLib_OwnedList<FrameLib_DSP> mSelects;
    static ParameterInfo sParamInfo;
};

#endif
