
#ifndef FRAMELIB_WINDOWFUNCTIONS_H
#define FRAMELIB_WINDOWFUNCTIONS_H

#include "FrameLib_Object.h"

#include "../../FrameLib_Dependencies/WindowFunctions.hpp"

template <unsigned long TypeIdx, unsigned long ParamsIdx, unsigned long ExponentIdx,
unsigned long CompensateIdx, unsigned long EndpointsIdx>
class FrameLib_WindowGenerator
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
    
    FrameLib_WindowGenerator(FrameLib_Block& owner, FrameLib_Parameters& parameters)
    : mOwner(owner), mParameters(parameters), mParamSize(0) {}
    
    void addWindowType(long argIdx = -1)
    {
        mParameters.addEnum(TypeIdx, "window", argIdx);
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
    
    void addWindowParameters()
    {
        mParameters.addVariableDoubleArray(ParamsIdx, "parameters", 0.0, 5, 0);
    }
    
    void addExponent(long argIdx = -1)
    {
        mParameters.addDouble(ExponentIdx, "exponent", 1.0, argIdx);
    }
    
    void addCompensation(long argIdx = -1)
    {
        mParameters.addEnum(CompensateIdx, "compensate", argIdx);
        mParameters.addEnumItem(kOff, "off");
        mParameters.addEnumItem(kLinear, "linear");
        mParameters.addEnumItem(kPower, "power");
        mParameters.addEnumItem(kReconstruct, "reconstruct");
    }
    
    void addEndpoints(long argIdx = -1)
    {
        mParameters.addEnum(EndpointsIdx, "endpoints", argIdx);
        mParameters.addEnumItem(kBoth, "both");
        mParameters.addEnumItem(kFirst, "first", true);
        mParameters.addEnumItem(kLast, "last");
        mParameters.addEnumItem(kNone, "none");
    }
    
    void generate(double *window, unsigned long N, unsigned long begin, unsigned long end, bool calcGains)
    {
        
        // FIX - validation and errors!
        
        window_functions::params p(mValidParams, static_cast<int>(mParamSize), getExponent());
        
        uint32_t typedN = static_cast<uint32_t>(N);
        uint32_t typedBegin = static_cast<uint32_t>(begin);
        uint32_t typedEnd = static_cast<uint32_t>(end);
        
        mGenerator(getType(), window, typedN, typedBegin, typedEnd, p);
        
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
        switch (mParameters.getEnum<Compensation>(CompensateIdx))
        {
            case kOff:          return 1.0;
            case kLinear:       return 1.0 / mLinGain;
            case kPower:        return 1.0 / mPowGain;
            case kReconstruct:  return mLinGain / mPowGain;
        }
    }
    
    void updateParameters()
    {
        unsigned long arraySize;
        const double *parameters = mParameters.getArray(ParamsIdx, &arraySize);
        
        // FIX - replace with proper validation and type selection!
        
        if (mParameters.changed(TypeIdx) || mParameters.changed(ParamsIdx))
        {
            for (unsigned long i = 0; i < arraySize; i++)
                mValidParams[i] = parameters[i];
        
            mParamSize = arraySize;
        }
    }
    
    WindowTypes getType() const     { return mParameters.getEnum<WindowTypes>(TypeIdx); }
    Endpoints getEndpoints() const  { return mParameters.getEnum<Endpoints>(EndpointsIdx); }
    double getExponent() const      { return mParameters.getValue(ExponentIdx); }
    
    void getValidatedParameters(double *params, unsigned long *size) const
    {
        for (unsigned long i = 0; i < mParamSize; i++)
            params[i] = mValidParams[i];

        *size = mParamSize;
    }
    
private:
    
    FrameLib_Block& mOwner;
    FrameLib_Parameters& mParameters;
    
    unsigned long mParamSize;
    double mValidParams[5];
    
    double mLinGain;
    double mPowGain;
    
    Generator mGenerator;
};

#endif
