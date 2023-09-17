

#ifndef FRAMELIB_SOURCE_H
#define FRAMELIB_SOURCE_H

#include "FrameLib_DSP.h"
#include "Framelib_IO_Helper.h"
#include <vector>

class FrameLib_Source final : public FrameLib_AudioInput, private FrameLib_IO_Helper
{
    // Parameter Enums and Info

    enum ParameterList { kBufferSize, kLength, kUnits, kDelay, kInterpolation };
    enum Interpolation { kNone, kLinear, kHermite, kBSpline, kLagrange };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };
    
    struct Fetcher : htl::table_fetcher<double>
    {
        Fetcher(const double *data, intptr_t size) : table_fetcher(size, 1.0), mData(data) {}
        
        double operator()(intptr_t offset) { return mData[offset < 0 ? offset + size : offset]; }
        
        const double *mData;
    };
    
public:
    
    // Constructor
    
    FrameLib_Source(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;
    std::string audioInfo(unsigned long idx, bool verbose) override;
    
private:
    
    // Helpers
    
    unsigned long bufferSize() const { return static_cast<unsigned long>(mBuffer.size()); }

    double convertTimeToSamples(double time);
    unsigned long convertTimeToIntSamples(double time);
    
    void copy(const double *input, unsigned long offset, unsigned long size);
    
    // Object Reset, Block Process, Update and Process 
    
    void objectReset() override;
    void blockProcess(const double * const *ins, double **outs, unsigned long blockSize) override;
    void update() override;
    void process() override;
    
    // Data
    
    std::vector<double> mBuffer;
    unsigned long mLength;
    unsigned long mCounter;
    
    static ParameterInfo sParamInfo;
};

#endif
