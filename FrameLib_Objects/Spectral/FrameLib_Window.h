
#ifndef FRAMELIB_WINDOW_H
#define FRAMELIB_WINDOW_H

#include "FrameLib_DSP.h"

// FIX - review gain calculation

class FrameLib_Window : public FrameLib_Processor
{
    // Parameter Enums and Info

	enum ParameterList { kWindowType, kSize, kSqrt, kCompensation, kEndPoints };
    enum Compensation { kOff, kLinear, kPower, kPowerOverLinear };
    enum EndPoints { kFirst, kLast, kBoth, kNone };
    enum WindowTypes { kHann, kHamming, kTriangle, kCosine, kBlackman, kBlackman62, kBlackman70, kBlackman74, kBlackman92, kBlackmanHarris, kFlatTop, kRectangle };

public:
	
    // Constructor / Destructor
    
    FrameLib_Window(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    ~FrameLib_Window();
	
private:
	
    // Helpers
    
    void updateWindow (unsigned long inSize, EndPoints ends);
    double linearInterp(double pos);
    
    // Update and Process
    
    void update();
    void process();
	
private:

    // Data
    
    double *mWindow;

    WindowTypes mWindowType;
    
    unsigned long mSize;

    bool mSqrtWindow;

    double mLinearGain;
    double mPowerGain;
};

#endif
