
#ifndef FRAMELIB_MEDIANFILTER_H
#define FRAMELIB_MEDIANFILTER_H

#include "FrameLib_DSP.h"

class FrameLib_MedianFilter : public FrameLib_Processor
{
    // Parameter Enums and Info

    enum ParameterList { kWidth, kPadding, kMode };
    enum Modes { kPad, kWrap, kFold };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_MedianFilter(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose);
    std::string inputInfo(unsigned long idx, bool verbose);
    std::string outputInfo(unsigned long idx, bool verbose);

private:
    
    // Helpers
    
    double insertMedian(double *temp, unsigned long *indices, double value, long index, long width);
    double median(double *temp, unsigned long *indices, long width);
    
    double getPad(const double *input, long index, long sizeIn, double padValue);
    double getWrap(const double *input, long index, long sizeIn);
    double getFold(const double *input, long index, long sizeIn);
    
    // Process
    
    void process();
    
    // Data
    
    static ParameterInfo sParamInfo;
};

#endif
