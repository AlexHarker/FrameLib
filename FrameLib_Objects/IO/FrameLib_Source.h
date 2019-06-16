

#ifndef FRAMELIB_SOURCE_H
#define FRAMELIB_SOURCE_H

#include "FrameLib_DSP.h"
#include <vector>

class FrameLib_Source final : public FrameLib_AudioInput
{
    // Parameter Enums and Info

    enum ParameterList { kBufferSize, kLength, kUnits, kDelay };
    enum Units { kSamples, kMS, kSeconds };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };
    
public:
    
    // Constructor
    
    FrameLib_Source(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;
    std::string audioInfo(unsigned long idx, bool verbose) override;
    
private:
    
    // Helpers
    
    unsigned long bufferSize() const { return truncToUInt(mBuffer.size()); }
    
    unsigned long convertTimeToSamples(double time);
    
    void copy(const double *input, unsigned long offset, unsigned long size);
    
    // Object Reset, Block Process, Update and Process 
    
    void objectReset() override;
    void blockProcess(const double * const *ins, double **outs, unsigned long blockSize) override;
    void update() override;
    void process() override;
    
    // Data
    
    std::vector<double> mBuffer;
    unsigned long mLength;
    unsigned long mCounter;
    
    static ParameterInfo sParamInfo;
};

#endif
