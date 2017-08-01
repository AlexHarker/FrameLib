

#ifndef FRAMELIB_TRACE_H
#define FRAMELIB_TRACE_H

#include "FrameLib_DSP.h"
#include <vector>

class FrameLib_Trace : public FrameLib_AudioOutput
{
    // Paramter Enums and Info
    
    enum ParameterList { kLength, kUnits };
    enum Units { kMS, kSeconds, kSamples };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };
    
public:
    
    // Constructor
    
    FrameLib_Trace(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    
    // Info
    
    std::string objectInfo(bool verbose);
    std::string inputInfo(unsigned long idx, bool verbose);
    std::string audioInfo(unsigned long idx, bool verbose);
    
private:
    
    // Helpers
    
    size_t bufferSize() const {return mBuffer.size(); }

    void copyAndZero(double *output, unsigned long offset, unsigned long size);
    void writeToBuffer(double *input, unsigned long offset, unsigned long size);
    
    // Object Reset, Block Process and Process
    
    void objectReset();
    void blockProcess(double **ins, double **outs, unsigned long vecSize);
    void process();

    // Data
    
    std::vector<double> mBuffer;
    std::vector<bool> mFlags;
    double mLastValue;
    unsigned long mCounter;
    
    static ParameterInfo sParamInfo;
};

#endif
