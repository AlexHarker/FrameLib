
#ifndef FRAMELIB_TIMESTDDEV_H
#define FRAMELIB_TIMESTDDEV_H

#include "FrameLib_TimeBuffer_Template.h"

class FrameLib_TimeStdDev : public FrameLib_TimeBuffer<FrameLib_TimeStdDev>
{
    // Parameter Enums and Info

    enum ParameterList { kUnits };
    enum Units { kSamples, kMS, kSeconds };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_TimeStdDev(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);

    ~FrameLib_TimeStdDev()
    {
        dealloc(mSum);
        dealloc(mCompensate);
        dealloc(mSqSum);
        dealloc(mSqCompensate);
    }

    // Info
    
    std::string objectInfo(bool verbose);
    std::string inputInfo(unsigned long idx, bool verbose);
    std::string outputInfo(unsigned long idx, bool verbose);
    
    // Smooth

    void resetSize(unsigned long maxFrames, unsigned long size);

    virtual void add(const double *newFrame, unsigned long size);
    virtual void remove(const double *oldFrame, unsigned long size);
    virtual void result(double *output, unsigned long size);
    
private:
    
    // Object Reset
    
    void objectReset() { smoothReset(); }
    
    // Data
    
    static ParameterInfo sParamInfo;
    
    double *mSum;
    double *mCompensate;
    double *mSqSum;
    double *mSqCompensate;
};

#endif
