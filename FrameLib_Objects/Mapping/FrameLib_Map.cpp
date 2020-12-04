
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
    mParameters.addEnumItem(kTranspose, "transpose");
    mParameters.addEnumItem(kInvPow, "inverse_pow");
    mParameters.addEnumItem(kInvDB, "inverse_db");
    mParameters.addEnumItem(kInvTranspose, "inverse_transpose");
    
    mParameters.addDouble(kIn1, "in_1", 0., 1);
    mParameters.addDouble(kIn2, "in_2", 1., 2);
    mParameters.addDouble(kOut1, "out_1", 0., 3);
    mParameters.addDouble(kOut2, "out_2", 1., 4);
    
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
    add("Sets the first) (low) input value.");
    add("Sets the second (high) input value.");
    add("Sets the first (low) output value.");
    add("Sets the second (high) output value.");
    add("If true then the output is clipped to the range between the two output values.");
    add("Sets the exponent for the pow mode.");
}

// Helpers

void FrameLib_Map::setScaling()
{
    auto sToR = [](double s) { return semitonesToRatio(s); };
    
    double in1 = mParameters.getValue(kIn1);
    double in2 = mParameters.getValue(kIn2);
    double out1 = mParameters.getValue(kOut1);
    double out2 = mParameters.getValue(kOut2);
    double exponent = mParameters.getValue(kExponent);
    
    switch (mParameters.getEnum<Modes>(kMode))
    {
        case kLinear:       mScaler.setLin(in1, in2, out1, out2);                       break;
        case kLog:          mScaler.setLog(in1, in2, out1, out2);                       break;
        case kExp:          mScaler.setExp(in1, in2, out1, out2);                       break;
        case kPow:          mScaler.setPow(in1, in2, out1, out2, exponent);             break;
        case kDB:           mScaler.setExp(in1, in2, dbToAmp(out1), dbToAmp(out2));     break;
        case kTranspose:    mScaler.setExp(in1, in2, sToR(out1), sToR(out2));           break;
        case kInvPow:       mScaler.setPow(in1, in2, out1, out2, 1.0 / exponent);       break;
        case kInvDB:        mScaler.setLog(dbToAmp(in1), dbToAmp(in2), out1, out2);     break;
        case kInvTranspose: mScaler.setLog(sToR(in1), sToR(in2), out1, out2);           break;
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
