
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
    return formatInfo("Converts the input vector according to a specified conversion: "
                      "The output length matches the input length.",
                      "Converts the input vector with the specified conversion.", verbose);
}

std::string FrameLib_Convert::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return parameterInputInfo(verbose);
    else
        return "Input";
}

std::string FrameLib_Convert::outputInfo(unsigned long idx, bool verbose)
{
    return "Output";
}

// Parameter Info

FrameLib_Convert::ParameterInfo FrameLib_Convert::sParamInfo;

FrameLib_Convert::ParameterInfo::ParameterInfo()
{
    add("Sets the mode of conversion: "
        "db->amp - converts from decibels to linear amplitudes. "
        "amp->db - converts from linear amplitudes to decibels. "
        "midi->freq - converts from midi notes to frequencies. "
        "freq->mid - converts from frequencies to midi notes. "
        "semitones->ratios - converts semitone transpositions to ratios. "
        "ratios->semitones - converts from ratios to semitone transpositions. "
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
