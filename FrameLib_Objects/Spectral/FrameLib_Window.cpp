
#include "FrameLib_Window.h"

// Window Calculator

FrameLib_Window::WindowCalculator FrameLib_Window::sWindowCalculator;

// Constructor / Destructor

FrameLib_Window::FrameLib_Window(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_Processor(context, proxy, &sParamInfo, 2, 1)
{
    mParameters.addEnum(kWindowType, "window", 0);
    mParameters.addEnumItem(kHann, "hann");
    mParameters.addEnumItem(kHamming, "hamming");
    mParameters.addEnumItem(kTriangle, "triangle");
    mParameters.addEnumItem(kCosine, "cosine");
    mParameters.addEnumItem(kBlackman, "blackman");
    mParameters.addEnumItem(kBlackman62, "blackman62");
    mParameters.addEnumItem(kBlackman70, "blackman70");
    mParameters.addEnumItem(kBlackman74, "blackman74");
    mParameters.addEnumItem(kBlackman92, "blackman92");
    mParameters.addEnumItem(kBlackmanHarris, "blackmanharris");
    mParameters.addEnumItem(kFlatTop, "flattop");
    mParameters.addEnumItem(kRectangle, "rectangle");
    
    mParameters.addDouble(kSize, "size", 0, 1);
    mParameters.setMin(0.0);
    
    mParameters.addBool(kSqrt, "sqrt", false, 2);
    
    mParameters.addEnum(kCompensation, "compensate");
    mParameters.addEnumItem(kOff, "off");
    mParameters.addEnumItem(kLinear, "linear");
    mParameters.addEnumItem(kPower, "power");
    mParameters.addEnumItem(kPowerOverLinear, "powoverlin");
    
    mParameters.addEnum(kEndPoints, "endpoints");
    mParameters.addEnumItem(kFirst, "first");
    mParameters.addEnumItem(kLast, "last");
    mParameters.addEnumItem(kBoth, "both");
    mParameters.addEnumItem(kNone, "none");
    
    mParameters.set(serialisedParameters);
    
    mWindow = nullptr;
    mWindowType = kHann;
    mSize = 0;
    mSqrtWindow = false;
    mLinearGain = 0.0;
    mPowerGain = 0.0;
    mEnds = kNone;
    
    setParameterInput(1);
}

FrameLib_Window::~FrameLib_Window()
{
    dealloc(mWindow);
}

// Info

std::string FrameLib_Window::objectInfo(bool verbose)
{
    return formatInfo("Multiplies the incoming frame against a specified window: The output length will match the input length.",
                   "Multiplies the incoming frame against a specified window.", verbose);
}

std::string FrameLib_Window::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return parameterInputInfo(verbose);
    else
        return formatInfo("Input Frame", "Input Frame", idx, verbose);
}

std::string FrameLib_Window::outputInfo(unsigned long idx, bool verbose)
{
    return "Windowed Output";
}

// Parameter Info

FrameLib_Window::ParameterInfo FrameLib_Window::sParamInfo;

FrameLib_Window::ParameterInfo::ParameterInfo()
{
    add("Sets the window type.");
    add("Sets the size of the internal window. If set to 0 the window will be recalculated to match the input size (good for frequency domain applications. "
        "Otherwise an internally stored window is linearly interpolated to fit the input size (good for granular applications.");
    add("Sets whether the window should be used directly, or the square root of the window.");
    add("Sets the gain compensation used. "
        "off - no compensation is used. linear - compensate the linear gain of the window. "
        "power - compensate the power gain of the window. powoverlin - compensate by the power gain divided by the linear gain");
    add("Sets which endpoints of the window used will be non-zero for windows that start and end at zero.");
}

// Helpers

void FrameLib_Window::updateWindow(unsigned long inSize, EndPoints ends)
{
    WindowTypes windowType = (WindowTypes) mParameters.getInt(kWindowType);
    bool sqrtWindow = mParameters.getBool(kSqrt);
    uint32_t windowSize = static_cast<uint32_t>(mParameters.getInt(kSize));
    
    windowSize = !windowSize ? static_cast<uint32_t>(inSize) : windowSize;
    
    windowSize = ends == kBoth ? windowSize - 1 : windowSize;
    windowSize = ends == kNone ? windowSize + 1 : windowSize;
    
    if (windowSize == mSize && windowType == mWindowType && sqrtWindow == mSqrtWindow && ends == mEnds)
        return;
    
    if (mSize != windowSize)
    {
        dealloc(mWindow);
        mWindow = alloc<double>(windowSize + 2);
    }
    
    sWindowCalculator.calculate(windowType, mWindow, windowSize, windowSize + 1);
    
    mWindow[windowSize + 1] = mWindow[windowSize];
    
    if (sqrtWindow)
        for (unsigned long i = 0; i <= windowSize + 1; i++)
            mWindow[i] = sqrt(mWindow[i]);
    
    // Store window parameters
    
    mWindowType = windowType;
    mSize = windowSize;
    mSqrtWindow = sqrtWindow;
    mEnds = ends;
    
    // Calculate the gain of the window
    
    double linearGain = 0.0;
    double powerGain = 0.0;
    
    for (unsigned long i = 1; i < windowSize; i++)
        linearGain += mWindow[i];
    
    for (unsigned long i = 1; i < windowSize; i++)
        powerGain += mWindow[i] * mWindow[i];
    
    unsigned long sizeNorm = mSize;
    
    if (ends == kFirst || ends == kBoth)
    {
        linearGain += mWindow[0];
        powerGain += mWindow[0] * mWindow[0];
    }
    
    if (ends == kLast || ends == kBoth)
    {
        linearGain += mWindow[mSize - 1];
        powerGain += mWindow[mSize - 1] * mWindow[mSize - 1];
        sizeNorm++;
    }
    
    if (ends == kNone)
        sizeNorm--;
    
    mLinearGain = (double) sizeNorm / linearGain;
    mPowerGain = (double) sizeNorm / powerGain;
}

double FrameLib_Window::linearInterp(double pos)
{
    unsigned long idx = (unsigned long) pos;
    
    double fract = pos - idx;
    double lo = mWindow[idx];
    double hi = mWindow[idx + 1];
    
    return lo + fract * (hi - lo);
}

// Process

void FrameLib_Window::process()
{
    const int VecSize = SIMDLimits<double>::max_size;
    using VectorType = SIMDType<double, VecSize>;
    
    // Get Input
    
    unsigned long sizeIn, sizeOut, sizeFactor;
    const double *input = getInput(0, &sizeIn);
    
    requestOutputSize(0, sizeIn);
    allocateOutputs();
    
    double *output = getOutput(0, &sizeOut);
    
    if (sizeOut)
    {
        Compensation compensate = static_cast<Compensation>(mParameters.getInt(kCompensation));
        EndPoints ends = (EndPoints) mParameters.getInt(kEndPoints);
        
        sizeFactor = ends == kBoth ? sizeIn - 1 : sizeIn;
        sizeFactor = ends == kNone ? sizeIn + 1 : sizeFactor;
        
        bool preIncrement = ends == kNone || ends == kLast;
        double gain;

        updateWindow(sizeIn, ends);
        
        switch (compensate)
        {
            case kOff:                  gain = 1.0;                         break;
            case kLinear:               gain = mLinearGain;                 break;
            case kPower:                gain = mPowerGain;                  break;
            case kPowerOverLinear:      gain = mPowerGain / mLinearGain;    break;
        }
        
        if (mSize % sizeFactor)
        {
            const int workSize = VecSize * 8;
            double incr = (double) mSize / (double) sizeFactor;
            double pos = preIncrement ? incr : 0.0;
            unsigned long i = 0;
            
            if (sizeOut > (VecSize * 4))
            {
                for (; i + workSize - 1 < sizeIn; i += workSize)
                {
                    const VectorType *VecInput = reinterpret_cast<const VectorType *>(input + i);
                    VectorType *VecOutput = reinterpret_cast<VectorType *>(output + i);

                    // Accumulate positions
                
                    for (unsigned long j = 0; j < workSize; j++, pos += incr)
                        output[i + j] = pos;
                
                    // Interpolate from the window
                
                    table_read(Fetch(mWindow), output + i, output + i, workSize, 1.0, kInterpLinear);
                
                    // Multiply
                
                    for (unsigned long j = 0; j + VecSize - 1 < workSize; j += VecSize)
                        *VecOutput++ *= *VecInput++;
                }
            }
            
            for (; i < sizeIn; i++, pos += incr)
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
