
#ifndef FRAMELIB_WINDOW_H
#define FRAMELIB_WINDOW_H

#include "FrameLib_DSP.h"
#include "FrameLib_WindowFunctions.h"
#include "../../FrameLib_Dependencies/TableReader.hpp"

// FIX - review gain calculation

class FrameLib_Window final : public FrameLib_Processor
{
    using WindowTypes = FrameLib_WindowFunctions::WindowTypes;
    using Compensation = FrameLib_WindowFunctions::Compensation;
    using Endpoints = FrameLib_WindowFunctions::Endpoints;

    struct CompareWindowParams
    {
        CompareWindowParams();
        CompareWindowParams(FrameLib_WindowFunctions& generator, unsigned long size);
        
        bool operator == (const CompareWindowParams& a);
        
        WindowTypes mWindowType;
        double mExponent;
        Endpoints mEndpoints;
        unsigned long mSize = 0;
    };
   
    struct Fetch : table_fetcher<double>
    {
        Fetch(const double *data, intptr_t size) : table_fetcher(size, 1.0), mData(data) {}
        
        double operator()(intptr_t offset) { return mData[offset]; }
        
        const double *mData;
    };
    
    // Parameter Enums and Info

    enum ParameterList { kWindowType, kSize, kExponent, kCompensation, kParameters, kEndpoints };
    //enum ParameterList { kWindowType, kSize, kSqrt, kCompensation, kEndpoints };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:

    // Constructor
    
    FrameLib_Window(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;

private:

    // Helpers
    
    void updateWindow (unsigned long sizeIn);
    double linearInterp(double pos);
    
    // Process
    
    void process() override;

private:
    
    // Data
    
    CompareWindowParams mCompare;
    
    AutoArray<double> mWindow;

    FrameLib_WindowFunctions mGenerator;
    
    static ParameterInfo sParamInfo;
};

#endif
