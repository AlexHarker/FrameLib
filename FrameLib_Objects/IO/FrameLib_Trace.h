

#ifndef FRAMELIB_TRACE_H
#define FRAMELIB_TRACE_H

#include "FrameLib_DSP.h"
#include "Framelib_IO_Helper.h"
#include <vector>

class FrameLib_Trace final : public FrameLib_AudioOutput, private FrameLib_IO_Helper
{
    // Paramter Enums and Info

    enum ParameterList { kMode, kBufferSize, kUnits, kPosition, kDelay };
    enum Modes { kFull, kFirst, kLast, kSpecified, kRatio };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };
    
public:
    
    // Constructor
    
    FrameLib_Trace(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string audioInfo(unsigned long idx, bool verbose) override;
    
private:
    
    // Helpers
    
    unsigned long bufferSize() const { return static_cast<unsigned long>(mBuffer.size()); }

    unsigned long convertTimeToSamples(double time);

    void copyAndZero(double *output, unsigned long offset, unsigned long size);
    void writeToBuffer(const double *input, unsigned long offset, unsigned long size);
    
    // Object Reset, Block Process and Process
    
    void objectReset() override;
    void blockProcess(const double * const *ins, double **outs, unsigned long blockSize) override;
    void process() override;

    // Data
    
    std::vector<double> mBuffer;
    std::vector<bool> mFlags;
    double mLastValue;
    unsigned long mCounter;
    
    static ParameterInfo sParamInfo;
};

#endif
