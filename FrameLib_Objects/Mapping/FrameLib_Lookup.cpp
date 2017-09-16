
#include "FrameLib_Lookup.h"

FrameLib_Lookup::FrameLib_Lookup(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, NULL, 2, 1)
{
    // FIX - loads of different mode options here (mapping of positions + padding values
    
    mParameters.addEnum(kMode, "mode");
    mParameters.addEnumItem(kZero, "zero");
    mParameters.addEnumItem(kZero, "zero");
    mParameters.setInstantiation();
    mParameters.addEnum(kInterpolation, "interp");
    mParameters.addEnumItem(kHermite, "hermite");
    mParameters.addEnumItem(kBSpline, "bspline");
    mParameters.addEnumItem(kLagrange, "lagrange");
    mParameters.addEnumItem(kLinear, "linear");
    mParameters.addEnumItem(kNone, "none");
    mParameters.setInstantiation();
    
    mParameters.set(serialisedParameters);
    
    mMode = mParameters.getInt(kMode);
    mInterpMode = mParameters.getInt(kInterpolation);
    
    inputMode(1, false, false, false);
}

// Info

std::string FrameLib_Lookup::objectInfo(bool verbose)
{
    return getInfo("Use one frame as a lookup table for another: The left input frame is used to lookup values from the last values received as the right input frame. "
                   "The output is the same size as the left input, which is interpreted as a set of sample positions used to read the right input. "
                   "Only the left input triggers output.",
                   "Use one frame as a lookup table for another.", verbose);
}

std::string FrameLib_Lookup::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return getInfo("Frame for Table - values are retrieved from this frame / does not trigger output", "Frame for Table", verbose);
    else
        return getInfo("Values to Lookup - interpreted as sample positions into the table / right input", "Values to Lookup", verbose);
}

std::string FrameLib_Lookup::outputInfo(unsigned long idx, bool verbose)
{
    return getInfo("Output Frame - values after look up", "Output Frame", verbose);
}

// Parameter Info

FrameLib_Lookup::ParameterInfo FrameLib_Lookup::sParamInfo;

FrameLib_Lookup::ParameterInfo::ParameterInfo()
{
    add("Sets the mode for values requested out of range:"
        "zero - values out of range are treated as zeroes."
        "clip - values out of range are clipped to the end points of the frame used for lookup");
    add("Sets the interpolation mode: "
        "hermite - cubic hermite interpolation. "
        "bspline - cubic bspline interpolation. "
        "lagrange - cubic lagrange interpolation. "
        "linear - linear interpolation.");
}

// Process

void FrameLib_Lookup::process()
{
    unsigned long sizeIn1, sizeIn2, sizeOut;
    
    double *input1 = getInput(0, &sizeIn1);
    double *input2 = getInput(1, &sizeIn2);
    
    requestOutputSize(0, sizeIn1);
    allocateOutputs();
    
    double *output = getOutput(0, &sizeOut);
    
    enum InterpType interpType;
    
    switch (mInterpMode)
    {
        case kNone:         interpType = kInterpNone;               break;
        case kLinear:       interpType = kInterpLinear;             break;
        case kLagrange:     interpType = kInterpCubicLagrange;      break;
        case kHermite:      interpType = kInterpCubicHermite;       break;
        case kBSpline:      interpType = kInterpCubicBSpline;       break;
    }
    
    switch (mMode)
    {
        case kZero:     table_read(FetchZero(input2, sizeIn2), output, input1, sizeIn1, 1.0, interpType);   break;
        case kClip:     table_read(FetchClip(input2, sizeIn2), output, input1, sizeIn1, 1.0, interpType);   break;
        //case kWrap:   table_read(FetchWrap(input2, sizeIn2), output, input1, sizeIn1, 1.0, interpType);   break;
        //case kPad:   table_read(FetchPad(input2, sizeIn2, padValue), output, input1, sizeIn1, 1.0, interpType);   break;
    }
}
