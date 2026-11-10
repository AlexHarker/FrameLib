
#ifndef FRAMELIB_MFCC_H
#define FRAMELIB_MFCC_H

#include "FrameLib_DSP.h"

class FrameLib_MFCC final : public FrameLib_Processor
{
    enum ParameterList { kMaxFFTSize, kMaxBands, kCoefficients, kBands, kMinFrequency, kMaxFrequency, kDropZero };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_MFCC(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    ~FrameLib_MFCC() override;
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;
    
private:
    
    // Process
    
    void objectReset() override;
    void process() override;
    
    // Data
    
    static ParameterInfo sParamInfo;
    struct Implementation;
    std::unique_ptr<Implementation> mImplementation;
    unsigned long mMaxFFTSize;
    
    // Values used for the current initialisation (those not held in the parameters)
    
    long mInitBins;
    long mInitBands;
    long mInitCoefficients;
    double mInitSamplingRate;
};

#endif
