
#ifndef FRAMELIB_WINDOW_H
#define FRAMELIB_WINDOW_H

#include "FrameLib_DSP.h"

#define W_PI			3.14159265358979323846
#define W_TWOPI			6.28318530717958647692
#define W_THREEPI		9.42477796076937971538
#define W_FOURPI		12.56637061435817295384
#define W_SIXPI			18.84955592153875943076

// FIX - review gain calculation

class FrameLib_Window : public FrameLib_Processor
{
	enum AttributeList {kWindowType, kSize, kSqrt, kCompensation, kEndPoints};
    
    enum Compensation {kOff, kLinear, kPower, kPowerOverLinear};
    enum EndPoints {kFirst, kLast, kBoth, kNone};
    enum WindowTypes {kHann, kHamming, kTriangle, kCosine, kBlackman, kBlackman62, kBlackman70, kBlackman74, kBlackman92, kBlackmanHarris, kFlatTop, kRectangle};

public:
	
    FrameLib_Window(DSPQueue *queue, FrameLib_Attributes::Serial *serialisedAttributes) : FrameLib_Processor(queue, 1, 1, 0, 0)
    {
        mAttributes.addEnum(kWindowType, "window", 0);
        mAttributes.addEnumItem(kHann, "hann");
        mAttributes.addEnumItem(kHamming, "hamming");
        mAttributes.addEnumItem(kTriangle, "triangle");
        mAttributes.addEnumItem(kCosine, "cosine");
        mAttributes.addEnumItem(kBlackman, "blackman");
        mAttributes.addEnumItem(kBlackman62, "blackman62");
        mAttributes.addEnumItem(kBlackman70, "blackman70");
        mAttributes.addEnumItem(kBlackman74, "blackman74");
        mAttributes.addEnumItem(kBlackman92, "blackman92");
        mAttributes.addEnumItem(kBlackmanHarris, "blackmanharris");
        mAttributes.addEnumItem(kFlatTop, "flattop");
        mAttributes.addEnumItem(kRectangle, "rectangle");
        
        mAttributes.addDouble(kSize, "size", 0, 1);
        mAttributes.setMin(0.0);
        
        mAttributes.addBool(kSqrt, "sqrt", FALSE, 2);
        
        mAttributes.addEnum(kCompensation, "compensation");
        mAttributes.addEnumItem(kOff, "off");
        mAttributes.addEnumItem(kLinear, "linear");
        mAttributes.addEnumItem(kPower, "power");
        mAttributes.addEnumItem(kPowerOverLinear, "powoverlin");
        
        mAttributes.addEnum(kEndPoints, "endpoints");
        mAttributes.addEnumItem(kFirst, "first");
        mAttributes.addEnumItem(kLast, "last");
        mAttributes.addEnumItem(kBoth, "both");
        mAttributes.addEnumItem(kNone, "none");
        
        mAttributes.set(serialisedAttributes);
        
        mWindow = NULL;
        mWindowType = kHann;
        mSize = 0;
        mSqrtWindow = FALSE;
        mLinearGain = 0.0;
        mPowerGain = 0.0;
    }
    
	~FrameLib_Window()
	{
        mAllocator->dealloc(mWindow);
	}
	
private:
	
	void updateWindow (unsigned long inSize, EndPoints ends)
	{
        WindowTypes windowType = (WindowTypes) mAttributes.getInt(kWindowType);
        bool sqrtWindow = mAttributes.getBool(kSqrt);
        unsigned long windowSize = mAttributes.getInt(kSize);
        
        windowSize = !windowSize ? inSize : windowSize;
        
        windowSize = ends == kBoth ? windowSize - 1 : windowSize;
        windowSize = ends == kNone ? windowSize + 1 : windowSize;
        
        if (windowSize == mSize && windowType == mWindowType && sqrtWindow == mSqrtWindow)
            return;

        if (mSize != windowSize)
        {
            mAllocator->dealloc(mWindow);
            mWindow = (double *) mAllocator->alloc(sizeof(double) * (windowSize + 2));
        }

        switch (windowType)
        {
            case kHann:
                for (unsigned long i = 0; i <= windowSize; i++)
                    mWindow[i] = 0.5 - (0.5 * cos(W_TWOPI * ((double) i / (double) windowSize)));
                break;
                
            case kHamming:
                for (unsigned long i = 0; i <= windowSize; i++)
                    mWindow[i] = 0.54347826 - (0.45652174 * cos(W_TWOPI * ((double) i / (double) windowSize)));
                break;
                
            case kTriangle:
                for (unsigned long i = 0; i <= (windowSize >> 1); i++)
                    mWindow[i] = (double) i / (double) (windowSize / 2.0);
                for (unsigned long i = (windowSize >> 1) + 1; i <= windowSize; i++)
                    mWindow[i] = (double) (((double) windowSize - 1.0) - (double) i) / (double) (windowSize / 2.0);
                break;
                
            case kCosine:
                for (unsigned long i = 0; i <= windowSize; i++)
                    mWindow[i] = sin(W_PI * ((double) i / (double) windowSize));
                break;
                
            case kBlackman:
                for (unsigned long i = 0; i <= windowSize; i++)
                    mWindow[i] = 0.42659071 - (0.49656062 * cos(W_TWOPI * ((double) i / (double) windowSize))) + (0.07684867 * cos(W_FOURPI * ((double) i / (double) windowSize)));
                break;
                
            case kBlackman62:
                for (unsigned long i = 0; i <= windowSize; i++)
                    mWindow[i] = (0.44859f - 0.49364f * cos(W_TWOPI * ((double) i / (double) windowSize)) + 0.05677f * cos(W_FOURPI * ((double) i / (double) windowSize)));
                break;
                
            case kBlackman70:
                for (unsigned long i = 0; i <= windowSize; i++)
                    mWindow[i] = (0.42323f - 0.49755f * cos(W_TWOPI * ((double) i / (double) windowSize)) + 0.07922f * cos(W_FOURPI * ((double) i / (double) windowSize)));
                break;
                
            case kBlackman74:
                for (unsigned long i = 0; i <= windowSize; i++)
                    mWindow[i] = (0.402217f - 0.49703f * cos(W_TWOPI * ((double) i / (double) windowSize)) + 0.09892f * cos(W_FOURPI * ((double) i / (double) windowSize)) - 0.00188 * cos(W_THREEPI * ((double) i / (double) windowSize)));
                break;
                
            case kBlackman92:
                for (unsigned long i = 0; i <= windowSize; i++)
                    mWindow[i] = (0.35875f - 0.48829f * cos(W_TWOPI * ((double) i / (double) windowSize)) + 0.14128f * cos(W_FOURPI * ((double) i / (double) windowSize)) - 0.01168 * cos(W_THREEPI * ((double) i / (double) windowSize)));
                break;
                
            case kBlackmanHarris:
                for (unsigned long i = 0; i <= windowSize; i++)
                    mWindow[i] = 0.35875 - (0.48829 * cos(W_TWOPI * ((double) i / (double) windowSize))) + (0.14128 * cos(W_FOURPI * ((double) i / (double) windowSize))) - (0.01168 * cos(W_SIXPI * ((double) i / (double) windowSize)));
                break;
                
            case kFlatTop:
                for (unsigned long i = 0; i <= windowSize; i++)
                    mWindow[i] = 0.2810639 - (0.5208972 * cos(W_TWOPI * ((double) i / (double) windowSize))) + (0.1980399 * cos(W_FOURPI * ((double) i / (double) windowSize)));
                break;
                
            case kRectangle:
                for (unsigned long i = 0; i <= windowSize; i++)
                    mWindow[i] = 1.0;
                break;
        }
        
        mWindow[windowSize + 1] = 0.0;
        
        if (sqrtWindow == TRUE)
        {
            for (unsigned long i = 0; i <= windowSize; i++)
                mWindow[i] = sqrt(mWindow[i]);
        }
        
        // Store window parameters
        
        mWindowType = windowType;
        mSize = windowSize;
        mSqrtWindow = sqrtWindow;
        
        // Calculate the gain of the window
        
        double linearGain = 0.0;
        double powerGain = 0.0;
        
        for (unsigned long i = 0; i <= windowSize; i++)
            linearGain += mWindow[i];
        
        mLinearGain = linearGain / (double) (windowSize + 1);

        for (unsigned long i = 0; i <= windowSize; i++)
            powerGain += mWindow[i] * mWindow[i];

        mPowerGain = powerGain / (double) (windowSize + 1);
	}
    
    double linearInterp(double pos)
    {
        unsigned long idx = (unsigned long) pos;
        
        double fract = pos - idx;
        double lo = mWindow[idx];
        double hi = mWindow[idx + 1];
        
        return lo + fract * (hi - lo);
    }
	
protected:
    
    void process ()
	{
        // Get Input
        
        unsigned long sizeIn, sizeOut, sizeFactor;
        double *input = getInput(0, &sizeIn);

        requestOutputSize(0, sizeIn);
        allocateOutputs();
        
        double *output = getOutput(0, &sizeOut);
        
        if (sizeOut)
        {
            Compensation compensate = (Compensation) mAttributes.getInt(kCompensation);
            EndPoints ends = (EndPoints) mAttributes.getInt(kEndPoints);
            
            double gain;
            
            sizeFactor = ends == kBoth ? sizeIn - 1 : sizeIn;
            sizeFactor = ends == kNone ? sizeIn + 1 : sizeFactor;

            updateWindow(sizeIn, ends);
            
            bool preIncrement = ends == kNone || ends == kLast;

            switch (compensate)
            {
                case kOff:
                    gain = 1.0;
                    break;
                
                case kLinear:
                    gain = mLinearGain;
                    break;
                    
                case kPower:
                    gain = mPowerGain;
                    break;
                    
                case kPowerOverLinear:
                    gain = mPowerGain / mLinearGain;
                    break;
            }
            
            if (mSize % sizeFactor)
            {
                double incr = (double) mSize / (double) sizeFactor;
                double pos = preIncrement ? incr : 0.0;
                
                for (unsigned long i = 0; i < sizeIn; i++, pos += incr)
                    output[i] = input[i] * gain * linearInterp(pos);
            }
            else
            {
                unsigned long incr =  mSize / sizeFactor;
                unsigned long pos = preIncrement ? incr : 0;
                
                for (unsigned long i = 0; i < sizeIn; i++, pos += incr)
                    output[i] = input[i] * gain * mWindow[pos];
            }
        }
    }
	
protected:

    double *mWindow;

    WindowTypes mWindowType;
    
    unsigned long mSize;

    bool mSqrtWindow;

    double mLinearGain;
    double mPowerGain;
};

#endif
