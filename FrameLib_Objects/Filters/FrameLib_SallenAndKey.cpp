
#include "FrameLib_SallenAndKey.h"

// Filter Class

// Filter Types

double FrameLib_SallenAndKey::SallenAndKey::HPF(double x)
{
    calculateFilter(x);
    return x - v2;
}

double FrameLib_SallenAndKey::SallenAndKey::BPF(double x)
{
    calculateFilter(x);
    return v1 - v2;
}

double FrameLib_SallenAndKey::SallenAndKey::LPF(double x)
{
    calculateFilter(x);
    return v2;
}

// Set Parameters

void FrameLib_SallenAndKey::SallenAndKey::setParams(double freq, double resonance, double samplingRate)
{
    double g = tan(M_PI * (freq / samplingRate));
    double gp1 = 1.0 + g;
    
    k = 2.0 * resonance;
    a0 = 1.0 / ((gp1 * gp1) - (g * k));
    a1 = k * a0;
    a2 = gp1 * a0;
    a3 = g * a2;
    a4 = 1.0 / gp1;
    a5 = g * a4;
}

// Filter Calculation

void FrameLib_SallenAndKey::SallenAndKey::calculateFilter(double x)
{
    v1 = (a1 * ic2eq) + (a2 * ic1eq) + (a3 * x);
    v2 = (a4 * ic2eq) + (a5 * v1);
    
    ic1eq = (2 * (v1 - (k * v2))) - ic1eq;
    ic2eq = (2 * v2) - ic2eq;
}

// Main Class

// Constructor

FrameLib_SallenAndKey::FrameLib_SallenAndKey(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, 2, 1)
{
    mParameters.addDouble(kFreq, "freq", 0.0, 0);
    mParameters.setMin(0.0);
    
    mParameters.addDouble(kReson, "reson", 0.0, 1);
    mParameters.setClip(0.0, 1.0);
    
    mParameters.addEnum(kMode, "mode", 2);
    mParameters.addEnumItem(kLPF, "lpf");
    mParameters.addEnumItem(kBPF, "bpf");
    mParameters.addEnumItem(kHPF, "hpf");
    
    mParameters.setInfo(&sParamInfo);

    mParameters.set(serialisedParameters);
        
    setParameterInput(1);
}

// Info

const char *FrameLib_SallenAndKey::objectInfo(bool verbose)
{
    return getInfo("Filters input frames using a sallen and key filter: The size of the output is equal to the input.",
                   "Filters input frames using a sallen and key filter.", verbose);
}

const char *FrameLib_SallenAndKey::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return getInfo("Parameter Update - tagged input updates paramaeters", "Parameter Update", verbose);
    else
        return getInfo("Input Frame - input to be triggered", "Input Frame", verbose);
}

const char *FrameLib_SallenAndKey::outputInfo(unsigned long idx, bool verbose)
{
    return "Frame of Filtered Values";
}

// Parameter Info

FrameLib_SallenAndKey::ParameterInfo FrameLib_SallenAndKey::sParamInfo;

FrameLib_SallenAndKey::ParameterInfo::ParameterInfo()
{
    add("Sets the filter cutoff frequency.");
    add("Sets the filter resonance [0-1].");
    add("Sets the filter mode.");
}

// Process

void FrameLib_SallenAndKey::process()
{
    SallenAndKey filter;
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