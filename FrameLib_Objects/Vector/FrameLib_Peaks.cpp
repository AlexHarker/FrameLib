
#include "FrameLib_Peaks.h"

FrameLib_Peaks::FrameLib_Peaks(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, nullptr, 1, 3)
{
    mParameters.set(serialisedParameters);
}

// Info

std::string FrameLib_Peaks::objectInfo(bool verbose)
{
    return formatInfo("Finds peaks in an input frame (spectrum): "
                   "Peaks are output in terms of interpolated sample position, interpolated amplitude and peak index. "
                   "The first output is the same size as the input, other outputs are as long as the number of detected peaks.",
                   "Finds peaks in an input frame (spectrum).", verbose);
}

std::string FrameLib_Peaks::inputInfo(unsigned long idx, bool verbose)
{
    return "Input Frame / Spectrum";
}

std::string FrameLib_Peaks::outputInfo(unsigned long idx, bool verbose)
{
    if (idx == 0) return formatInfo("Peak Index - for each input sample / bin the output lists the peak it belongs to", "Peak Index", verbose);
    else if (idx == 1) return formatInfo("Peak Position - an interpolated position in samples / bins for each peak", "Peak Position", verbose);
    else return formatInfo("Peak Amplitude - an interpolated amplitude for each peak", "Peak Amplitude", verbose);
}

// Helpers

double FrameLib_Peaks::logValue(double val)
{
    val = log(val);
    
    return val < -500.0 ? -500.0 : val;
}

void FrameLib_Peaks::refinePeak(double& pos, double& amp, double posUncorrected, double vm1, double v0, double vp1)
{
    // FIX - neg values won't work with this interpolation - problem??
    
    // Take log values (avoiding values that are too low)
    
    vm1 = logValue(std::max(vm1, 0.0));
    v0 = logValue(std::max(v0, 0.0));
    vp1 = logValue(std::max(vp1, 0.0));
    
    // Parabolic interpolation
    
    double divisor = vm1 + vp1 - (2.0 * v0);
    double correction = divisor ? (0.5 * (vm1 - vp1)) / divisor : 0.0;
    
    // N.B - Leave amplitude in a log format
    
    pos = posUncorrected + correction;
    amp = exp(v0 - (0.25 * (vm1 - vp1) * correction));
}

// Process

void FrameLib_Peaks::process()
{
    // Get Input
    
    unsigned long sizeIn, sizeOut1, sizeOut2, sizeOut3;
    unsigned long nPeaks = 0;
    
    const double *input = getInput(0, &sizeIn);
    
    if (!sizeIn)
        return;
    
    // Calculate number of peaks (for now ignore peaks in the top 2 positions)
    
    if (sizeIn > 2 && (input[0] > input[1]) && (input[0] > input[2]))
        nPeaks++;
    else
    {
        if (sizeIn > 3 && (input[1] > input[2]) && (input[1] > input[3]) && (input[1] > input[0]))
            nPeaks++;
    }
    
    for (unsigned long i = 2; i < (std::max(sizeIn, 2UL) - 2); i++)
        if ((input[i] > input[i - 2]) && (input[i] > input[i - 1]) && (input[i] > input[i + 1]) && (input[i] > input[i + 2]))
            nPeaks++;
    
    // Allocate outputs
    
    requestOutputSize(0, sizeIn);
    requestOutputSize(1, nPeaks);
    requestOutputSize(2, nPeaks);
    allocateOutputs();
    
    double *output1 = getOutput(0, &sizeOut1);
    double *output2 = getOutput(1, &sizeOut2);
    double *output3 = getOutput(2, &sizeOut3);
    
    // Find and Refine Peaks (for now ignore peaks in the top 2 positions)
    
    nPeaks = 0;
    
    if (sizeOut1 && sizeOut2 && sizeOut3)
    {
        if (sizeIn > 2 && (input[0] > input[1]) && (input[0] > input[2]))
        {
            refinePeak(output2[0], output3[0], 0, input[1], input[0], input[1]);
            nPeaks++;
        }
        else
        {
            if (sizeIn > 3 && (input[1] > input[2]) && (input[1] > input[3]) && (input[1] > input[0]))
            {
                refinePeak(output2[0], output3[0], 1, input[0], input[1], input[2]);
                nPeaks++;
            }
        }
        
        for (unsigned long i = 2; i < (std::max(sizeIn, 2UL) - 2); i++)
        {
            if ((input[i] > input[i - 2]) && (input[i] > input[i - 1]) && (input[i] > input[i + 1]) && (input[i] > input[i + 2]))
            {
                refinePeak(output2[nPeaks], output3[nPeaks], i, input[i - 1], input[i], input[i + 1]);
                nPeaks++;
            }
        }
    }
    
    // Set indices
    
    unsigned long binsFilled = 0;
    unsigned long peak = 0;
    unsigned long minPoint = 0;
    
    if (nPeaks)
    {
        for (; peak < (nPeaks - 1); peak++)
        {
            unsigned long beg = truncToUInt(output2[peak]);
            unsigned long end = roundToUInt(output2[peak + 1]);
            
            double minValue = input[beg];
            minPoint = beg;
            
            for (unsigned long i = beg; i < end; i++)
            {
                if (input[i] < minValue)
                {
                    minValue = input[i];
                    minPoint = i;
                }
            }
            
            for (; binsFilled < minPoint; binsFilled++)
                output1[binsFilled] = peak;
        }
    }
    
    // Fill to the end
    
    for (; binsFilled < sizeOut1; binsFilled++)
        output1[binsFilled] = peak;
}
