
#include "FrameLib_Peaks.h"
#include <algorithm>

FrameLib_Peaks::FrameLib_Peaks(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, nullptr, 1, 3)
{
    mParameters.addEnum(kCriteria, "criteria", 0);
    mParameters.addEnumItem(kOneNeighbour, "one");
    mParameters.addEnumItem(kTwoNeighbours, "two", true);
    mParameters.addEnumItem(kThreeNeighbours, "three");
    mParameters.addEnumItem(kFourNeighbours, "four");

    mParameters.addDouble(kThreshold, "threshold", 0.0, 1);
    
    mParameters.addEnum(kEdges, "edges", 2);
    mParameters.addEnumItem(kPad, "pad");
    mParameters.addEnumItem(kExtend, "extend");
    mParameters.addEnumItem(kWrap, "wrap");
    mParameters.addEnumItem(kFold, "fold");
    mParameters.addEnumItem(kMirror, "mirror");

    mParameters.addDouble(kPadding, "padding", 0.0, 3);
                          
    mParameters.addEnum(kRefinement, "refine", 4);
    mParameters.addEnumItem(kNone, "none");
    mParameters.addEnumItem(kParabolic, "parabolic", true);
    mParameters.addEnumItem(kParabolicLog, "parabolic_log");
    
    mParameters.addEnum(kBoundary, "boundary", 4);
    mParameters.addEnumItem(kMinimum, "minimum");
    mParameters.addEnumItem(kMidpoint, "midpoint");
    
    mParameters.addBool(kAlwaysDetect, "always_detect", true, 5);
    
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

// Edges

void padEdges(double *data, unsigned long size, unsigned long edgeSize, double padValue)
{
    for (unsigned long i = 0; i < edgeSize; i++)
    {
        data[-(i + 1)] = padValue;
        data[size + i] = padValue;
    }
}

void extendEdges(double *data, unsigned long size, unsigned long edgeSize)
{
    for (unsigned long i = 0; i < edgeSize; i++)
    {
        data[-(i + 1)] = data[0];
        data[size + i] = data[size - 1];
    }
}

void wrapEdges(double *data, unsigned long size, unsigned long edgeSize)
{
    for (unsigned long i = 0; i < edgeSize; i++)
    {
        data[-(i + 1)] = data[size - (i + 1)];
        data[size + i] = data[i];
    }
}

void foldEdges(double *data, unsigned long size, unsigned long edgeSize)
{
    if (size == 1)
    {
        extendEdges(data, size, edgeSize);
        return;
    }
    
    for (unsigned long i = 0; i < edgeSize; i++)
    {
        data[-(i + 1)] = data[i + 1];
        data[size + i] = data[size - (i + 2)];
    }
}

void mirrorEdges(double *data, unsigned long size, unsigned long edgeSize)
{
    for (unsigned long i = 0; i < edgeSize; i++)
    {
        data[-(i + 1)] = data[i];
        data[size + i] = data[size - (i + 1)];
    }
}

// Peak Finding

template <bool Func(const double *, double, unsigned long)>
unsigned long findPeaks(unsigned long *peaks, const double *data, unsigned long size, double threshold)
{
    unsigned long nPeaks = 0;
    
    for (unsigned long i = 0; i < size; i++)
        if (Func(data, threshold, i))
            peaks[nPeaks++] = i;
    
    return nPeaks;
}

template <int N>
bool checkPeak(const double *data, double threshold, unsigned long i)
{
    return checkPeak<N-1>(data, threshold, i) && data[i] > data[i-N] && data[i] > data[i+N];
}

template <>
bool checkPeak<1>(const double *data, double threshold, unsigned long i)
{
    return data[i] > threshold && data[i] > data[i-1] && data[i] > data[i+1];
}

// Refinement

template <void Func(double *, double *, const double *, unsigned long, unsigned long)>
void refinePeaks(double *positions, double *values, const double *data, unsigned long *peaks, unsigned long nPeaks)
{
    for (unsigned long i = 0; i < nPeaks; i++)
        Func(positions, values, data, i, peaks[i]);
}

void parabolicInterp(double& position, double& value, double idx, double vm1, double v_0, double vp1)
{
    const double divisor = vm1 + vp1 - (2.0 * v_0);
    const double correction = divisor ? (0.5 * (vm1 - vp1)) / divisor : 0.0;
    
    position = idx + correction;
    value = v_0 - (0.25 * (vm1 - vp1) * correction);
}

void refineNone(double *positions, double *values, const double *data, unsigned long peak, unsigned long idx)
{
    positions[peak] = idx;
    values[peak] = data[idx];
}

void refineParabolic(double *positions, double *values, const double *data, unsigned long peak, unsigned long idx)
{
    parabolicInterp(positions[peak], values[peak], idx, data[idx-1], data[idx], data[idx+1]);
}

void refineParabolicLog(double *positions, double *values, const double *data, unsigned long peak, unsigned long idx)
{
    // Take log values (avoiding values that are too low) - doesn't work for negative values
    // FIX - clip value is arbitrary and creates major overshoot
    auto logLim = [](double x) { return std::max(-1000.0, log(std::max(x, 0.0))); };
    double position, value;
    
    parabolicInterp(position, value, idx, logLim(data[idx-1]), logLim(data[idx]), logLim(data[idx+1]));
    
    positions[peak] = position;
    values[peak] = exp(value);
}

// Process

void FrameLib_Peaks::process()
{
    Criteria criterion = static_cast<Criteria>(mParameters.getInt(kCriteria));
    Refinements refine = static_cast<Refinements>(mParameters.getInt(kRefinement));
    Boundaries boundary = static_cast<Boundaries>(mParameters.getInt(kBoundary));
    Edges edges = static_cast<Edges>(mParameters.getInt(kEdges));
    
    double padValue = mParameters.getValue(kPadding);
    double threshold = mParameters.getValue(kThreshold);
    
    bool alwaysDetect = mParameters.getBool(kAlwaysDetect);
    
    const static int padding = 4;
    
    // Get Input
    
    unsigned long sizeIn, sizeOut1, sizeOut2, sizeOut3;
    unsigned long nPeaks = 0;
    
    const double *input = getInput(0, &sizeIn);
    
    if (!sizeIn)
        return;
    
    double *edgeFilled = alloc<double>(sizeIn + padding * 2);
    double *data = edgeFilled + padding;
    unsigned long *indices = alloc<unsigned long>(sizeIn);

    // N.B. parabolic log interpolation can only work on +ve values
    
    if (refine == kParabolicLog)
        threshold = std::max(0.0, threshold);
    
    if (!edgeFilled || !indices)
        return;
    
    // Copy input and prepare edges
    
    copyVector(data, input, sizeIn);
    
    switch (edges)
    {
        case kPad:      padEdges(data, sizeIn, padding, padValue);  break;
        case kExtend:   extendEdges(data, sizeIn, padding);         break;
        case kWrap:     wrapEdges(data, sizeIn, padding);           break;
        case kFold:     foldEdges(data, sizeIn, padding);           break;
        case kMirror:   mirrorEdges(data, sizeIn, padding);         break;
    }
    
    // Find and store peaks
    
    switch (criterion)
    {
        case kOneNeighbour:     nPeaks = findPeaks<checkPeak<1>>(indices, data, sizeIn, threshold);     break;
        case kTwoNeighbours:    nPeaks = findPeaks<checkPeak<2>>(indices, data, sizeIn, threshold);     break;
        case kThreeNeighbours:  nPeaks = findPeaks<checkPeak<3>>(indices, data, sizeIn, threshold);     break;
        case kFourNeighbours:   nPeaks = findPeaks<checkPeak<4>>(indices, data, sizeIn, threshold);     break;
    }
    
    // If needed reate a single peak at the maximum, (place central to multiple consecutive maxima)
    
    if (!nPeaks && alwaysDetect)
    {
        double *fwd1 = data;
        double *fwd2 = data + sizeIn;
        std::reverse_iterator<double *> rev1(data + sizeIn);
        std::reverse_iterator<double *> rev2(data);
        
        unsigned long max = std::distance(data, std::max_element(fwd1, fwd2));
        unsigned long beg = std::distance(fwd1, std::find(fwd1, fwd2, data[max]));
        unsigned long end = sizeIn  - (std::distance(rev1, std::find(rev1, rev2, data[max])) + 1);
        unsigned long centre = (beg + end) >> 1;
        
        indices[nPeaks++] = data[centre] == data[max] ? centre : max;
    }
        
    // Allocate outputs
    
    requestOutputSize(0, nPeaks ? sizeIn : 0);
    requestOutputSize(1, nPeaks);
    requestOutputSize(2, nPeaks);
    
    if (!allocateOutputs())
        return;
    
    double *output1 = getOutput(0, &sizeOut1);
    double *output2 = getOutput(1, &sizeOut2);
    double *output3 = getOutput(2, &sizeOut3);
    
    // Refine peaks

    switch (refine)
    {
        case kNone:             refinePeaks<refineNone>(output2, output3, data, indices, nPeaks);           break;
        case kParabolic:        refinePeaks<refineParabolic>(output2, output3, data, indices, nPeaks);      break;
        case kParabolicLog:     refinePeaks<refineParabolicLog>(output2, output3, data, indices, nPeaks);   break;
    }
    
    // Determine which samples belong to which peak
    
    for (unsigned long peak = 0, samples = 0; peak < nPeaks; peak++)
    {
        unsigned long peakEnd = sizeIn;
        
        if (peak != nPeaks - 1)
        {
            switch (boundary)
            {
                case kMinimum:
                {
                    auto it = std::min_element(data + indices[peak] + 1, data + indices[peak + 1]);
                    peakEnd = std::distance(data, it);
                    break;
                }
                
                case kMidpoint:
                {
                    // FIX - check
                    peakEnd = roundToUInt((output2[peak] + output2[peak + 1]) / 2.0);
                    break;
                }
            }
        }
        
        for (; samples < peakEnd; samples++)
            output1[samples] = peak;
    }
    
    dealloc(edgeFilled);
    dealloc(indices);
}
