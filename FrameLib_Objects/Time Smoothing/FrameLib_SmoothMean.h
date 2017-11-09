
#ifndef FRAMELIB_SMOOTHMEAN_H
#define FRAMELIB_SMOOTHMEAN_H

#include "FrameLib_Time_Smoothing_Template.h"

class FrameLib_SmoothMean : public FrameLib_TimeSmoothing<FrameLib_SmoothMean>
{
    // Parameter Enums and Info

    enum ParameterList { kUnits };
    enum Units { kSamples, kMS, kSeconds };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_SmoothMean(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);

    ~FrameLib_SmoothMean()
    {
        dealloc(mSum);
        dealloc(mCompensate);
    }

    // Info
    
    std::string objectInfo(bool verbose);
    std::string inputInfo(unsigned long idx, bool verbose);
    std::string outputInfo(unsigned long idx, bool verbose);
    
    // Smooth

    void resetSize(unsigned long size);

    void smooth(double *output, const double *newFrame, const double *oldestFrame, unsigned long size);
    
private:
    
    // Object Reset
    
    void objectReset() { smoothReset(); }
    
    // Data
    
    static ParameterInfo sParamInfo;
    
    double *mSum;
    double *mCompensate;    
};

#endif
