
#ifndef FRAMELIB_TIMEMEDIAN_H
#define FRAMELIB_TIMEMEDIAN_H

#include "FrameLib_TimeBuffer_Template.h"

class FrameLib_TimeMedian : public FrameLib_TimeBuffer<FrameLib_TimeMedian>
{
   
public:
    
    // Constructor
    
    FrameLib_TimeMedian(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);

    ~FrameLib_TimeMedian()
    {
        dealloc(mOrdered);
    }

    // Info
    
    std::string objectInfo(bool verbose);
    std::string inputInfo(unsigned long idx, bool verbose);
    std::string outputInfo(unsigned long idx, bool verbose);
    
    // Smooth

    void resetSize(unsigned long maxFrames, unsigned long size);

private:

    double *getChannel(unsigned long idx) const { return mOrdered + (idx * getMaxFrames()); }

    virtual void add(const double *newFrame, unsigned long size);
    virtual void remove(const double *oldFrame, unsigned long size);
    virtual void exchange(const double *newFrame, const double *oldFrame, unsigned long size);
    virtual void result(double *output, unsigned long size);
    
    // Object Reset
    
    void objectReset() { smoothReset(); }
    
    // Data
        
    double *mOrdered;
    unsigned long mNumFrames;
};

#endif
