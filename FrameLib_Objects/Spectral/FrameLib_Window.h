
#ifndef FRAMELIB_WINDOW_H
#define FRAMELIB_WINDOW_H

#include "FrameLib_DSP.h"
#include "../../FrameLib_Dependencies/WindowFunctions.hpp"
#include "../../FrameLib_Dependencies/TableReader.hpp"

// FIX - review gain calculation

class FrameLib_Window final : public FrameLib_Processor
{
    // Parameter Enums and Info

    enum ParameterList { kWindowType, kSize, kSqrt, kCompensation, kEndPoints };
    enum Compensation { kOff, kLinear, kPower, kPowerOverLinear };
    enum EndPoints { kFirst, kLast, kBoth, kNone };
    enum WindowTypes { kHann, kHamming, kTriangle, kCosine, kBlackman, kBlackman62, kBlackman70, kBlackman74, kBlackman92, kBlackmanHarris, kFlatTop, kRectangle };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:

    // Constructor / Destructor
    
    FrameLib_Window(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    ~FrameLib_Window();
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;

private:

    // Helpers
    
    void updateWindow (unsigned long inSize, EndPoints ends);
    double linearInterp(double pos);
    
    // Process
    
    void process() override;

private:

    struct Fetch : table_fetcher<double>
    {
        Fetch(const double *data, intptr_t size) : table_fetcher(size, 1.0), mData(data) {}
        
        double operator()(intptr_t offset) { return mData[offset]; }
        
        const double *mData;
    };
    
    struct WindowCalculator : public WindowFunctions<double *, WindowTypes>
    {
        WindowCalculator()
        {
            add(kHann, window_hann);
            add(kHamming, window_hamming);
            add(kTriangle, window_triangle);
            add(kCosine, window_cosine);
            add(kBlackman, window_blackman);
            add(kBlackman62, window_blackman_62);
            add(kBlackman70, window_blackman_70);
            add(kBlackman74, window_blackman_62);
            add(kBlackman92, window_blackman_92);
            add(kBlackmanHarris, window_blackman_harris);
            add(kFlatTop, window_kaiser);
            add(kRectangle, window_rect);
        }
    };
    
    // Data
    
    static WindowCalculator sWindowCalculator;
    double *mWindow;

    WindowTypes mWindowType;
    EndPoints mEnds;
    
    unsigned long mSize;

    bool mSqrtWindow;

    double mLinearGain;
    double mPowerGain;
    
    static ParameterInfo sParamInfo;
};

#endif
