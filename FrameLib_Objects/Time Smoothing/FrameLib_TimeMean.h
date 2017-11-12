
#ifndef FrameLib_TimeMean_H
#define FrameLib_TimeMean_H

#include "FrameLib_TimeBuffer_Template.h"

class FrameLib_TimeMean : public FrameLib_TimeBuffer<FrameLib_TimeMean>
{
    
public:
    
    // Constructor
    
    FrameLib_TimeMean(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);

    ~FrameLib_TimeMean()
    {
        dealloc(mSum);
        dealloc(mCompensate);
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
    
    double *mSum;
    double *mCompensate;    
};

#endif
