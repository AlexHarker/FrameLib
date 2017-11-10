
#ifndef FRAMELIB_LAG_H
#define FRAMELIB_LAG_H

#include "FrameLib_Time_Smoothing_Template.h"

class FrameLib_Lag : public FrameLib_TimeSmoothing<FrameLib_Lag>
{
    // Parameter Enums and Info

    enum ParameterList { kUnits };
    enum Units { kSamples, kMS, kSeconds };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_Lag(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);

    ~FrameLib_Lag()
    {
        dealloc(mFrameCopy);
    }
    
    // Info
    
    std::string objectInfo(bool verbose);
    std::string inputInfo(unsigned long idx, bool verbose);
    std::string outputInfo(unsigned long idx, bool verbose);
    
    // Smooth

    void resetSize(unsigned long size);

    virtual void add(const double *newFrame, unsigned long size) {}
    virtual void remove(const double *oldFrame, unsigned long size);
    virtual void result(double *output, unsigned long size);
    
private:
    
    // Object Reset
    
    void objectReset() { smoothReset(); }
    
    // Data
    
    static ParameterInfo sParamInfo;
    
    double *mFrameCopy;
};

#endif
