
#include "FrameLib_Peaks.h"
#include "FrameLib_Edges.h"
#include <algorithm>

// Distance Helper

template <typename T, typename U>
unsigned long delta(T a, U b)
{
    return static_cast<unsigned long>(std::distance(a, b)); 
}

// Constructor

FrameLib_Peaks::FrameLib_Peaks(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, &sParamInfo, 1, 3)
{
    mParameters.addInt(kNeighbours, "neighbours", 1, 0);
    mParameters.setClip(1, 4);

    mParameters.addDouble(kThreshold, "threshold", 0.0, 1);
    
    mParameters.addDouble(kPadding, "pad", 0.0, 2);

    mParameters.addEnum(kEdges, "edges", 3);
    mParameters.addEnumItem(kEdgePad, "pad");
    mParameters.addEnumItem(kEdgeExtend, "extend");
    mParameters.addEnumItem(kEdgeWrap, "wrap");
    mParameters.addEnumItem(kEdgeFold, "fold");
    mParameters.addEnumItem(kEdgeMirror, "mirror");
    
    mParameters.addEnum(kRefinement, "refine", 4);
    mParameters.addEnumItem(kOff, "off");
    mParameters.addEnumItem(kParabolic, "parabolic", true);
    mParameters.addEnumItem(kParabolicLog, "parabolic_log");
    
    mParameters.addEnum(kBoundaries, "boundaries", 4);
    mParameters.addEnumItem(kMinimum, "minimum");
    mParameters.addEnumItem(kMidpoint, "midpoint");
    
    mParameters.addBool(kAlwaysDetect, "always_detect", true, 5);
    
    mParameters.set(serialisedParameters);
    
    addParameterInput();
}

// Info

std::string FrameLib_Peaks::objectInfo(bool verbose)
{
    return formatInfo("Finds peaks in the input frame: "
                      "The input frame is searched for peaks (values greater than the surrounding values). "
                      "Each input sample is assigned to a peak. "
                      "Peaks positions and values are also interpolated and output. "
                      "The first output is the same length as the input. "
                      "Other outputs are as long as the number of detected peaks. "
                      "Peaks are detected if a value exceeds its neighbours and a fixed threshold. "
                      "The parameters control the details of the way in which peaks are defined and detected.",
                      "Finds peaks in the input frame.", verbose);
}

std::string FrameLib_Peaks::inputInfo(unsigned long idx, bool verbose)
{
    return "Input";
}

std::string FrameLib_Peaks::outputInfo(unsigned long idx, bool verbose)
{
    if (idx == 0) return formatInfo("Assigned Peaks - the peak each input sample belongs to", "Assigned Peaks", verbose);
    else if (idx == 1) return formatInfo("Peak Positions - interpolated positions per peak", "Peak Positions", verbose);
    else return formatInfo("Peak Values - interpolated values per peak", "Peak Values", verbose);
}

// Parameter Info

FrameLib_Peaks::ParameterInfo FrameLib_Peaks::sParamInfo;

FrameLib_Peaks::ParameterInfo::ParameterInfo()
{
    add("Sets the number of neighbours each side that must be exceeded in order to detect a peak.");
    add("Sets the threshold value for detecting a peak.");
    add("Sets the padding value.");
    add("Sets the edge behaviour for peak detection: "
        "pad - values beyond the edges of the input are read as the padding value. "
        "extend - the edge values are extended infinitely in either direction. "
        "wrap - values are read as wrapped or cyclical. "
        "fold - values are folded at edges without repetition of the edge values. "
        "mirror - values are mirrored at edges with the edge values repeated.");
    add("Sets the method for refining peak values: "
        "off - return the peak value without refinement. "
        "parabolic - apply parabolic interpolation to the three values around the peak. "
        "parabolic_log - apply parabolic interpolation to the log of the three values around the peak. "
        "Note that parabolic_log is suitable for interpolating linear amplitudes.");
    add("Sets the method for selecting the boundaries between peaks: "
        "minimum - boundaries are set at the minimum value between consecutive peaks. "
        "midpoint - boundaries are set to the indices halfway between consecutive peaks.");
    add("If set on at least one peak will be detected even if no values match the peak criteria. "
        "If set off then the outputs will be empty if no peak is detected.");
}

// Edges

template <class T>
void createEdges(double *output, T data, unsigned long size, int edgeSize)
{
    for (int i = 0; i < edgeSize; i++)
    {
        output[-(i + 1)] = data(-(i + 1));
        output[size + i] = data(size + i);
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
    // N.B. we assume a max of -80dB difference between samples to prevent extreme overshoot

    double limit = std::max(std::max(data[idx-1], data[idx+1]) * 0.0001, std::numeric_limits<double>::min());
    auto logLim = [&](double x) { return std::log(std::max(x, limit)); };
    
    double position, value;
    
    parabolicInterp(position, value, idx, logLim(data[idx-1]), logLim(data[idx]), logLim(data[idx+1]));
    
    positions[peak] = position;
    values[peak] = std::exp(value);
}

// Process

void FrameLib_Peaks::process()
{
    Refinements refine = mParameters.getEnum<Refinements>(kRefinement);
    Boundaries boundaries = mParameters.getEnum<Boundaries>(kBoundaries);
    Edges edges = mParameters.getEnum<Edges>(kEdges);
    
    long neighbours = mParameters.getInt(kNeighbours);
    double padValue = mParameters.getValue(kPadding);
    double threshold = mParameters.getValue(kThreshold);
    
    bool alwaysDetect = mParameters.getBool(kAlwaysDetect);
    
    static constexpr int padding = 4;
    
    // Get Input
    
    unsigned long sizeIn, sizeOut1, sizeOut2, sizeOut3;
    unsigned long nPeaks = 0;
    
    const double *input = getInput(0, &sizeIn);
    
    if (!sizeIn)
        return;
    
    auto edgeFilled = allocAutoArray<double>(sizeIn + padding * 2);
    auto indices = allocAutoArray<unsigned long>(sizeIn);
    double *data = edgeFilled + padding;

    // N.B. parabolic log interpolation can only work on +ve values
    
    if (refine == kParabolicLog)
        threshold = std::max(0.0, threshold);
    
    if (!edgeFilled || !indices)
        return;
    
    // Copy input and prepare edges
    
    copyVector(data, input, sizeIn);
    
    switch (edges)
    {
        case kEdgePad:      createEdges(data, EdgesPad(data, sizeIn, padValue), sizeIn, padding);   break;
        case kEdgeExtend:   createEdges(data, EdgesExtend(data, sizeIn), sizeIn, padding);          break;
        case kEdgeWrap:     createEdges(data, EdgesWrap(data, sizeIn), sizeIn, padding);            break;
        case kEdgeFold:     createEdges(data, EdgesFold(data, sizeIn), sizeIn, padding);            break;
        case kEdgeMirror:   createEdges(data, EdgesMirror(data, sizeIn), sizeIn, padding);          break;
    }
    
    // Find and store peaks
    
    switch (neighbours)
    {
        case 1:    nPeaks = findPeaks<checkPeak<1>>(indices, data, sizeIn, threshold);     break;
        case 2:    nPeaks = findPeaks<checkPeak<2>>(indices, data, sizeIn, threshold);     break;
        case 3:    nPeaks = findPeaks<checkPeak<3>>(indices, data, sizeIn, threshold);     break;
        case 4:    nPeaks = findPeaks<checkPeak<4>>(indices, data, sizeIn, threshold);     break;
    }
    
    // If needed create a single peak at the maximum, (place central to multiple consecutive maxima)
    
    if (!nPeaks && alwaysDetect)
    {
        double *fwd1 = data;
        double *fwd2 = data + sizeIn;
        std::reverse_iterator<double *> rev1(data + sizeIn);
        std::reverse_iterator<double *> rev2(data);
        
        unsigned long max = delta(data, std::max_element(fwd1, fwd2));
        unsigned long beg = delta(fwd1, std::find(fwd1, fwd2, data[max]));
        unsigned long end = sizeIn - (delta(rev1, std::find(rev1, rev2, data[max])) + 1UL);
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
        case kOff:              refinePeaks<refineNone>(output2, output3, data, indices, nPeaks);           break;
        case kParabolic:        refinePeaks<refineParabolic>(output2, output3, data, indices, nPeaks);      break;
        case kParabolicLog:     refinePeaks<refineParabolicLog>(output2, output3, data, indices, nPeaks);   break;
    }
    
    // Determine which samples belong to which peak
    
    for (unsigned long peak = 0, samples = 0; peak < nPeaks; peak++)
    {
        unsigned long peakEnd = sizeIn;
        
        if (peak != nPeaks - 1)
        {
            switch (boundaries)
            {
                case kMinimum:
                {
                    auto it = std::min_element(data + indices[peak] + 1, data + indices[peak + 1]);
                    peakEnd = delta(data, it);
                    break;
                }
                
                case kMidpoint:
                {
                    peakEnd = static_cast<unsigned long>(std::ceil((output2[peak] + output2[peak + 1]) / 2.0));
                    break;
                }
            }
        }
        
        for (; samples < peakEnd; samples++)
            output1[samples] = peak;
    }
}
