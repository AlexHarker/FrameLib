
#include "FrameLib_Shift.h"
#include "FrameLib_Edges.h"

// Constructor

FrameLib_Shift::FrameLib_Shift(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, &sParamInfo, 2, 1)
{
    mParameters.addDouble(kShift, "shift", 0.0, 0);
    
    mParameters.addDouble(kPadding, "pad", 0.0, 1);
    
    mParameters.addEnum(kEdges, "edges", 2);
    mParameters.addEnumItem(kEdgePad, "pad");
    mParameters.addEnumItem(kEdgeExtend, "extend");
    mParameters.addEnumItem(kEdgeWrap, "wrap");
    mParameters.addEnumItem(kEdgeFold, "fold");
    mParameters.addEnumItem(kEdgeMirror, "mirror");

    mParameters.addEnum(kUnits, "units", 3);
    mParameters.addEnumItem(kSamples, "samples");
    mParameters.addEnumItem(kRatio, "ratios");
    
    mParameters.set(serialisedParameters);
    
    setParameterInput(1);
}

// Info

std::string FrameLib_Shift::objectInfo(bool verbose)
{
    return formatInfo("Shifts samples within a frame either with padding, or with wrapping: "
                      "The output is the same length as the input. Output can be shifted left or right in comparison to the input. "
                      "The mode parameter is used to set either padding or wrapping behaviour. "
                      "When in pad mode, values are moved out on one side and padded on the other. "
                      "When in wrap mode the shift is cyclical.",
                      "Shifts samples within a frame either with padding, or or with wrapping.", verbose);
}

std::string FrameLib_Shift::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return parameterInputInfo(verbose);
    else
        return "Input";
}

std::string FrameLib_Shift::outputInfo(unsigned long idx, bool verbose)
{
    return "Output";
}

// Parameter Info

FrameLib_Shift::ParameterInfo FrameLib_Shift::sParamInfo;

FrameLib_Shift::ParameterInfo::ParameterInfo()
{
    add("Sets the shift amount (-ve for left shift / +ve for right shift).");
    add("Sets the padding value.");
    add("Sets the shift mode. pad - values are replaced with the padding value. wrap - values are wrapped cyclically.");
    add("Sets units for the shift (samples or ratio of the input length [-1 to 1]).");
}

long limit(long x, long a, long b) { return std::max(std::min(x, b), a); }

template <class T>
void doShift(T edgeReader, double *out, const double *in, unsigned long size, long shift)
{
    const long loop1 = limit(shift, 0, size);
    const long loop2 = size - limit(-shift, 0, size);

    long i = 0;

    for (; i < loop1; i++)
        out[i] = edgeReader(i - shift);
            
    for (; i < loop2; i++)
        out[i] = in[i - shift];
        
    for (; i < size; i++)
        out[i] = edgeReader(i - shift);
}

// Process

void FrameLib_Shift::process()
{
    // Get Input
    
    long shift;
    unsigned long sizeIn, sizeOut;
    const double *input = getInput(0, &sizeIn);
    double pad = mParameters.getValue(kPadding);
    Units units = mParameters.getEnum<Units>(kUnits);
    
    requestOutputSize(0, sizeIn);
    
    if (!allocateOutputs())
        return;
    
    double *output = getOutput(0, &sizeOut);
    
    // Calculate shift amount
    
    if (units == kSamples)
        shift = mParameters.getInt(kShift);
    else
        shift = roundToInt(mParameters.getValue(kShift) * sizeIn);
    
    switch (mParameters.getEnum<Edges>(kEdges))
    {
        case kEdgePad:      doShift(EdgesPad(input, sizeIn, pad), output, input, sizeIn, shift); break;
        case kEdgeExtend:   doShift(EdgesExtend(input, sizeIn), output, input, sizeIn, shift);   break;
        case kEdgeWrap:     doShift(EdgesWrap(input, sizeIn), output, input, sizeIn, shift);     break;
        case kEdgeFold:     doShift(EdgesFold(input, sizeIn), output, input, sizeIn, shift);     break;
        case kEdgeMirror:   doShift(EdgesMirror(input, sizeIn), output, input, sizeIn, shift);   break;
            
    }
}
