
#include "FrameLib_Convert.h"

// Constructor

FrameLib_Convert::FrameLib_Convert(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, &sParamInfo, 2, 1)
{
    mParameters.addEnum(kMode, "mode", 0);
    mParameters.addEnumItem(kDBtoA, "db->amp");
    mParameters.addEnumItem(kAtoDB, "amp->db");
    mParameters.addEnumItem(kMtoF, "midi->freq");
    mParameters.addEnumItem(kFtoM, "freq->midi");
    mParameters.addEnumItem(kSemiToRatio, "semitones->ratios");
    mParameters.addEnumItem(kRatioToSemi, "ratios->semitones");
    mParameters.addEnumItem(kDegToRad, "degrees->radians");
    mParameters.addEnumItem(kRadToDeg, "radians->degrees");
    
    mParameters.set(serialisedParameters);
        
    setParameterInput(1);
    
    setScaling();
}

// Info

std::string FrameLib_Convert::objectInfo(bool verbose)
{
    return formatInfo("Converts the input vector with the specified conversion: The output size matches the input size.",
                   "Converts the input vector with the specified conversion.", verbose);
}

std::string FrameLib_Convert::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return parameterInputInfo(verbose);
    else
        return "Input Frame";
}

std::string FrameLib_Convert::outputInfo(unsigned long idx, bool verbose)
{
    return "Converted Output";
}

// Parameter Info

FrameLib_Convert::ParameterInfo FrameLib_Convert::sParamInfo;

FrameLib_Convert::ParameterInfo::ParameterInfo()
{
    add("Sets the mode of conversion: "
        "db->amp - converts from decibels into linear amplitudes. "
        "amp->db - converts from linear amplitudes int decibels. "
        "midi->freq - converts from midi notes into frequencies. "
        "freq->mid - converts from frequencies into midi notes. "
        "semitones->ratios - converts semitone transpositons into ratios. "
        "ratios->semitones - converts from transpositon ratios in semitones. "
        "degrees->radians - converts from degress to radians. "
        "radians->degrees - converts from radians to degrees.");
}

// Helpers

void FrameLib_Convert::setScaling()
{
    switch (mParameters.getEnum<Modes>(kMode))
    {
        case kDBtoA:            mConvertor.setDBToAmplitude();         break;
        case kAtoDB:            mConvertor.setAmplitudeToDB();         break;
        case kMtoF:             mConvertor.setMIDIToFreq();            break;
        case kFtoM:             mConvertor.setFreqToMIDI();            break;
        case kSemiToRatio:      mConvertor.setSemitonesToRatios();     break;
        case kRatioToSemi:      mConvertor.setRatiosToSemitones();     break;
        case kDegToRad:         mConvertor.setDegreesToRadians();      break;
        case kRadToDeg:         mConvertor.setRadiansToDegrees();      break;
    }
}

// Update and Process

void FrameLib_Convert::update()
{
    if (mParameters.changed(kMode))
        setScaling();
}

void FrameLib_Convert::process()
{
    unsigned long size;
    const double *input = getInput(0, &size);
    
    requestOutputSize(0, size);
    allocateOutputs();
    
    double *output = getOutput(0, &size);
        
    mConvertor.scale(output, input, size);
}
