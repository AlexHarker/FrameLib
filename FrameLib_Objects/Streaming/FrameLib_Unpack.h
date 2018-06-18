
#ifndef FRAMELIB_UNPACK_H
#define FRAMELIB_UNPACK_H

#include "FrameLib_Multistream.h"

// FrameLib_Unpack - Unpack Multi-stream Signals

class FrameLib_Unpack final : public FrameLib_Multistream
{
    enum AtrributeList { kOutputs };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo() { add("Sets the number of outputs."); } };
    
public:
    
    // Constructor

    const FrameLib_Parameters::Serial *getSerialised() override { return &mSerialisedParameters; }
    
    FrameLib_Unpack(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy, unsigned long nStreams);
    
    // Set Fixed Inputs
    
    void setFixedInput(unsigned long idx, double *input, unsigned long size) override {};
    const double *getFixedInput(unsigned long idx, unsigned long *size) override { return getEmptyFixedInput(idx, size); }
    
    // Audio Processing
    
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
    
    virtual bool inputUpdate() override;
    
    // Data

    FrameLib_Parameters::AutoSerial mSerialisedParameters;
    
    FrameLib_Parameters mParameters;
    
    static ParameterInfo sParamInfo;
};

#endif
