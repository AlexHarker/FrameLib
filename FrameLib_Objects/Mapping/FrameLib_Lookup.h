
#ifndef FRAMELIB_LOOKUP_H
#define FRAMELIB_LOOKUP_H

#include "FrameLib_DSP.h"
#include "../../FrameLib_Dependencies/TableReader.hpp"
#include <algorithm>

class FrameLib_Lookup : public FrameLib_Processor
{
    enum ParameterList { kMode, kInterpolation, kScaling };
    enum Mode { kZero, kClip, kWrap, kPad };
    enum Interpolation { kHermite, kBSpline, kLagrange, kLinear, kNone };
    enum Scaling { kSamples, kNormalised, kBipolar };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

    struct FetchBase : table_fetcher<double>
    {
        FetchBase(const double *data, intptr_t size) : table_fetcher(1.0), mData(data), mSize(size) {}
        
        const double *mData;
        const intptr_t mSize;
    };
    
    struct FetchZero : public FetchBase
    {
        FetchZero(const double *data, intptr_t size) : FetchBase(data, size) {}
        
        double operator()(intptr_t offset)
        {
            if (offset < 0 || offset >= mSize)
                return 0.0;
        
            return mData[offset];
        }
    };
    
    struct FetchPad : public FetchBase
    {
        FetchPad(const double *data, intptr_t size, double padValue) : FetchBase(data, size), mPadValue(padValue) {}
        
        double operator()(intptr_t offset)
        {
            if (offset < 0 || offset >= mSize)
                return mPadValue;
            
            return mData[offset];
        }
        
        const double mPadValue;
    };
    
    /*
    struct FetchWrap : public FetchBase
    {
        FetchWrap(const double *data, intptr_t size) : FetchBase(data, size) {}
        
        double operator()(intptr_t offset) { return mData[std::min(std::max(offset, 0), mSize - 1)]; }
    };*/
    
    struct FetchClip : public FetchBase
    {
        FetchClip(const double *data, intptr_t size) : FetchBase(data, size) {}
        
        double operator()(intptr_t offset) { return mData[std::min(std::max(offset, static_cast<intptr_t>(0)), mSize - 1)]; }
    };
    
public:
    
    // Constructor

    FrameLib_Lookup(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose);
    std::string inputInfo(unsigned long idx, bool verbose);
    std::string outputInfo(unsigned long idx, bool verbose);

private:
    
    // Process
    
    void process();
    
    // Data
    
    long mMode;
    long mInterpMode;
    long mScaling;
    
    static ParameterInfo sParamInfo;
};

#endif
