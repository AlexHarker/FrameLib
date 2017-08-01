
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
    
    switch ((Modes) mParameters.getInt(kMode))
    {
        case kLinear:
            mMode = kScaleLinear;
            break;
        case kLog:
            mMode = kScaleLog;
            break;
        case kExp:
            mMode = kScaleExp;
            break;
        case kDB:
            mMode = kScaleExp;
            outLo = pow(10, outLo / 20.);
            outHi = pow(10, outHi / 20.);
            break;
        case kInvDB:
            mMode = kScaleLog;
            inLo = pow(10, inLo / 20.);
            inHi = pow(10, inHi / 20.);
            break;
        case kTranspose:
            mMode = kScaleExp;
            outLo = pow(2, outLo / 12.);
            outHi = pow(2, outHi / 12.);
            break;
        case kInvTranspose:
            mMode = kScaleLog;
            inLo = pow(2, inLo / 12.);
            inHi = pow(2, inHi / 12.);
            break;
    }
    
    mMin = outLo < outHi ? outLo : outHi;
    mMax = outLo < outHi ? outHi : outLo;
    
    // Calculate simplified linear scaling values
    
    switch (mMode)
    {
        case kScaleLinear:
            break;
            
        case kScaleLog:
            inLo = log(inLo);
            inHi = log(inHi);
            break;
            
        case kScaleExp:
            outLo = log(outLo);
            outHi = log(outHi);
            break;
    }
    
    double mul = (inLo == inHi) ? 0.0 : (outHi - outLo) / (inHi - inLo);
    double sub = (inLo * mul) - outLo;
    
    mMul = mul;
    mSub = sub;
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
    
    double mul = mMul;
    double sub = mSub;
    
    switch (mMode)
    {
        case kScaleLinear:
            
            for (unsigned long i = 0; i < size; i++)
                output[i] = (input[i] * mul) - sub;
            break;
            
        case kScaleLog:
            for (unsigned long i = 0; i < size; i++)
                output[i] = log(input[i]) * mul - sub;
            break;
            
        case kScaleExp:
            for (unsigned long i = 0; i < size; i++)
                output[i] = exp((input[i] * mul) - sub);
            break;
    }
    
    if (mParameters.getBool(kClip))
    {
        double min = mMin;
        double max = mMax;
        
        for (unsigned long i = 0; i < size; i++)
            output[i] = (output[i] > max) ? max : (output[i] < min) ? min : output[i];
    }
    
}
