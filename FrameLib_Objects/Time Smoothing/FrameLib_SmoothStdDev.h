
#ifndef FRAMELIB_SMOOTHSTDDEV_H
#define FRAMELIB_SMOOTHSTDDEV_H

#include "FrameLib_Time_Smoothing_Template.h"

class FrameLib_SmoothStdDev : public FrameLib_TimeSmoothing<FrameLib_SmoothStdDev>
{
    // Parameter Enums and Info

    enum ParameterList { kUnits };
    enum Units { kSamples, kMS, kSeconds };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_SmoothStdDev(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);

    ~FrameLib_SmoothStdDev()
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
