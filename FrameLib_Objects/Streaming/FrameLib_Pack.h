
#ifndef FRAMELIB_PACK_H
#define FRAMELIB_PACK_H

#include "FrameLib_Multistream.h"

// FrameLib_Pack - Pack Multi-stream Signals

class FrameLib_Pack final : public FrameLib_Multistream
{
    enum AtrributeList { kInputs };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo() { add("Sets the number of inputs."); } };
    
public:
    
    static constexpr ObjectType sType = kProcessor;
    static constexpr bool sHandlesAudio = false;
    
    const FrameLib_Parameters::Serial *getSerialised() override { return &mSerialisedParameters; }
    
    // Constructor

    FrameLib_Pack(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy, unsigned long nStreams);
    
    // Set Fixed Inputs
    
    void setFixedInput(unsigned long idx, const double *input, unsigned long size) override {};
    const double *getFixedInput(unsigned long idx, unsigned long *size) override { return getEmptyFixedInput(idx, size); }
    
    // Audio Processing
    
    uint64_t getBlockTime() const override { return 0; }
    void blockUpdate(const double * const *ins, double **outs, unsigned long blockSize, FrameLib_AudioQueue& notifier) override {}
    void blockUpdate(const double * const *ins, double **outs, unsigned long blockSize) override {}
    void reset(double samplingRate, unsigned long maxBlockSize) override {}
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;
    
    const FrameLib_Parameters *getParameters() const override { return &mParameters; }
    
    // Connection Types

    FrameType inputType(unsigned long idx) const override { return kFrameAny; }
    FrameType outputType(unsigned long idx) const override { return kFrameAny; }
    
    // Auto Ordering Connections

    void autoOrderingConnections() override {}
    void clearAutoOrderingConnections() override {}
    
private:
    
    // Input Update

    bool inputUpdate() override;
    
    // Data
    
    FrameLib_Parameters::AutoSerial mSerialisedParameters;
    
    FrameLib_Parameters mParameters;
    
    static ParameterInfo sParamInfo;
};

#endif
