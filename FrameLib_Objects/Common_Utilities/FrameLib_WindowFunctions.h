
#ifndef FRAMELIB_WINDOWFUNCTIONS_H
#define FRAMELIB_WINDOWFUNCTIONS_H

#include "FrameLib_Parameters.h"
#include "../../FrameLib_Dependencies/WindowFunctions.hpp"

class FrameLib_WindowFunctions
{
    using Generator = window_functions::indexed_generator<double
    , window_functions::hann<double>
    , window_functions::hamming<double>
    , window_functions::triangle<double>
    , window_functions::trapezoid<double>
    , window_functions::tukey<double>
    , window_functions::sine<double>
    , window_functions::kaiser<double>
    , window_functions::blackman<double>
    , window_functions::blackman_harris_92dB<double>
    , window_functions::nuttall_1st_93dB<double>
    , window_functions::nuttall_minimal_98dB<double>
    , window_functions::heinzel_flat_top_95dB<double>
    , window_functions::rect<double>
    >;
    
public:
    
    enum WindowTypes { kHann, kHamming, kTriangle, kTrapezoid, kTukey, kSine, kKaiser, kBlackman, kBlackmanHarris, kNuttallContinuous, kNuttallMinimal, kFlatTop, kRectangle };
    
    enum Compensation { kOff, kLinear, kPower, kReconstruct };
    
    enum Endpoints { kBoth, kFirst, kLast, kNone };
    
    FrameLib_WindowFunctions(FrameLib_Parameters& parameters)
    : mParameters(parameters) {}
    
    void addWindowType(unsigned long idx, long argIdx = -1)
    {
        mTypeIdx = idx;
        
        mParameters.addEnum(idx, "window", argIdx);
        mParameters.addEnumItem(kHann, "hann");
        mParameters.addEnumItem(kHamming, "hamming");
        mParameters.addEnumItem(kTriangle, "triangle");
        mParameters.addEnumItem(kTrapezoid, "trapezoid");
        mParameters.addEnumItem(kTukey, "tukey");
        mParameters.addEnumItem(kSine, "sine");
        mParameters.addEnumItem(kKaiser, "kaiser");
        mParameters.addEnumItem(kBlackman, "blackman");
        mParameters.addEnumItem(kBlackmanHarris, "blackman-harris");
        mParameters.addEnumItem(kNuttallContinuous, "nuttall_continuous");
        mParameters.addEnumItem(kNuttallMinimal, "nuttall_min");
        mParameters.addEnumItem(kFlatTop, "flat-top");
        mParameters.addEnumItem(kRectangle, "rectangle");
    }
    
    void addWindowParameters(unsigned long idx)
    {
        mParametersIdx = idx;
        mParameters.addVariableDoubleArray(idx, "parameters", 0.0, 6, 0);
    }
    
    void addExponent(unsigned long idx, long argIdx = -1)
    {
        mExponentIdx = idx;
        mParameters.addDouble(idx, "exponent", 1.0, argIdx);
    }
    
    void addCompensation(unsigned long idx, long argIdx = -1)
    {
        mCompensationIdx = idx;
        
        mParameters.addEnum(idx, "compensate", argIdx);
        mParameters.addEnumItem(kOff, "off");
        mParameters.addEnumItem(kLinear, "linear");
        mParameters.addEnumItem(kPower, "power");
        mParameters.addEnumItem(kReconstruct, "reconstruct");
    }
    
    void addEndpoints(unsigned long idx, long argIdx = -1)
    {
        mEndpointsIdx = idx;
        
        mParameters.addEnum(idx, "endpoints", argIdx);
        mParameters.addEnumItem(kBoth, "both");
        mParameters.addEnumItem(kFirst, "first", true);
        mParameters.addEnumItem(kLast, "last");
        mParameters.addEnumItem(kNone, "none");
    }
    
    void generate(double *window, unsigned long N, unsigned long begin, unsigned long end, bool calcGains)
    {
        unsigned long arraySize;
        const double *parameters = mParameters.getArray(mParametersIdx, &arraySize);
        // FIX - validation and errors!
        
        window_functions::params p(parameters, static_cast<int>(arraySize), getExponent());
        
        mGenerator(getType(), window, N, begin, end, p);
        
        if (calcGains)
            calculateGains(window, begin, end);
    }
    
    void calculateGains(double *window, unsigned long begin, unsigned long end)
    {
        double linSum = 0.0;
        double powSum = 0.0;
        
        for (unsigned long i = begin; i < end; i++)
        {
            linSum += window[i];
            powSum += window[i] * window[i];
        }
        
        mLinGain = linSum / static_cast<double>(end - begin);
        mPowGain = powSum / static_cast<double>(end - begin);
    }
    
    unsigned long sizeAdjustForEndpoints(unsigned long size) const
    {
        Endpoints endpoints = getEndpoints();
        return endpoints == kBoth ? size - 1 : (endpoints == kNone ? size + 1 : size);
    }
    
    bool doFirst() const
    {
        Endpoints endpoints = getEndpoints();
        return endpoints == kFirst || endpoints == kBoth;
    }
    
    bool doLast() const
    {
        Endpoints endpoints = getEndpoints();
        return endpoints == kLast || endpoints == kBoth;
    }
    
    double gainCompensation()
    {
        switch (mParameters.getEnum<Compensation>(mCompensationIdx))
        {
            case kOff:          return 1.0;
            case kLinear:       return 1.0 / mLinGain;
            case kPower:        return 1.0 / mPowGain;
            case kReconstruct:  return mLinGain / mPowGain;
        }
    }
    
    WindowTypes getType() const     { return mParameters.getEnum<WindowTypes>(mTypeIdx); }
    Endpoints getEndpoints() const  { return mParameters.getEnum<Endpoints>(mEndpointsIdx); }
    double getExponent() const      { return mParameters.getValue(mExponentIdx); }
    
private:
    
    FrameLib_Parameters &mParameters;
    
    unsigned long mTypeIdx;
    unsigned long mParametersIdx;
    unsigned long mExponentIdx;
    unsigned long mCompensationIdx;
    unsigned long mEndpointsIdx;
    
    double mLinGain;
    double mPowGain;
    
    Generator mGenerator;
};

#endif
