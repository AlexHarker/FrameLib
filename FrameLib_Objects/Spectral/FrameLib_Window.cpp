
#include "FrameLib_Window.h"

// Window Parameter Comparison

FrameLib_Window::CompareWindowParams::CompareWindowParams()
: mWindowType(FrameLib_WindowGenerator::kHann)
, mEndpoints(FrameLib_WindowGenerator::kBoth)
, mExponent(0.0)
, mParamSize(0)
, mSize(0)
{}

FrameLib_Window::CompareWindowParams::CompareWindowParams(FrameLib_WindowGenerator& generator, unsigned long size)
: mWindowType(generator.getType())
, mEndpoints(generator.getEndpoints())
, mExponent(generator.getExponent())
, mSize(size)
{
    generator.getValidatedParameters(mParams, &mParamSize);
}

bool FrameLib_Window::CompareWindowParams::operator == (const CompareWindowParams& a)
{
    bool equal = true;
    
    equal &= mWindowType == a.mWindowType;
    equal &= mExponent == a.mExponent;
    equal &= mEndpoints == a.mEndpoints;
    equal &= mSize == a.mSize;
    
    if (mParamSize == a.mParamSize)
    {
        for (unsigned long i = 0; i < mParamSize; i++)
            equal &= mParams[i] == a.mParams[i];
        
        return equal;
    }
    
    return false;
}

// Constructor

FrameLib_Window::FrameLib_Window(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, &sParamInfo, 2, 1)
, mGenerator(mParameters)
{
    mGenerator.addWindowType(kWindowType, 0);
    
    mParameters.addDouble(kSize, "size", 0, 1);
    mParameters.setMin(0.0);
    
    mGenerator.addExponent(kExponent, 2);
    mGenerator.addCompensation(kCompensation);
    mGenerator.addWindowParameters(kParameters);
    mGenerator.addEndpoints(kEndpoints);
    
    mParameters.set(serialisedParameters);
    
    mGenerator.updateParameters(getReporter());

    setParameterInput(1);
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

void FrameLib_Window::updateWindow(unsigned long sizeIn)
{
    unsigned long size = mParameters.getInt(kSize);
    size = mGenerator.sizeAdjustForEndpoints(!size ? sizeIn : size);
    
    // Check for changes and exit if none, else ersize and stoe new parameters
    
    CompareWindowParams compare(mGenerator, size);
    
    if (mCompare == compare)
        return;
    
    if (mCompare.mSize != size)
        mWindow = allocAutoArray<double>(size + 2);
    
    mCompare = compare;
    
    // Generate
    
    mGenerator.generate(mWindow, size, 0, size + 1, false);
    mWindow[size + 1] = mWindow[size];
    
    // Calculate the gain of the window
    
    unsigned long begin = mGenerator.doFirst() ? 0 : 1;
    unsigned long end = size - (mGenerator.doLast() ? 1 : 2);
    
    mGenerator.calculateGains(mWindow, begin, end);
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
    const int vecSize = SIMDLimits<double>::max_size;
    const int workSize = vecSize * 8;
    
    using VectorType = SIMDType<double, vecSize>;
    
    // Get Input
    
    unsigned long sizeIn, sizeOut;
    const double *input = getInput(0, &sizeIn);
    
    requestOutputSize(0, sizeIn);
    allocateOutputs();
    
    double *output = getOutput(0, &sizeOut);
    
    if (sizeOut)
    {
        updateWindow(sizeIn);

        unsigned long sizeFactor = mGenerator.sizeAdjustForEndpoints(sizeIn);
        unsigned long size = mCompare.mSize;

        double gain = mGenerator.gainCompensation();

        if (size % sizeFactor)
        {
            double delta = static_cast<double>(size) / static_cast<double>(sizeFactor);
            double phase = !mGenerator.doFirst() ? delta : 0.0;
            
            unsigned long i = 0;
            
            for (; i + workSize - 1 < sizeIn; i += workSize)
            {
                const VectorType *vecIn = reinterpret_cast<const VectorType *>(input + i);
                VectorType *vecOut = reinterpret_cast<VectorType *>(output + i);

                // Accumulate phases
                
                for (unsigned long j = 0; j < workSize; j++, phase += delta)
                    output[i + j] = phase;
                
                // Interpolate from the window
                
                table_read(Fetch(mWindow, size + 2), output + i, output + i, workSize, gain, kInterpLinear);
                
                // Multiply
                
                for (unsigned long j = 0; j + vecSize - 1 < workSize; j += vecSize)
                    *vecOut++ *= *vecIn++;
            }
            
            for (; i < sizeIn; i++, phase += delta)
                output[i] = input[i] * gain * linearInterp(phase);
        }
        else
        {
            unsigned long delta = size / sizeFactor;
            unsigned long phase = !mGenerator.doFirst() ? delta : 0;
            
            for (unsigned long i = 0; i < sizeIn; i++, phase += delta)
                output[i] = input[i] * gain * mWindow[phase];
        }
    }
}
