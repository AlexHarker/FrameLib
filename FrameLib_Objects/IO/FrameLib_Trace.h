

#ifndef FRAMELIB_TRACE_H
#define FRAMELIB_TRACE_H

#include "FrameLib_DSP.h"

class FrameLib_Trace : public FrameLib_AudioOutput, private FrameLib_Info
{
    // Paramter Enums and Info
    
    enum ParameterList { kLength, kUnits };
    enum Units { kMS, kSeconds, kSamples };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };
    
public:
    
    // Constructor / Destructor
    
    FrameLib_Trace(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    ~FrameLib_Trace();
    
    // Info
    
    const char *objectInfo(bool verbose);
    const char *inputInfo(unsigned long idx, bool verbose);
    const char *audioInfo(unsigned long idx, bool verbose);
    
private:
    
    // Helpers
    
    void copyAndZero(double *output, unsigned long offset, unsigned long size);
    void writeToBuffer(double *input, unsigned long offset, unsigned long size);
    
    // Object Reset, Block Process and Process
    
    void objectReset();
    void blockProcess(double **ins, double **outs, unsigned long vecSize);
    void process();

    // Data
    
    double *mBuffer;
    bool *mFlags;
    double mLastValue;
    unsigned long mSize;
    unsigned long mCounter;
    
    static ParameterInfo sParamInfo;
};

#endif
