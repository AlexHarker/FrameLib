
#ifndef FRAMELIB_SCALING_FUNCTIONS_H
#define FRAMELIB_SCALING_FUNCTIONS_H

#include <algorithm>
#include <cmath>

// Common Conversions

template <class T>
T dbToAmp(T x) { return std::pow(10.0, x / 20.0); }

template <class T>
T ampToDb(T x) { return std::log10(x) * 20.0; }

template <class T>
T midiToFreq(T x) { return std::exp2((x - 69.0) / 12.0) * 440.0; }

template <class T>
T freqToMidi(T x) { return std::log2(x / 440.0) * 12.0 + 69.0; }

template <class T>
T semitonesToRatio(T x) { return std::exp2(x / 12.0); }

template <class T>
T ratioToSemitones(T x) { return std::log2(x) * 12.0; }

// Scaling of Vectors

template <class T, typename ScaleOp>
void scaleVector(T *output, const T *input, unsigned long size, ScaleOp scaler)
{
    for (unsigned long i = 0; i < size; i++)
        output[i] = scaler(input[i]);
}

// Basic Clipping

template <class T>
T clip(T x, T minVal, T maxVal) { return std::max(std::min(x, maxVal), minVal); }

// Scaling Base Struct

template <class T>
struct ScaleCoefficients
{
    ScaleCoefficients() : mMul(1), mSub(0) {}
    ScaleCoefficients(T mul, T sub) : mMul(mul), mSub(sub) {}

    ScaleCoefficients(T inLo, T inHi, T outLo, T outHi)
    : mMul((inLo == inHi) ? 0 : (outHi - outLo) / (inHi - inLo)), mSub((inLo * mMul) - outLo) {}

    T mMul, mSub;
};

// Lin, Log, Exp and Power Scaling Functors

template <class T>
struct LinScaler : public ScaleCoefficients<T>
{
    LinScaler() : ScaleCoefficients<T>() {}
    LinScaler(T inLo, T inHi, T outLo, T outHi) : ScaleCoefficients<T>(inLo, inHi, outLo, outHi) {}
    LinScaler(const ScaleCoefficients<T>& coeff) : ScaleCoefficients<T>(coeff) {}
    
    template <class U> U operator()(U x) const { return (x * ScaleCoefficients<T>::mMul) - ScaleCoefficients<T>::mSub; }
    template <class U> void operator()(U *output, const U *input, unsigned long size) const { scaleVector(output, input, size, *this); }
};

template <class T>
struct LogScaler : public ScaleCoefficients<T>
{
    LogScaler(T inLo, T inHi, T outLo, T outHi) : ScaleCoefficients<T>(std::log(inLo), std::log(inHi), outLo, outHi) {}
    LogScaler(const ScaleCoefficients<T>& coeff) : ScaleCoefficients<T>(coeff) {}
    
    template <class U> U operator()(U x) const { return std::log(x) * ScaleCoefficients<T>::mMul - ScaleCoefficients<T>::mSub; }
    template <class U> void operator()(U *output, const U *input, unsigned long size) const { scaleVector(output, input, size, *this); }
};

template <class T>
struct ExpScaler : public ScaleCoefficients<T>
{
    ExpScaler(T inLo, T inHi, T outLo, T outHi) : ScaleCoefficients<T>(inLo, inHi, std::log(outLo), std::log(outHi)) {}
    ExpScaler(const ScaleCoefficients<T>& coeff) : ScaleCoefficients<T>(coeff) {}
    
    template <class U> U operator()(U x) const { return exp((x * ScaleCoefficients<T>::mMul) - ScaleCoefficients<T>::mSub); }
    template <class U> void operator()(U *output, const U *input, unsigned long size) const { scaleVector(output, input, size, *this); }
};

template <class T>
struct PowScaler
{
    PowScaler(T inLo, T inHi, T outLo, T outHi, T exponent) : mInputScaler(inLo, inHi, 0, 1), mOutputScaler(0, 1, outLo, outHi), mExponent(exponent) {}
    PowScaler(const ScaleCoefficients<T>& inCoeff, const ScaleCoefficients<T>& outCoeff, T exponent)
    : mInputScaler(inCoeff), mOutputScaler(outCoeff), mExponent(exponent) {}
    
    void getCoefficients(ScaleCoefficients<T> &inCoeff, ScaleCoefficients<T>& outCoeff, T& exponent)
    {
        inCoeff = mInputScaler;
        outCoeff = mOutputScaler;
        exponent = mExponent;
    }
    
    template <class U> U operator()(U x) const { return mOutputScaler(std::pow(mInputScaler(x), mExponent)); }
    template <class U> void operator()(U *output, const U *input, unsigned long size) const { scaleVector(output, input, size, *this); }
    
    LinScaler<T> mInputScaler;
    LinScaler<T> mOutputScaler;
    T mExponent;
};

// Clip After Scale

template <class T, typename ScaleOp>
struct ClipScaler
{
    ClipScaler(ScaleOp scaler, T mMin, T mMax) : mScaler(scaler), mMin(mMin), mMax(mMax) {}
    
    template <class U> T operator()(U x) const { return clip(mScaler(x), mMin, mMax); }
    template <class U> void operator()(U *output, const U *input, unsigned long size) const { scaleVector(output, input, size, *this); }
    
    ScaleOp mScaler;
    T mMin, mMax;
};

// ClipScaler Definitions (add clipping to functors)

template <class T> struct
LinClipScaler : public ClipScaler<T, LinScaler<T>>
{
    LinClipScaler() : ClipScaler<T, LinScaler<T>>(LinScaler<T>(), 0, 1) {}
    LinClipScaler(T inLo, T inHi, T outLo, T outHi) : ClipScaler<T, LinScaler<T>>(LinScaler<T>(inLo, inHi, outLo, outHi), outLo, outHi) {}
    LinClipScaler(const ScaleCoefficients<T>& coeff, T min, T max) : ClipScaler<T, LinScaler<T>>(LinScaler<T>(coeff), min, max) {}
};

template <class T>
struct LogClipScaler : public ClipScaler<T, LogScaler<T>>
{
    LogClipScaler(T inLo, T inHi, T outLo, T outHi) : ClipScaler<T, LogScaler<T>>(std::log(inLo), std::log(inHi), outLo, outHi) {}
    LogClipScaler(const ScaleCoefficients<T>& coeff, T min, T max) : ClipScaler<T, LogScaler<T>>(LogScaler<T>(coeff), min, max) {}
};

template <class T>
struct ExpClipScaler : public ClipScaler<T, ExpScaler<T>>
{
    ExpClipScaler(T inLo, T inHi, T outLo, T outHi) : ClipScaler<T, ExpScaler<T>>(ExpScaler<T>(inLo, inHi, std::log(outLo), std::log(outHi)), outLo, outHi) {}
    ExpClipScaler(const ScaleCoefficients<T>& coeff, T min, T max) : ClipScaler<T, ExpScaler<T>>(ExpScaler<T>(coeff), min, max) {}
};

template <class T>
struct PowClipScaler : public ClipScaler<T, PowScaler<T>>
{
    PowClipScaler(T inLo, T inHi, T outLo, T outHi, T exponent)
    : ClipScaler<T, PowScaler<T>>(PowScaler<T>(inLo, inHi, std::log(outLo), std::log(outHi)), outLo, outHi, exponent) {}
    PowClipScaler(const ScaleCoefficients<T>& inCoeff, const ScaleCoefficients<T>& outCoeff, T exponent, T min, T max)
    : ClipScaler<T, PowScaler<T>>(PowScaler<T>(inCoeff, outCoeff, exponent), min, max) {}
};

// Variable Lin, Log or Exp Scaling

template <class T>
struct VariableScaler
{
    VariableScaler() : mMode(kScaleLin) {}

    void setLin(T inLo, T inHi, T outLo, T outHi)
    {
        mMode = kScaleLin;
        mCoefficients1 = LinScaler<T>(inLo, inHi, outLo, outHi);
    }
    
    void setLog(T inLo, T inHi, T outLo, T outHi)
    {
        mMode = kScaleLog;
        mCoefficients1 = LogScaler<T>(inLo, inHi, outLo, outHi);
    }
    
    void setExp(T inLo, T inHi, T outLo, T outHi)
    {
        mMode = kScaleExp;
        mCoefficients1 = ExpScaler<T>(inLo, inHi, outLo, outHi);
    }
    
    void setPow(T inLo, T inHi, T outLo, T outHi, T exponent)
    {
        if (exponent == 1)
            setLin(inLo, inHi, outLo, outHi);
        else
        {
            mMode = kScalePow;
            PowScaler<T>(inLo, inHi, outLo, outHi, exponent).getCoefficients(mCoefficients1, mCoefficients2, mExponent);
        }
    }
    
    template <class U>
    U scale(const U x)
    {
        switch (mMode)
        {
            case kScaleLin:     return (LinScaler<T>(mCoefficients1))(x);
            case kScaleLog:     return (LogScaler<T>(mCoefficients1))(x);
            case kScaleExp:     return (ExpScaler<T>(mCoefficients1))(x);
            case kScalePow:     return (PowScaler<T>(mCoefficients1, mCoefficients2, mExponent))(x);
        }
    }
    
    template <class U>
    void scale(U *output, const U* input, unsigned long size)
    {
        switch (mMode)
        {
            case kScaleLin:     (LinScaler<T>(mCoefficients1))(output, input, size);                                break;
            case kScaleLog:     (LogScaler<T>(mCoefficients1))(output, input, size);                                break;
            case kScaleExp:     (ExpScaler<T>(mCoefficients1))(output, input, size);                                break;
            case kScalePow:     (PowScaler<T>(mCoefficients1, mCoefficients2, mExponent))(output, input, size);     break;
        }
    }
    
protected:
    
    enum ScaleMode { kScaleLin, kScaleExp, kScaleLog, kScalePow };

    ScaleMode mMode;
    ScaleCoefficients<T> mCoefficients1;
    ScaleCoefficients<T> mCoefficients2;
    T mExponent;
};

// Variable Lin, Log or Exp Scaling With or Without Clipping

template <class T>
struct VariClipScaler : public VariableScaler<T>
{
    VariClipScaler() : mMin(0), mMax(1) {}
    
    void setLin(T inLo, T inHi, T outLo, T outHi)
    {
        setClip(outLo, outHi);
        Base::setLin(inLo, inHi, outLo, outHi);
    }
    
    void setLog(T inLo, T inHi, T outLo, T outHi)
    {
        setClip(outLo, outHi);
        Base::setLog(inLo, inHi, outLo, outHi);
    }
    
    void setExp(T inLo, T inHi, T outLo, T outHi)
    {
        setClip(outLo, outHi);
        Base::setExp(inLo, inHi, outLo, outHi);
    }
    
    void setPow(T inLo, T inHi, T outLo, T outHi, T exponent)
    {
        setClip(outLo, outHi);
        Base::setPow(inLo, inHi, outLo, outHi, exponent);
    }

    template <class U> U scaleClip(T x) { return clip(scale(x), mMin, mMax); }
    
    template <class U> void scaleClip(U *output, const U *input, unsigned long size)
    {
        switch (VariableScaler<T>::mMode)
        {
            case Base::kScaleLin:
                (LinClipScaler<T>(Base::mCoefficients1, mMin, mMax))(output, input, size);
                break;
            case Base::kScaleLog:
                (LogClipScaler<T>(Base::mCoefficients1, mMin, mMax))(output, input, size);
                break;
            case Base::kScaleExp:
                (ExpClipScaler<T>(Base::mCoefficients1, mMin, mMax))(output, input, size);
                break;
            case Base::kScalePow:
                (PowClipScaler<T>(Base::mCoefficients1, Base::mCoefficients2, Base::mExponent, mMin, mMax))(output, input, size);
                break;
        }
    }
    
protected:
    
    void setClip(T minVal, T maxVal)
    {
        mMin = std::min(minVal, maxVal);
        mMax = std::max(minVal, maxVal);
    }
    
    T mMin, mMax;
    
private:
    
    typedef VariableScaler<T> Base;
};

// Variable Scaling and Common Conversions

template <class T>
struct ScaleConverter : public VariableScaler<T>
{
    void setDBToAmplitude()     { Base::setExp(0, 20, 1, 10); }
    void setAmplitudeToDB()     { Base::setLog(1, 10, 0, 20); }
    void setMIDIToFreq()        { Base::setExp(57, 69, 220, 440); }
    void setFreqToMIDI()        { Base::setLog(220, 440, 57, 69); }
    void setSemitonesToRatios() { Base::setExp(0, 12, 1, 2); }
    void setRatiosToSemitones() { Base::setLog(1, 2, 0, 12); }
    void setDegreesToRadians()  { Base::setLin(0, 360, 0, M_PI * 2.0); }
    void setRadiansToDegrees()  { Base::setLin(0, M_PI * 2.0, 0, 360); }
    
private:
    
    typedef VariableScaler<T> Base;
};

// Typedefs for double precision versions

typedef LinScaler<double> FrameLib_LinScaler;
typedef LogScaler<double> FrameLib_LogScaler;
typedef ExpScaler<double> FrameLib_ExpScaler;
typedef PowScaler<double> FrameLib_PowScaler;

typedef LinClipScaler<double> FrameLib_LinClipScaler;
typedef LogClipScaler<double> FrameLib_LogClipScaler;
typedef ExpClipScaler<double> FrameLib_ExpClipScaler;
typedef PowClipScaler<double> FrameLib_PowClipScaler;

typedef VariableScaler<double> FrameLib_VariableScaler;
typedef VariClipScaler<double> FrameLib_VariClipScaler;
typedef ScaleConverter<double> FrameLib_ScaleConverter;

#endif
