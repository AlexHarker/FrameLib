

#ifndef FRAMELIB_SINK_H
#define FRAMELIB_SINK_H

#include "FrameLib_DSP.h"
#include <vector>

class FrameLib_Sink : public FrameLib_AudioOutput, private FrameLib_Info
{
    enum ParameterList { kLength, kUnits };
    enum Units { kMS, kSeconds, kSamples };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };
    
public:

    // Constructor
    
    FrameLib_Sink(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    
    // Info
    
    const char *objectInfo(bool verbose);
    const char *inputInfo(unsigned long idx, bool verbose);
    const char *audioInfo(unsigned long idx, bool verbose);
    
private:
    
    // Helpers
    
    size_t bufferSize() const {return mBuffer.size(); }

    void copyAndZero(double *output, unsigned long offset, unsigned long size);
    void addToBuffer(double *input, unsigned long offset, unsigned long size);
    
    // Object Reset, Block Process and Process

    void objectReset();
    void blockProcess(double **ins, double **outs, unsigned long vecSize);
    void process();
    
    // Data
    
    std::vector<double> mBuffer;
    unsigned long mCounter;
    
    static ParameterInfo sParamInfo;
};

#endif
