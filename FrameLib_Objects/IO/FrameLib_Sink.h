

#ifndef FRAMELIB_SINK_H
#define FRAMELIB_SINK_H

#include "FrameLib_DSP.h"
#include <vector>

class FrameLib_Sink : public FrameLib_AudioOutput
{
    enum ParameterList { kBufferSize, kUnits, kDelay };
    enum Units { kSamples, kMS, kSeconds };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };
    
public:

    // Constructor
    
    FrameLib_Sink(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    
    // Info
    
    std::string objectInfo(bool verbose);
    std::string inputInfo(unsigned long idx, bool verbose);
    std::string audioInfo(unsigned long idx, bool verbose);
    
private:
    
    // Helpers
    
    size_t bufferSize() const { return mBuffer.size(); }

    unsigned long convertTimeToSamples(double time);

    void copyAndZero(double *output, unsigned long offset, unsigned long size);
    void addToBuffer(const double *input, unsigned long offset, unsigned long size);
    
    // Object Reset, Block Process and Process

    void objectReset();
    void blockProcess(const double * const *ins, double **outs, unsigned long blockSize);
    void process();
    
    // Data
    
    std::vector<double> mBuffer;
    unsigned long mCounter;

    static ParameterInfo sParamInfo;
};

#endif
