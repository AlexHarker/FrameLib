
#ifndef FRAMELIB_SMOOTHMEDIAN_H
#define FRAMELIB_SMOOTHMEDIAN_H

#include "FrameLib_Time_Smoothing_Template.h"

class FrameLib_SmoothMedian : public FrameLib_TimeSmoothing<FrameLib_SmoothMedian>
{
    // Parameter Enums and Info

    enum ParameterList { kUnits };
    enum Units { kSamples, kMS, kSeconds };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_SmoothMedian(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);

    ~FrameLib_SmoothMedian()
    {
        dealloc(mOrdered);
    }

    // Info
    
    std::string objectInfo(bool verbose);
    std::string inputInfo(unsigned long idx, bool verbose);
    std::string outputInfo(unsigned long idx, bool verbose);
    
    // Smooth

    void resetSize(unsigned long size);

private:

    double *getChannel(unsigned long idx) const { return mOrdered + (idx * getMaxFrames()); }

    virtual void add(const double *newFrame, unsigned long size);
    virtual void remove(const double *oldFrame, unsigned long size);
    virtual void exchange(const double *newFrame, const double *oldFrame, unsigned long size);
    virtual void result(double *output, unsigned long size);
    
    // Object Reset
    
    void objectReset() { smoothReset(); }
    
    // Data
    
    static ParameterInfo sParamInfo;
    
    double *mOrdered;
    unsigned long mNumFrames;
};

#endif
