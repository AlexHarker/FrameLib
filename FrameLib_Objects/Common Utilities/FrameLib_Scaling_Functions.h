
#ifndef FRAMELIB_SCALING_FUNCTIONS_H
#define FRAMELIB_SCALING_FUNCTIONS_H

#include <algorithm>
#include <cmath>

// Common Conversions

template <class T> T dbtoa(T x) { return pow(10.0, x / 20.0); }
template <class T> T atodb(T x) { return log10(x) * 20.0; }

template <class T> T mtof(T x) { return exp2(x - 69.0) * 440.0; }
template <class T> T ftom(T x) { return log2(x / 440.0) + 69.0; }

template <class T> T semitonesToRatio(T x) { return exp2(x / 12.0); }
template <class T> T ratioToSemitones(T x) { return log2(x) * 12.0; }

// Scaling of Vectors

template <class T, typename ScaleOp>
void scaleVector(T *output, T *input, unsigned long size, ScaleOp scaler)
{
    for (unsigned long i = 0; i < size; i++)
        output[i] = scaler(input[i]);
}

// Basic Clipping

template <class T> T clip(T x, T minVal, T maxVal) { return std::max(std::min(x, maxVal), minVal); }

// Scaling Base Struct

template <class T> struct ScaleCoefficients
{
    ScaleCoefficients() : mMul(1), mSub(0) {}
    ScaleCoefficients(T mul, T sub) : mMul(mul), mSub(sub) {}

    ScaleCoefficients(T inLo, T inHi, T outLo, T outHi)
    : mMul((inLo == inHi) ? 0 : (outHi - outLo) / (inHi - inLo)), mSub((inLo * mMul) - outLo) {}

    T mMul, mSub;
};

// Lin, Log and Exp Scaling Functors

template <class T> struct LinScaler : public ScaleCoefficients<T>
{
    LinScaler() : ScaleCoefficients<T>() {}
    LinScaler(T inLo, T inHi, T outLo, T outHi) : ScaleCoefficients<T>(inLo, inHi, outLo, outHi) {}
    LinScaler(const ScaleCoefficients<T>& coeff) : ScaleCoefficients<T>(coeff) {}
    
    template <class U> U operator()(U x) const { return (x * ScaleCoefficients<T>::mMul) - ScaleCoefficients<T>::mSub; }
    template <class U> void operator()(U *output, U *input, unsigned long size) const { scaleVector(output, input, size, *this); }
};

template <class T> struct LogScaler : public ScaleCoefficients<T>
{
    LogScaler(T inLo, T inHi, T outLo, T outHi) : ScaleCoefficients<T>(log(inLo), log(inHi), outLo, outHi) {}
    LogScaler(const ScaleCoefficients<T>& coeff) : ScaleCoefficients<T>(coeff) {}
    
    template <class U> U operator()(U x) const { return log(x) * ScaleCoefficients<T>::mMul - ScaleCoefficients<T>::mSub; }
    template <class U> void operator()(U *output, U *input, unsigned long size) const { scaleVector(output, input, size, *this); }
};

template <class T> struct ExpScaler : public ScaleCoefficients<T>
{
    ExpScaler(T inLo, T inHi, T outLo, T outHi) : ScaleCoefficients<T>(inLo, inHi, log(outLo), log(outHi)) {}
    ExpScaler(const ScaleCoefficients<T>& coeff) : ScaleCoefficients<T>(coeff) {}
    
    template <class U> U operator()(U x) const { return exp((x * ScaleCoefficients<T>::mMul) - ScaleCoefficients<T>::mSub); }
    template <class U> void operator()(U *output, U *input, unsigned long size) const { scaleVector(output, input, size, *this); }
};

// Clip After Scale

template <class T, typename ScaleOp>
struct ClipScaler
{
    ClipScaler(ScaleOp scaler, T mMin, T mMax) : mScaler(scaler), mMin(mMin), mMax(mMax) {}
    
    template <class U> T operator()(U x) const { return clip(mScaler(x), mMin, mMax); }
    template <class U> void operator()(U *output, U *input, unsigned long size) const { scaleVector(output, input, size, *this); }
    
    ScaleOp mScaler;
    T mMin, mMax;
};

// ClipScaler Definitions (add clipping to functors)

template <class T> struct LinClipScaler : public ClipScaler<T, LinScaler<T> >
{
    LinClipScaler() : ClipScaler<T, LinScaler<T> >(LinScaler<T>(), 0, 1) {}
    LinClipScaler(T inLo, T inHi, T outLo, T outHi) : ClipScaler<T, LinScaler<T> >(LinScaler<T>(inLo, inHi, outLo, outHi), outLo, outHi) {}
    LinClipScaler(const ScaleCoefficients<T>& coeff, T min, T max) : ClipScaler<T, LinScaler<T> >(LinScaler<T>(coeff), min, max) {}
};

template <class T> struct LogClipScaler : public ClipScaler<T, LogScaler<T> >
{
    LogClipScaler(T inLo, T inHi, T outLo, T outHi) : ClipScaler<T, LogScaler<T> >(log(inLo), log(inHi), outLo, outHi) {}
    LogClipScaler(const ScaleCoefficients<T>& coeff, T min, T max) : ClipScaler<T, LogScaler<T> >(LogScaler<T>(coeff), min, max) {}
};

template <class T> struct ExpClipScaler : public ClipScaler<T, ExpScaler<T> >
{
    ExpClipScaler(T inLo, T inHi, T outLo, T outHi) : ClipScaler<T, ExpScaler<T> >(ExpScaler<T>(inLo, inHi, log(outLo), log(outHi)), outLo, outHi) {}
    ExpClipScaler(const ScaleCoefficients<T>& coeff, T min, T max) : ClipScaler<T, ExpScaler<T> >(ExpScaler<T>(coeff), min, max) {}
};

// Variable Lin, Log or Exp Scaling

template <class T>struct VariScaler
{
    enum ScaleMode { kScaleLinear, kScaleExp, kScaleLog };

    VariScaler() : mMode(kScaleLinear) {}

    void set(ScaleMode mode, T inLo, T inHi, T outLo, T outHi)
    {
        mMode = mode;
        
        switch (mMode)
        {
            case kScaleLinear:  mCoefficients = LinScaler<T>(inLo, inHi, outLo, outHi);   break;
            case kScaleLog:     mCoefficients = LogScaler<T>(inLo, inHi, outLo, outHi);   break;
            case kScaleExp:     mCoefficients = LogScaler<T>(inLo, inHi, outLo, outHi);   break;
        }
    }
  
    template <class U> U scale(U x)
    {
        switch (mMode)
        {
            case kScaleLinear:  return (LinScaler<T>(mCoefficients))(x);
            case kScaleLog:     return (LogScaler<T>(mCoefficients))(x);
            case kScaleExp:     return (ExpScaler<T>(mCoefficients))(x);
        }
    }
    
    template <class U> void scale(U *output, U*input, unsigned long size)
    {
        switch (mMode)
        {
            case kScaleLinear:  (LinScaler<T>(mCoefficients))(output, input, size);       break;
            case kScaleLog:     (LogScaler<T>(mCoefficients))(output, input, size);       break;
            case kScaleExp:     (ExpScaler<T>(mCoefficients))(output, input, size);       break;
        }
    }
    
protected:
    
    ScaleMode mMode;
    ScaleCoefficients<T> mCoefficients;
};

// Variable Lin, Log or Exp Scaling With or Without Clipping

template <class T> struct VariClipScaler : public VariScaler<T>
{
    VariClipScaler() : mMin(0), mMax(1) {}
    
    void set(typename VariScaler<T>::ScaleMode mode, T inLo, T inHi, T outLo, T outHi)
    {
        mMin = std::min(outLo, outHi);
        mMax = std::max(outLo, outHi);
        VariScaler<T>::set(mode, inLo, inHi, outLo, outHi);
    }
    
    template <class U> U scaleClip(T x) { return clip(scale(x), mMin, mMax); }
    
    template <class U> void scaleClip(U *output, U *input, unsigned long size)
    {
        switch (VariScaler<T>::mMode)
        {
            case VariScaler<T>::kScaleLinear:
                (LinClipScaler<T>(VariScaler<T>::mCoefficients, mMin, mMax))(output, input, size);
                break;
            case VariScaler<T>::kScaleLog:
                (LogClipScaler<T>(VariScaler<T>::mCoefficients, mMin, mMax))(output, input, size);
                break;
            case VariScaler<T>::kScaleExp:
                (ExpClipScaler<T>(VariScaler<T>::mCoefficients, mMin, mMax))(output, input, size);
                break;
        }
    }
    
protected:
    
    T mMin, mMax;
};

// Variable Scaling and Common Conversions

template <class T> struct ScaleConvert : public VariScaler<T>
{
    void setDBToAmplitude()     { set(VariScaler<T>::kScaleExp, 0, 20, 1, 10); }
    void setAmplitudeToDB()     { set(VariScaler<T>::kScaleLog, 1, 10, 0, 20); }
    void setMIDIToFreq()        { set(VariScaler<T>::kScaleExp, 57, 69, 220, 440); }
    void setFreqToMIDI()        { set(VariScaler<T>::kScaleLog, 220, 440, 57, 69); }
    void setSemitonesToRatio()  { set(VariScaler<T>::kScaleExp, 0, 12, 1, 2); }
    void setRatioToSemitones()  { set(VariScaler<T>::kScaleLog, 1, 2, 0, 12); }
};

// Typedefs for double precision versions

typedef LinScaler<double> FrameLib_LinScaler;
typedef LogScaler<double> FrameLib_LogScaler;
typedef ExpScaler<double> FrameLib_ExpScaler;

typedef LinClipScaler<double> FrameLib_LinClipScaler;
typedef LogClipScaler<double> FrameLib_LogClipScaler;
typedef ExpClipScaler<double> FrameLib_ExpClipScaler;

typedef VariScaler<double>      FrameLib_VariScaler;
typedef VariClipScaler<double>  FrameLib_VariClipScaler;
typedef ScaleConvert<double>    FrameLib_ScaleConvert;

#endif
