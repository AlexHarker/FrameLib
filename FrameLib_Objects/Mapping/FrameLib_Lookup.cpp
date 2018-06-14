
#include "FrameLib_Lookup.h"

FrameLib_Lookup::FrameLib_Lookup(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_Processor(context, proxy, nullptr, 2, 1)
{
    // FIX - loads of different mode options here (mapping of positions + padding values
    
    mParameters.addEnum(kMode, "mode");
    mParameters.addEnumItem(kZero, "zero");
    mParameters.addEnumItem(kClip, "clip");
    
    mParameters.addEnum(kInterpolation, "interp");
    mParameters.addEnumItem(kHermite, "hermite");
    mParameters.addEnumItem(kBSpline, "bspline");
    mParameters.addEnumItem(kLagrange, "lagrange");
    mParameters.addEnumItem(kLinear, "linear");
    mParameters.addEnumItem(kNone, "none");

    mParameters.addEnum(kScaling, "scale");
    mParameters.addEnumItem(kSamples, "samples");
    mParameters.addEnumItem(kNormalised, "normalised");
    mParameters.addEnumItem(kBipolar, "bipolar");
    
    mParameters.set(serialisedParameters);
    
    setInputMode(1, false, false, false);
    
    addParameterInput();
}

// Info

std::string FrameLib_Lookup::objectInfo(bool verbose)
{
    return formatInfo("Use one frame as a lookup table for another: The left input frame is used to lookup values from the last values received as the right input frame. "
                   "The output is the same size as the left input, which is interpreted as a set of sample positions used to read the right input. "
                   "Only the left input triggers output.",
                   "Use one frame as a lookup table for another.", verbose);
}

std::string FrameLib_Lookup::inputInfo(unsigned long idx, bool verbose)
{
    switch (idx)
    {
        case 0: return formatInfo("Values to Lookup - interpreted as sample positions into the table / right input", "Values to Lookup", verbose);
        case 1: return formatInfo("Frame for Table - values are retrieved from this frame / does not trigger output", "Frame for Table", verbose);
        case 2: return parameterInputInfo(verbose);
        default: return "Unknown input";
    }
}

std::string FrameLib_Lookup::outputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Output Frame - values after look up", "Output Frame", verbose);
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
    
    const double *input1 = getInput(0, &sizeIn1);
    const double *input2 = getInput(1, &sizeIn2);
    
    requestOutputSize(0, sizeIn1);
    allocateOutputs();
    
    double *output = getOutput(0, &sizeOut);
    const double *positions = input1;
    double *temp = nullptr;
    double scaleFactor;
    
    Scaling scaling = (Scaling) mParameters.getInt(kScaling);
    InterpType interpType = kInterpNone;
    
    switch ((Interpolation) mParameters.getInt(kInterpolation))
    {
        case kNone:         interpType = kInterpNone;               break;
        case kLinear:       interpType = kInterpLinear;             break;
        case kLagrange:     interpType = kInterpCubicLagrange;      break;
        case kHermite:      interpType = kInterpCubicHermite;       break;
        case kBSpline:      interpType = kInterpCubicBSpline;       break;
    }
    
    if (scaling != kSamples)
    {
        temp = alloc<double>(sizeIn1);
        positions = temp;
        
        if (!temp)
        {
            zeroVector(output, sizeOut);
            return;
        }
    }
    
    switch (scaling)
    {
        case kSamples:
            break;
            
        case kNormalised:
            scaleFactor = static_cast<double>(sizeIn2 - 1);
            
            for (unsigned long i = 0; i < sizeIn1; i++)
                temp[i] = input1[i] * scaleFactor;
            break;
            
        case kBipolar:
            scaleFactor = static_cast<double>(sizeIn2 - 1) / 2.0;
            
            for (unsigned long i = 0; i < sizeIn1; i++)
                temp[i] = (input1[i] + 1.0) * scaleFactor;
            break;
    }
    
    switch ((Mode) mParameters.getInt(kMode))
    {
        case kZero:     table_read(FetchZero(input2, sizeIn2), output, positions, sizeIn1, 1.0, interpType);   break;
        case kClip:     table_read(FetchClip(input2, sizeIn2), output, positions, sizeIn1, 1.0, interpType);   break;
        //case kWrap:   table_read(FetchWrap(input2, sizeIn2), output, input1, sizeIn1, 1.0, interpType);   break;
        //case kPad:   table_read(FetchPad(input2, sizeIn2, padValue), output, input1, sizeIn1, 1.0, interpType);   break;
    }
    
     if (scaling != kSamples)
         dealloc(temp);
}
