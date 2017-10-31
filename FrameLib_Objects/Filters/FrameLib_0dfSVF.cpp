
#include "FrameLib_0dfSVF.h"

// Filter Class

// Filter Types

double FrameLib_0dfSVF::ZeroDelayFeedbackSVF::HPF(double x)
{
    calculateFilter(x);
    return hp;
}

double FrameLib_0dfSVF::ZeroDelayFeedbackSVF::BPF(double x)
{
    calculateFilter(x);
    return bp;
}

double FrameLib_0dfSVF::ZeroDelayFeedbackSVF::LPF(double x)
{
    calculateFilter(x);
    return lp;
}

// Set Parameters

void FrameLib_0dfSVF::ZeroDelayFeedbackSVF::setParams(double freq, double resonance, double samplingRate)
{
    r = 1.0 - resonance;
    r = r < 0.005 ? 0.005 : r;
    r = r > 1.0 ? 1.0 : r;
    double srConst = 0.5 / samplingRate;
    g = ((2.0 * samplingRate) * tan((freq * FILTER_TWO_PI) * srConst) * srConst);
}

// Filter Calculation

void FrameLib_0dfSVF::ZeroDelayFeedbackSVF::calculateFilter(double x)
{
    // Compute highpass then bandpass  by applying 1st integrator to highpass output and update state
    
    hp = (x - (2.0 * r * s1) - (g * s1) - s2) / (1.0 + (2.0 * r * g) + (g * g));
    bp = g * hp + s1;
    s1 = g * hp + bp;
    
    // Compute lowpass by applying 2nd integrator to bandpass output and update state
    
    lp = g * bp + s2;
    s2 = g * bp + lp;
}

// Main Class

// Constructor

FrameLib_0dfSVF::FrameLib_0dfSVF(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, owner, &sParamInfo, 2, 1)
{
    mParameters.addDouble(kFreq, "freq", 500.0, 0);
    mParameters.setMin(0.0);
    
    mParameters.addDouble(kReson, "reson", 0.0, 1);
    mParameters.setClip(0.0, 1.0);
    
    mParameters.addEnum(kMode, "mode", 2);
    mParameters.addEnumItem(kLPF, "lpf");
    mParameters.addEnumItem(kBPF, "bpf");
    mParameters.addEnumItem(kHPF, "hpf");
        
    mParameters.set(serialisedParameters);
    
    setParameterInput(1);
}

// Info

std::string FrameLib_0dfSVF::objectInfo(bool verbose)
{
    return formatInfo("Filters input frames using a zero delay state variable filter: The size of the output is equal to the input.",
                   "Filters input frames using a zero delay state variable filter.", verbose);
}

std::string FrameLib_0dfSVF::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return parameterInputInfo(verbose);
    else
        return formatInfo("Input Frame - input to be triggered", "Input Frame", verbose);
}

std::string FrameLib_0dfSVF::outputInfo(unsigned long idx, bool verbose)
{
    return "Frame of Filtered Values";
}

// Parameter Info

FrameLib_0dfSVF::ParameterInfo FrameLib_0dfSVF::sParamInfo;

FrameLib_0dfSVF::ParameterInfo::ParameterInfo()
{
    add("Sets the filter cutoff frequency.");
    add("Sets the filter resonance [0-1].");
    add("Sets the filter mode.");
}

// Process

void FrameLib_0dfSVF::process()
{
    ZeroDelayFeedbackSVF filter;
    Modes mode = (Modes) mParameters.getValue(kMode);
    
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
                
            case kBPF:
                for (unsigned long i = 0; i < sizeOut; i++)
                    output[i] = filter.BPF(input[i]);
                break;
                
            case kHPF:
                for (unsigned long i = 0; i < sizeOut; i++)
                    output[i] = filter.HPF(input[i]);
                break;
        }
    }
}
