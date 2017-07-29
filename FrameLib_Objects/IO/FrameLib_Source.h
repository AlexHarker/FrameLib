

#ifndef FRAMELIB_SOURCE_H
#define FRAMELIB_SOURCE_H

#include "FrameLib_DSP.h"
#include <vector>

class FrameLib_Source : public FrameLib_AudioInput, private FrameLib_Info
{
    // Parameter Enums and Info

    enum ParameterList { kMaxLength, kLength, kUnits };
    enum Units { kSamples, kMS, kSeconds };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };
    
public:
    
    // Constructor
    
    FrameLib_Source(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    
    // Info
    
    const char *objectInfo(bool verbose);
    const char *inputInfo(unsigned long idx, bool verbose);
    const char *outputInfo(unsigned long idx, bool verbose);
    const char *audioInfo(unsigned long idx, bool verbose);
    
private:
    
    // Helpers
    
    size_t bufferSize() const {return mBuffer.size(); }
    
    unsigned long convertTimeToSamples(double time);
    void copy(double *input, unsigned long offset, unsigned long size);
    
    // Object Reset, Block Process and Process
    
    void objectReset();
    void blockProcess(double **ins, double **outs, unsigned long vecSize);
    void process();
    
    // Data
    
    std::vector<double> mBuffer;
//    double *mBuffer;
  //  unsigned long mSize;
    unsigned long mLength;
    unsigned long mCounter;
    
    static ParameterInfo sParamInfo;
};

#endif
