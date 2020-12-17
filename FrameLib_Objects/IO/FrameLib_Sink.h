

#ifndef FRAMELIB_SINK_H
#define FRAMELIB_SINK_H

#include "FrameLib_DSP.h"
#include "Framelib_IO_Helper.h"
#include <vector>

class FrameLib_Sink final : public FrameLib_AudioOutput, private FrameLib_IO_Helper
{
    enum ParameterList { kBufferSize, kUnits, kDelay, kInterpolation };
    enum Interpolation { kNone, kLinear, kHermite, kBSpline, kLagrange };
    enum Units { kSamples, kMS, kSeconds };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };
    
    struct Fetcher : table_fetcher<double>
    {
        Fetcher(const double *data, intptr_t size) : table_fetcher(size, 1.0), mData(data) {}
        
        double operator()(intptr_t offset) { return mData[offset]; }
        
        const double *mData;
    };
    
public:

    // Constructor
    
    FrameLib_Sink(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string audioInfo(unsigned long idx, bool verbose) override;
    
private:
    
    // Helpers
    
    unsigned long bufferSize() const { return static_cast<unsigned long>(mBuffer.size()); }

    double convertTimeToSamples(double time);
    unsigned long convertTimeToIntSamples(double time);
    
    void copyAndZero(double *output, unsigned long offset, unsigned long size);
    void addToBuffer(const double *input, unsigned long offset, unsigned long size);
    
    // Object Reset, Block Process and Process

    void objectReset() override;
    void blockProcess(const double * const *ins, double **outs, unsigned long blockSize) override;
    void process() override;
    
    // Data
    
    std::vector<double> mBuffer;
    unsigned long mCounter;

    static ParameterInfo sParamInfo;
};

#endif
