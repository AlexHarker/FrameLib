
#include "FrameLib_Map.h"

// Constructor

FrameLib_Map::FrameLib_Map(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, &sParamInfo, 2, 1)
{
    mParameters.addEnum(kMode, "mode", 0);
    mParameters.addEnumItem(kLinear, "linear");
    mParameters.addEnumItem(kLog, "log");
    mParameters.addEnumItem(kExp, "exp");
    mParameters.addEnumItem(kDB, "db");
    mParameters.addEnumItem(kInvDB, "invdb");
    mParameters.addEnumItem(kTranspose, "transpose");
    mParameters.addEnumItem(kInvTranspose, "invtranspose");
    
    mParameters.addDouble(kInLo, "inlo", 0., 1);
    mParameters.addDouble(kInHi, "inhi", 1., 2);
    mParameters.addDouble(kOutLo, "outlo", 0., 3);
    mParameters.addDouble(kOutHi, "outhi", 1., 4);
    
    mParameters.addBool(kClip, "clip", true, 5);
    
    mParameters.set(serialisedParameters);
        
    setParameterInput(1);
    
    setScaling();
}

// Info

std::string FrameLib_Map::objectInfo(bool verbose)
{
    return getInfo("Maps values in the input via a given scaling to corresponding output values: The output size matches the input size. "
                   "Scaling maps a specified range of values in the input to a specified range of output values. Different modes of scaling are offered. "
                   "Values may be optionally constrained within the specified ranges.",
                   "Maps values in the input via a given scaling to corresponding output values.", verbose);
}

std::string FrameLib_Map::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return getInfo("Parameter Update - tagged input updates paramaeters", "Parameter Update", verbose);
    else
        return "Input Frame";
}

std::string FrameLib_Map::outputInfo(unsigned long idx, bool verbose)
{
    return "Mapped Output";
}

// Parameter Info

FrameLib_Map::ParameterInfo FrameLib_Map::sParamInfo;

FrameLib_Map::ParameterInfo::ParameterInfo()
{
    add("Sets the type of output scaling: linear / log / exp - scaling as specified. "
        "db / invdb - output / input respectively are set in dB but scaled as gain values. "
        "transpose / invtranspose - output / input respectively are set in semitones but scaled as ratios for transposition.");
    add("Sets the low input value.");
    add("Sets the high input value.");
    add("Sets the low output value.");
    add("Sets the high output value.");
    add("If true then the output is clipped between the low and high output value.");
}

// Helpers

void FrameLib_Map::setScaling()
{
    double inLo = mParameters.getValue(kInLo);
    double inHi = mParameters.getValue(kInHi);
    double outLo = mParameters.getValue(kOutLo);
    double outHi = mParameters.getValue(kOutHi);
    
    ScaleMode mode;
    
    switch ((Modes) mParameters.getInt(kMode))
    {
        case kLinear:
            mode = kScaleLinear;
            break;
        case kLog:
            mode = kScaleLog;
            break;
        case kExp:
            mode = kScaleExp;
            break;
        case kDB:
            mode = kScaleExp;
            outLo = dbtoa(outLo);
            outHi = dbtoa(outHi);
            break;
        case kInvDB:
            mode = kScaleLog;
            inLo = dbtoa(inLo);
            inHi = dbtoa(inHi);
            break;
        case kTranspose:
            mode = kScaleExp;
            outLo = semitonesToRatio(outLo);
            outHi = semitonesToRatio(outHi);
            break;
        case kInvTranspose:
            mode = kScaleLog;
            inLo = semitonesToRatio(inLo);
            inHi = semitonesToRatio(inHi);
            break;
    }
    
    FrameLib_VariClipScaler::set(mode, inLo, inHi, outLo, outHi);
}

// Update and Process

void FrameLib_Map::update()
{
    setScaling();
}

void FrameLib_Map::process()
{
    unsigned long size;
    double *input = getInput(0, &size);
    
    requestOutputSize(0, size);
    allocateOutputs();
    
    double *output = getOutput(0, &size);
        
    if (mParameters.getBool(kClip))
        scaleClip(output, input, size);
    else
        scale(output, input, size);
}
