
#ifndef FRAMELIB_MEDIANFILTER_H
#define FRAMELIB_MEDIANFILTER_H

#include "FrameLib_DSP.h"

class FrameLib_MedianFilter final : public FrameLib_Processor
{
    // Parameter Enums and Info

    enum ParameterList { kWidth, kPadding, kMode };
    enum Modes { kPad, kWrap, kFold };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_MedianFilter(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;

private:
    
    // Helpers
    
    double insertMedian(double *temp, unsigned long *indices, double value, long index, long width);
    double median(double *temp, unsigned long *indices, long width);
    
    double getPad(const double *input, long index, long sizeIn, double padValue);
    double getWrap(const double *input, long index, long sizeIn);
    double getFold(const double *input, long index, long sizeIn);
    
    // Process
    
    void process() override;
    
    // Data
    
    static ParameterInfo sParamInfo;
};

#endif
