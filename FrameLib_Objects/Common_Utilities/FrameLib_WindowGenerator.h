
#ifndef FRAMELIB_WINDOWFUNCTIONS_H
#define FRAMELIB_WINDOWFUNCTIONS_H

#include <cmath>

#include "FrameLib_Object.h"

#include "../../FrameLib_Dependencies/WindowFunctions.hpp"

template <unsigned long TypeIdx, unsigned long ParamsIdx, unsigned long ExponentIdx,
unsigned long CompensateIdx, unsigned long EndpointsIdx>
class FrameLib_WindowGenerator
{
    using Generator = window_functions::window_generator<double>;
    
public:
    
    // Parameter enums
    
    enum WindowType { kRectangle, kTriangle, kTrapezoid, kWelch, kParzen, kTukey, kSine, kHann, kHamming, kBlackman, kExactBlackman, kBlackmanHarris, kNuttallContinuous, kNuttallMinimal, kFlatTop, kCosineSum, kKaiser, kSineTaper  };
    
    enum Compensation { kOff, kLinear, kSquare, kReconstruct };
    
    enum Endpoints { kBoth, kFirst, kLast, kNone };
    
    FrameLib_WindowGenerator(FrameLib_Block& owner, FrameLib_Parameters& parameters)
    : mOwner(owner), mParameters(parameters), mParamSize(0) , mGenerator(nullptr) {}
    
    // Add parameters
    
    void addWindowType(long argIdx = -1)
    {
        mParameters.addEnum(TypeIdx, "window", argIdx);
       
        mParameters.addEnumItem(kRectangle, "rectangle");
        mParameters.addEnumItem(kTriangle, "triangle");
        mParameters.addEnumItem(kTrapezoid, "trapezoid");
        mParameters.addEnumItem(kWelch, "welch");
        mParameters.addEnumItem(kParzen, "parzen");
        mParameters.addEnumItem(kTukey, "tukey");
        mParameters.addEnumItem(kSine, "sine");
        mParameters.addEnumItem(kHann, "hann", true);
        mParameters.addEnumItem(kHamming, "hamming");
        mParameters.addEnumItem(kBlackman, "blackman");
        mParameters.addEnumItem(kExactBlackman, "exact_blackman");
        mParameters.addEnumItem(kBlackmanHarris, "blackman_harris");
        mParameters.addEnumItem(kNuttallContinuous, "nuttall_continuous");
        mParameters.addEnumItem(kNuttallMinimal, "nuttall_minimal");
        mParameters.addEnumItem(kFlatTop, "flat_top");
        mParameters.addEnumItem(kCosineSum, "cosine_sum");
        mParameters.addEnumItem(kKaiser, "kaiser");
        mParameters.addEnumItem(kSineTaper, "sine_taper");
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
        mParameters.addEnumItem(kSquare, "square");
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
    
    // Generation and calculations
    
    void generate(double *window, unsigned long N, unsigned long begin, unsigned long end, bool calcGains)
    {
        window_functions::params p(mValidParams, static_cast<int>(mParamSize), getExponent());
        
        uint32_t typedN = static_cast<uint32_t>(N);
        uint32_t typedBegin = static_cast<uint32_t>(begin);
        uint32_t typedEnd = static_cast<uint32_t>(end);
        
        mGenerator(window, typedN, typedBegin, typedEnd, p);
        
        if (calcGains)
            calculateGains(window, 0, end - begin);
    }
    
    void calculateGains(double *window, unsigned long begin, unsigned long end)
    {
        double linSum = 0.0;
        double sqrSum = 0.0;
        
        for (unsigned long i = begin; i < end; i++)
        {
            linSum += window[i];
            sqrSum += window[i] * window[i];
        }
        
        mLinGain = linSum / static_cast<double>(end - begin);
        mSqrGain = sqrSum / static_cast<double>(end - begin);
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
            case kSquare:       return 1.0 / mSqrGain;
            case kReconstruct:  return mLinGain / mSqrGain;
        }

        assert("This code should never run");
        
        return 1.0;
    }
    
    // Parameter updates and retrieval
    
    void updateParameters()
    {
        unsigned long arraySize;
        const double *parameters = mParameters.getArray(ParamsIdx, &arraySize);
        
        using namespace window_functions;
        
        if (!mGenerator || mParameters.changed(TypeIdx) || mParameters.changed(ParamsIdx))
        {
            switch (getType())
            {
                case kRectangle:            mGenerator = rect<double>;                      break;
                case kTriangle:             mGenerator = triangle<double>;                  break;
                case kTrapezoid:            mGenerator = trapezoid<double>;                 break;
                case kWelch:                mGenerator = welch<double>;                    break;
                case kParzen:               mGenerator = parzen<double>;                    break;
                case kTukey:                mGenerator = tukey<double>;                     break;
                case kSine:                 mGenerator = sine<double>;                      break;
                case kHann:                 mGenerator = hann<double>;                      break;
                case kHamming:              mGenerator = hamming<double>;                   break;
                case kBlackman:             mGenerator = blackman<double>;                  break;
                case kExactBlackman:        mGenerator = exact_blackman<double>;            break;
                case kBlackmanHarris:       mGenerator = blackman_harris_92dB<double>;      break;
                case kNuttallContinuous:    mGenerator = nuttall_1st_93dB<double>;          break;
                case kNuttallMinimal:       mGenerator = nuttall_minimal_98dB<double>;      break;
                case kFlatTop:              mGenerator = heinzel_flat_top_95dB<double>;     break;
                case kCosineSum:
                {
                    if (arraySize <= 2)
                        mGenerator = cosine_2_term<double>;
                    else if (arraySize == 3)
                        mGenerator = cosine_3_term<double>;
                    else if (arraySize == 4)
                        mGenerator = cosine_4_term<double>;
                    else
                        mGenerator = cosine_5_term<double>;
                    break;
                }
                case kKaiser:               mGenerator = kaiser<double>;                    break;
                case kSineTaper:            mGenerator = sine_taper<double>;                break;
            }
            
            switch (getType())
            {
                case kTrapezoid:
                    mValidParams[0] = arraySize ? parameters[0] / 100.0 : 0.25;
                    mValidParams[1] = arraySize > 1 ? parameters[1] / 100.0 : 1.0 - mValidParams[0];
                    mParamSize = 2;
                    break;
                    
                case kTukey:
                    mValidParams[0] = arraySize ? parameters[0] / 100.0 : 0.5;
                    mParamSize = 1;
                    break;
                    
                case kCosineSum:
                {
                    if (arraySize == 0)
                    {
                        mValidParams[0] = 0.5;
                        mValidParams[1] = 0.5;
                    }
                    else if (arraySize == 1)
                    {
                        mValidParams[0] = parameters[0];
                        mValidParams[1] = 1.0 - parameters[0];
                    }
                    else
                    {
                        for (unsigned long i = 0; i < arraySize; i++)
                            mValidParams[i] = parameters[i];
                    }
                    
                    mParamSize = arraySize > 1 ? arraySize : 2;
                    break;
                }
                
                case kKaiser:
                    mValidParams[0] = arraySize ? parameters[0] : 6.24;
                    mParamSize = 1;
                    break;
                    
                case kSineTaper:
                    mValidParams[0] = arraySize ? std::max(round(parameters[0]), 1.0) : 1;
                    mParamSize = 1;
                    break;
                    
                default:
                    break;
            }
            
            if (mParamSize && arraySize > mParamSize)
            {
                mOwner.getReporter()(ErrorSource::Object, mOwner.getProxy(), "too many parameters given for window type #", mParameters.getItemString(TypeIdx, getType()));
            }
        }
    }
    
    WindowType getType() const      { return mParameters.getEnum<WindowType>(TypeIdx); }
    Endpoints getEndpoints() const  { return mParameters.getEnum<Endpoints>(EndpointsIdx); }
    double getExponent() const      { return mParameters.getValue(ExponentIdx); }
    
    void getValidatedParameters(double *params, unsigned long *size) const
    {
        for (unsigned long i = 0; i < mParamSize; i++)
            params[i] = mValidParams[i];

        *size = mParamSize;
    }
    
    // Parameter info
    
    static const char *getWindowTypeInfo()
    {
        return "Sets the window type: "
               "rectangle - a window of equal values. "
               "triangle - a triangular window. "
               "trapezoid - an adjustable trapezoid window. "
               "welch - the Welch polynomial window. "
               "parzen - the Parzen polynomial window. "
               "tukey - the adjustable Tukey (or cosine-tapered) window. "
               "sine - the sine (or cosine) window. "
               "hann - the Hann window. "
               "hamming - the Hamming window with a0 = 0.54 and a1 = 0.46. "
               "blackman - the Blackman window with a0 = 0.42, a1 = 0.50, and a2 = 0.08. "
               "exact_blackman - the exact Blackman window as outlined in the literature. "
               "The coefficients are a0 = 7938/18608, a1 = 9240/18608 and a2 = 1430/18608. "
               "blackman_harris - the Blackman-Harris window with 92dB rejection. "
               "nuttall_continuous - Nuttall's continuous 1st derivative window with 93dB rejection. "
               "nuttall_minimal - Nuttall's minimal sidelobe window with 98dB rejection. "
               "flat_top - Heinzel's flat-top window with 95dB rejection. "
               "cosine_sum - a sum of cosines as set by the parameters. "
               "kaiser - an kaiser window adjustable via the beta parameter. "
               "sine_taper - one of the sine tapers selectable by parameter.";
    }
    
    static const char *getWindowParametersInfo()
    {
        return "An array that sets parameters specific to the window type: "
               "trapezoid - takes either one or two parameter as percentages. "
               "If one parameter is provided it sets the first high point which is then mirrored. "
               "If two parameters are provided they set the two points of the trapezoid. "
               "The defaults are 25% and 75%. "
               "tukey - takes a single parameter to set the cosine percentage (defaults to 50%). "
               "When set to 100% the window is the same as the Hann window. "
               "Reducing the percentage increases the central plateau. "
               "cosine_sum - takes up to 5 parameters (a0-a4) as in the following formula. "
               "w(x) = a0 - a1 * cos(2πx) + a2 * cos(4πx) - a3 * cos(6πx) + a4 * cos(8πx). "
               "The defaults are set to the values for a Hann window. "
               "kaiser - takes a single parameter to set beta (defaults to 6.24). "
               "sine_taper - takes a single integer parameter to select the taper (defaults to 1). "
               "Note that other window types do not take any parameters.";
    }
    
    static const char *getExponentInfo()
    {
        return "Sets an exponent that each value in the window should be raised to.";
    }
    
    static const char *getCompensationInfo()
    {
        return "Sets the gain compensation (the window is divided by the compensated gain): "
        "off - no compensation is used. "
        "linear - compensate the linear gain of the window. "
        "square - compensate the gain of the window when applied twice (squared). "
        "reconstruct - compensate the gain of the squared window divided by the linear gain. "
        "This last mode is suited to FFT output windows used with the same input window. "
        "Note that the gain of the window is calculated after applying any exponent.";
    }
    
    static const char *getEndpointsInfo()
    {
        return "Sets which endpoints of the window will be generated. "
               "For spectral applications a periodic window (first or last) is preferable. "
               "For granular or filter usage a symmetric window (both or none) is preferable.";
    }
    
private:
    
    FrameLib_Block& mOwner;
    FrameLib_Parameters& mParameters;
    
    unsigned long mParamSize;
    double mValidParams[5];
    
    double mLinGain;
    double mSqrGain;
    
    Generator *mGenerator;
};

#endif
