
#ifndef FRAMELIB_WINDOW_H
#define FRAMELIB_WINDOW_H

#include "FrameLib_DSP.h"
#include "FrameLib_WindowGenerator.h"
#include "../../FrameLib_Dependencies/TableReader.hpp"

class FrameLib_Window final : public FrameLib_Processor
{
    // Parameter Enums and Info
    
    enum ParameterList { kWindowType, kSize, kExponent, kCompensation, kParameters, kEndpoints };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };
    
    // Types (depend on the parameter enum)
    
    using Generator = FrameLib_WindowGenerator<kWindowType, kParameters, kExponent, kCompensation, kEndpoints>;

    using WindowTypes = Generator::WindowTypes;
    using Compensation = Generator::Compensation;
    using Endpoints = Generator::Endpoints;

    struct CompareWindowParams
    {
        CompareWindowParams();
        CompareWindowParams(Generator& generator, unsigned long size);
        
        bool operator == (const CompareWindowParams& a);
        
        WindowTypes mWindowType;
        Endpoints mEndpoints;
        double mParams[5];
        double mExponent;
        unsigned long mParamSize;
        unsigned long mSize;
    };
   
    struct Fetch : table_fetcher<double>
    {
        Fetch(const double *data, intptr_t size) : table_fetcher(size, 1.0), mData(data) {}
        
        double operator()(intptr_t offset) { return mData[offset]; }
        
        const double *mData;
    };

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
    
    // Update
    
    void update() override { mGenerator.updateParameters(); }

    // Process
    
    void process() override;

private:
    
    // Data
    
    CompareWindowParams mCompare;
    
    AutoArray<double> mWindow;

    Generator mGenerator;
    
    static ParameterInfo sParamInfo;
};

#endif
