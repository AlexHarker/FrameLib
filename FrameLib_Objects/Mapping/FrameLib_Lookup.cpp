
#include "FrameLib_Lookup.h"

FrameLib_Lookup::FrameLib_Lookup(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, &sParamInfo, 2, 1)
{    
    mParameters.addEnum(kScale, "scale", 0);
    mParameters.addEnumItem(kSamples, "samples");
    mParameters.addEnumItem(kNormalised, "normalised");
    mParameters.addEnumItem(kBipolar, "bipolar");
    
    mParameters.addEnum(kEdges, "edges", 1);
    mParameters.addEnumItem(kZeroPad, "zero");
    mParameters.addEnumItem(kExtend, "extend");
    mParameters.addEnumItem(kWrap, "wrap");
    mParameters.addEnumItem(kFold, "fold");
    mParameters.addEnumItem(kMirror, "mirror");
    mParameters.addEnumItem(kExtrapolate, "extrapolate", true);

    mParameters.addBool(kBound, "bound", true, 2);
    
    mParameters.addEnum(kInterpolation, "interp", 3);
    mParameters.addEnumItem(kNone, "none");
    mParameters.addEnumItem(kLinear, "linear");
    mParameters.addEnumItem(kHermite, "hermite", true);
    mParameters.addEnumItem(kBSpline, "bspline");
    mParameters.addEnumItem(kLagrange, "lagrange");
    
    mParameters.set(serialisedParameters);
    
    setInputMode(1, false, false, false);
    
    addParameterInput();
}

// Info

std::string FrameLib_Lookup::objectInfo(bool verbose)
{
    return formatInfo("Lookup values from one input using a table provided at another: "
                      "The first input is interpreted as the positions to read from the table at the second. "
                      "The output is the same length as the frame at the first input. "
                      "The scale, edges and bound parameters control the mapping of the input to the table. "
                      "The lookup can be performed with different types of interpolation (or none). "
                      "Only the first input triggers output.",
                      "Lookup values from one input using a table provided at another.", verbose);
}

std::string FrameLib_Lookup::inputInfo(unsigned long idx, bool verbose)
{
    switch (idx)
    {
        case 0: return formatInfo("Input - values to be looked up", "Input", verbose);
        case 1: return formatInfo("Table Input - does not trigger output", "Table Input", verbose);
        default: return parameterInputInfo(verbose);
    }
}

std::string FrameLib_Lookup::outputInfo(unsigned long idx, bool verbose)
{
    return "Output";
}

// Parameter Info

FrameLib_Lookup::ParameterInfo FrameLib_Lookup::sParamInfo;

FrameLib_Lookup::ParameterInfo::ParameterInfo()
{
    add("Sets the scaling of the input for lookup: "
        "samples - the input is read as sample indices for reading from the table. "
        "normalised - the input range [0 to 1] is mapped across the table. "
        "bipolar - the input range [-1 to 1] is mapped across the table. "
        "Note that the edge parameter is also accounted for normalised and bipolar modes. "
        "This adjusts the scaling to work sensibly with cyclical modes.");
    add("Sets the edge behaviour for both interpolation and lookup: "
        "zero - values beyond the edges of the table are treated as zeros. "
        "extend - the edge values are extended infinitely in either direction. "
        "wrap - values are treated as wrapped or cyclical. "
        "fold - values are folded (as spectral folding) with no repetition of the edge values. "
        "mirror - values are mirrored with the edge values repeated. "
        "extrapolate - values out of range are extrapolated via interpolation.");
    add("Sets whether lookup is bounded to the edges of the table, or can extend beyond it.");
    add("Sets the interpolation mode: "
        "none - no interpolation. "
        "linear - linear interpolation. "
        "hermite - cubic hermite interpolation. "
        "bspline - cubic bspline interpolation. "
        "lagrange - cubic lagrange interpolation.");
}

// Process

void FrameLib_Lookup::process()
{
    double scaleFactor;
    
    EdgeType edges = mParameters.getEnum<EdgeType>(kEdges);
    Scales scale = mParameters.getEnum<Scales>(kScale);
    InterpType interp = kInterpNone;
    bool bound = mParameters.getBool(kBound);
    bool adjustScaling = edges == kWrap || edges == kMirror;

    unsigned long sizeIn1, sizeIn2, sizeOut;
    
    const double *input1 = getInput(0, &sizeIn1);
    const double *input2 = getInput(1, &sizeIn2);
    const double *positions = input1;
    AutoArray<double> temp;

    // Before allocating check that we have a table and if not produce an empty frame
    
    sizeIn1 = sizeIn2 ? sizeIn1 : 0;
    requestOutputSize(0, sizeIn1);
    allocateOutputs();
    
    double *output = getOutput(0, &sizeOut);
    
    // Set interp[olation
    
    switch (mParameters.getEnum<Interpolation>(kInterpolation))
    {
        case kNone:         interp = kInterpNone;               break;
        case kLinear:       interp = kInterpLinear;             break;
        case kHermite:      interp = kInterpCubicHermite;       break;
        case kBSpline:      interp = kInterpCubicBSpline;       break;
        case kLagrange:     interp = kInterpCubicLagrange;      break;
    }
    
    // Deal with scaling
    
    if (scale != kSamples)
    {
        temp = allocAutoArray<double>(sizeIn1);
        positions = temp;
        
        if (!temp)
        {
            zeroVector(output, sizeOut);
            return;
        }
    }
    
    switch (scale)
    {
        case kSamples:
            break;
            
        case kNormalised:
            scaleFactor = static_cast<double>(sizeIn2 - (adjustScaling ? 0 : 1));
            
            for (unsigned long i = 0; i < sizeIn1; i++)
                temp[i] = input1[i] * scaleFactor;
            break;
            
        case kBipolar:
            scaleFactor = static_cast<double>(sizeIn2 - (adjustScaling ? 0 : 1)) / 2.0;
            
            for (unsigned long i = 0; i < sizeIn1; i++)
                temp[i] = (input1[i] + 1.0) * scaleFactor;
            break;
    }
    
    // Calculate output
    
    table_read_edges(Fetcher(input2, sizeIn2), output, positions, sizeIn1, 1.0, interp, edges, bound);
}
