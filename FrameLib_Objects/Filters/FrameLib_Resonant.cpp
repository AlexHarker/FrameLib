
#include "FrameLib_Resonant.h"

// Filter Class

// Set Parameters

void FrameLib_Resonant::Resonant::setParams(double freq, double reson, double samplingRate)
{
    double frad = cos(freq * M_PI * 2.0 / samplingRate);
    double res = 0.882497 * exp(reson * 0.125);
    
    scl = (frad * res) * -2.0;
    r2 = res * res;
}

// Filter Calculation

double FrameLib_Resonant::Resonant::calculateFilter(double x)
{
    x = x * ((scl + r2) + 1.0);
    double y = x - ((scl * y1) + (r2 * y2));
    
    y2 = y1;
    y1 = y;
    
    return y;
}

// Main Class

// Constructor

FrameLib_Resonant::FrameLib_Resonant(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, owner, &sParamInfo, 2, 1)
{
    mParameters.addDouble(kFreq, "freq", 0.0, 0);
    mParameters.setMin(0.0);
    
    mParameters.addDouble(kReson, "reson", 0.0, 1);
    mParameters.setClip(0.0, 1.0);
    
    mParameters.addEnum(kMode, "mode", 2);
    mParameters.addEnumItem(kLPF, "lpf");
    mParameters.addEnumItem(kHPF, "hpf");
    
    mParameters.set(serialisedParameters);
    
    setParameterInput(1);
}

// Info

std::string FrameLib_Resonant::objectInfo(bool verbose)
{
    return formatInfo("Filters input frames using a resonant filter: The size of the output is equal to the input.",
                   "Filters input frames using a resonant filter.", verbose);
}

std::string FrameLib_Resonant::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return parameterInputInfo(verbose);
    else
        return formatInfo("Input Frame - input to be triggered", "Input Frame", verbose);
}

std::string FrameLib_Resonant::outputInfo(unsigned long idx, bool verbose)
{
    return "Frame of Filtered Values";
}

// Parameter Info

FrameLib_Resonant::ParameterInfo FrameLib_Resonant::sParamInfo;

FrameLib_Resonant::ParameterInfo::ParameterInfo()
{
    add("Sets the filter cutoff frequency.");
    add("Sets the filter resonance [0-1].");
    add("Sets the filter mode.");
}

// Process

void FrameLib_Resonant::process()
{
    Resonant filter;
    Modes mode = static_cast<Modes>(mParameters.getValue(kMode));
    
    bool staticParams = true;
    
    double freq = mParameters.getValue(kFreq);
    double reson = mParameters.getValue(kReson);
    
    // Get Input
    
    unsigned long sizeIn, sizeOut;
    double *input = getInput(0, &sizeIn);
    
    requestOutputSize(0, sizeIn);
    allocateOutputs();
    
    double *output = getOutput(0, &sizeOut);
    
    filter.setParams(freq, reson, mSamplingRate);
    
    if (staticParams)
    {
        switch (mode)
        {
            case kLPF:
                for (unsigned long i = 0; i < sizeOut; i++)
                    output[i] = filter.LPF(input[i]);
                break;
                
            case kHPF:
                for (unsigned long i = 0; i < sizeOut; i++)
                    output[i] = filter.HPF(input[i]);
                break;
        }
    }
}
