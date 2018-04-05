
#ifndef FRAMELIB_WINDOW_H
#define FRAMELIB_WINDOW_H

#include "FrameLib_DSP.h"
#include "../../FrameLib_Dependencies/WindowFunctions.hpp"

// FIX - review gain calculation

class FrameLib_Window : public FrameLib_Processor
{
    // Parameter Enums and Info

	enum ParameterList { kWindowType, kSize, kSqrt, kCompensation, kEndPoints };
    enum Compensation { kOff, kLinear, kPower, kPowerOverLinear };
    enum EndPoints { kFirst, kLast, kBoth, kNone };
    enum WindowTypes { kHann, kHamming, kTriangle, kCosine, kBlackman, kBlackman62, kBlackman70, kBlackman74, kBlackman92, kBlackmanHarris, kFlatTop, kRectangle };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
	
    // Constructor / Destructor
    
    FrameLib_Window(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    ~FrameLib_Window();
    
    // Info
    
    std::string objectInfo(bool verbose);
    std::string inputInfo(unsigned long idx, bool verbose);
    std::string outputInfo(unsigned long idx, bool verbose);
	
private:
	
    // Helpers
    
    void updateWindow (unsigned long inSize, EndPoints ends);
    double linearInterp(double pos);
    
    // Process
    
    void process();
	
private:

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
