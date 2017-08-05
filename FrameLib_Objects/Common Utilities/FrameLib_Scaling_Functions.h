
#ifndef FRAMELIB_SCALING_FUNCTIONS_H
#define FRAMELIB_SCALING_FUNCTIONS_H

#include <algorithm>

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
    ScaleCoefficients() : mMul(1.0), mSub(0.0) {}
    ScaleCoefficients(T mul, T sub) : mMul(mul), mSub(sub) {}

    ScaleCoefficients(T inLo, T inHi, T outLo, T outHi)
    : mMul((inLo == inHi) ? 0.0 : (outHi - outLo) / (inHi - inLo)), mSub((inLo * mMul) - outLo) {}

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

template <typename ScaleOp>
struct ClipScaler
{
    ClipScaler(ScaleOp scaler, double mMin, double mMax) : mScaler(scaler), mMin(mMin), mMax(mMax) {}
    
    template <class T> T operator()(T x) const { return clip(mScaler(x), mMin, mMax); }
    template <class T> void operator()(T *output, T *input, unsigned long size) const { scaleVector(output, input, size, *this); }
    
    const ScaleOp mScaler;
    const double mMin, mMax;
};

// ClipScaler Definitions (add clipping to functors)

typedef ClipScaler<LinScaler<double> > LinClipScaler;
typedef ClipScaler<LogScaler<double> > LogClipScaler;
typedef ClipScaler<ExpScaler<double> > ExpClipScaler;

// Variable Lin, Log or Exp Scaling

template <class T>struct FrameLib_Scaler
{
    enum ScaleMode { kScaleLinear, kScaleExp, kScaleLog };

    FrameLib_Scaler() : mMode(kScaleLinear) {}

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

template <class T> struct FrameLib_ClipScaler : public FrameLib_Scaler<T>
{
    FrameLib_ClipScaler() : mMin(0.0), mMax(0.0) {}
    
    void set(typename FrameLib_Scaler<T>::ScaleMode mode, T inLo, T inHi, T outLo, T outHi)
    {
        mMin = std::min(outLo, outHi);
        mMax = std::max(outLo, outHi);
        FrameLib_Scaler<T>::set(mode, inLo, inHi, outLo, outHi);
    }
    
    template <class U> U scaleClip(T x) { return clip(scale(x), mMin, mMax); }
    
    template <class U> void scaleClip(U *output, U *input, unsigned long size)
    {
        switch (FrameLib_Scaler<T>::mMode)
        {
            case FrameLib_Scaler<T>::kScaleLinear:
                (LinClipScaler(LinScaler<T>(FrameLib_Scaler<T>::mCoefficients), mMin, mMax))(output, input, size);
                break;
            case FrameLib_Scaler<T>::kScaleLog:
                (LogClipScaler(LogScaler<T>(FrameLib_Scaler<T>::mCoefficients), mMin, mMax))(output, input, size);
                break;
            case FrameLib_Scaler<T>::kScaleExp:
                (ExpClipScaler(ExpScaler<T>(FrameLib_Scaler<T>::mCoefficients), mMin, mMax))(output, input, size);
                break;
        }
    }
    
private:
    
    T mMin, mMax;
};

// Variable Scaling and Common Conversions

template <class T> struct FrameLib_Convert : public FrameLib_Scaler<T>
{
    void setDBToAmplitude()     { set(FrameLib_Scaler<T>::kScaleExp, 0.0, 20.0, 1.0, 10.0); }
    void setAmplitudeToDB()     { set(FrameLib_Scaler<T>::kScaleLog, 1.0, 10.0, 0.0, 20.0); }
    void setMIDIToFreq()        { set(FrameLib_Scaler<T>::kScaleExp, 57.0, 69.0, 220.0, 440.0); }
    void setFreqToMIDI()        { set(FrameLib_Scaler<T>::kScaleLog, 220.0, 440.0, 57.0, 69.0); }
    void setSemitonesToRatio()  { set(FrameLib_Scaler<T>::kScaleExp, -12.0, 12.0, 0.5, 2.0); }
    void setRatioToSemitones()  { set(FrameLib_Scaler<T>::kScaleLog, 0.5, 2.0, -12.0, 12.0); }
};

#endif
