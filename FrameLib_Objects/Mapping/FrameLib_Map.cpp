
#include "FrameLib_Map.h"

// Constructor

FrameLib_Map::FrameLib_Map(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, &sParamInfo, 2, 1)
{
    mParameters.addEnum(kMode, "mode", 0);
    mParameters.addEnumItem(kLinear, "linear");
    mParameters.addEnumItem(kLog, "log");
    mParameters.addEnumItem(kExp, "exp");
    mParameters.addEnumItem(kPow, "pow");
    mParameters.addEnumItem(kDB, "db");
    mParameters.addEnumItem(kInvDB, "invdb");
    mParameters.addEnumItem(kTranspose, "transpose");
    mParameters.addEnumItem(kInvTranspose, "invtranspose");
    
    mParameters.addDouble(kInLo, "inlo", 0., 1);
    mParameters.addDouble(kInHi, "inhi", 1., 2);
    mParameters.addDouble(kOutLo, "outlo", 0., 3);
    mParameters.addDouble(kOutHi, "outhi", 1., 4);
    mParameters.addBool(kClip, "clip", true, 5);

    mParameters.addDouble(kExponent, "exponent", 1.);

    mParameters.set(serialisedParameters);
        
    setParameterInput(1);
    
    setScaling();
}

// Info

std::string FrameLib_Map::objectInfo(bool verbose)
{
    return formatInfo("Maps values in the input via a given scaling to corresponding output values: The output size matches the input size. "
                   "Scaling maps a specified range of values in the input to a specified range of output values. Different modes of scaling are offered. "
                   "Values may be optionally constrained within the specified ranges.",
                   "Maps values in the input via a given scaling to corresponding output values.", verbose);
}

std::string FrameLib_Map::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return parameterInputInfo(verbose);
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
        "pow - scale the input range to [0-1], apply the specified exponent and then scale to the output range. "
        "db / invdb - output / input respectively are set in dB but scaled as gain values. "
        "transpose / invtranspose - output / input respectively are set in semitones but scaled as ratios for transposition.");
    add("Sets the low input value.");
    add("Sets the high input value.");
    add("Sets the low output value.");
    add("Sets the high output value.");
    add("Sets the exponent for the pow mode.");
    add("If true then the output is clipped between the low and high output value.");
}

// Helpers

void FrameLib_Map::setScaling()
{
    double inLo = mParameters.getValue(kInLo);
    double inHi = mParameters.getValue(kInHi);
    double outLo = mParameters.getValue(kOutLo);
    double outHi = mParameters.getValue(kOutHi);
    double exponent = mParameters.getValue(kExponent);
    
    switch (mParameters.getEnum<Modes>(kMode))
    {
        case kLinear:           mScaler.setLin(inLo, inHi, outLo, outHi);                                       break;
        case kLog:              mScaler.setLog(inLo, inHi, outLo, outHi);                                       break;
        case kExp:              mScaler.setExp(inLo, inHi, outLo, outHi);                                       break;
        case kPow:              mScaler.setPow(inLo, inHi, outLo, outHi, exponent);                             break;
        case kDB:               mScaler.setExp(inLo, inHi, dbtoa(outLo), dbtoa(outHi));                         break;
        case kInvDB:            mScaler.setLog(dbtoa(inLo), dbtoa(inHi), outLo, outHi);                         break;
        case kTranspose:        mScaler.setExp(inLo, inHi, semitonesToRatio(outLo), semitonesToRatio(outHi));   break;
        case kInvTranspose:     mScaler.setLog(semitonesToRatio(inLo), semitonesToRatio(inHi), outLo, outHi);   break;
    }
}

// Update and Process

void FrameLib_Map::update()
{
    setScaling();
}

void FrameLib_Map::process()
{
    unsigned long size;
    const double *input = getInput(0, &size);
    
    requestOutputSize(0, size);
    allocateOutputs();
    
    double *output = getOutput(0, &size);
        
    if (mParameters.getBool(kClip))
        mScaler.scaleClip(output, input, size);
    else
        mScaler.scale(output, input, size);
}
