
#ifndef FRAMELIB_MEDIANFILTER_H
#define FRAMELIB_MEDIANFILTER_H

#include "FrameLib_DSP.h"

class FrameLib_MedianFilter : public FrameLib_Processor
{
    // Parameter Enums and Info

    enum ParameterList { kWidth, kPadding, kMode };
    enum Modes { kPad, kWrap, kFold };
    
public:
    
    // Constructor
    
    FrameLib_MedianFilter(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    
private:
    
    // Helpers
    
    double insertMedian(double *temp, unsigned long *indices, double value, long index, long width);
    double median(double *temp, unsigned long *indices, long width);
    
    double getPad(double *input, long index, long sizeIn, long width, double padValue);
    double getWrap(double *input, long index, long sizeIn, long width);
    double getFold(double *input, long index, long sizeIn, long width);
    
    // Process
    
    void process();
};

#endif
