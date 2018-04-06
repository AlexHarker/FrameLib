

#ifndef FRAMELIB_TRACE_H
#define FRAMELIB_TRACE_H

#include "FrameLib_DSP.h"
#include <vector>

class FrameLib_Trace : public FrameLib_AudioOutput
{
    // Paramter Enums and Info

    enum ParameterList { kMode, kBufferSize, kUnits, kPosition, kDelay };
    enum Modes { kFull, kFirst, kLast, kSpecified, kRatio };
    enum Units { kSamples, kMS, kSeconds };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };
    
public:
    
    // Constructor
    
    FrameLib_Trace(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose);
    std::string inputInfo(unsigned long idx, bool verbose);
    std::string audioInfo(unsigned long idx, bool verbose);
    
private:
    
    // Helpers
    
    size_t bufferSize() const { return mBuffer.size(); }

    unsigned long convertTimeToSamples(double time);

    void copyAndZero(double *output, unsigned long offset, unsigned long size);
    void writeToBuffer(const double *input, unsigned long offset, unsigned long size);
    
    // Object Reset, Block Process and Process
    
    void objectReset();
    void blockProcess(const double * const *ins, double **outs, unsigned long blockSize);
    void process();

    // Data
    
    std::vector<double> mBuffer;
    std::vector<bool> mFlags;
    double mLastValue;
    unsigned long mCounter;
    
    static ParameterInfo sParamInfo;
};

#endif
