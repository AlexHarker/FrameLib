
#include "FrameLib_Convert.h"

// Constructor

FrameLib_Convert::FrameLib_Convert(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, &sParamInfo, 2, 1)
{
    mParameters.addEnum(kMode, "mode", 0);
    mParameters.addEnumItem(kDBtoA, "db->amp");
    mParameters.addEnumItem(kAtoDB, "amp->db");
    mParameters.addEnumItem(kMtoF, "midi->freq");
    mParameters.addEnumItem(kFtoM, "freq->midi");
    mParameters.addEnumItem(kSemiToRatio, "semitone->ratio");
    mParameters.addEnumItem(kRatioToSemi, "ratio->semitone");
    
    mParameters.set(serialisedParameters);
        
    setParameterInput(1);
    
    setScaling();
}

// Info

std::string FrameLib_Convert::objectInfo(bool verbose)
{
    return getInfo("Converts the input vector with the specified conversion: The output size matches the input size.",
                   "Converts the input vector with the specified conversion.", verbose);
}

std::string FrameLib_Convert::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return getInfo("Parameter Update - tagged input updates paramaeters", "Parameter Update", verbose);
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
        "semitone->ratio - converts semitone transpositons into ratios. "
        "ratio->semitone - converts from transpositon ratios in semitones. ");
}

// Helpers

void FrameLib_Convert::setScaling()
{
    switch ((Modes) mParameters.getInt(kMode))
    {
        case kDBtoA:            setDBToAmplitude();     break;
        case kAtoDB:            setAmplitudeToDB();     break;
        case kMtoF:             setMIDIToFreq();        break;
        case kFtoM:             setFreqToMIDI();        break;
        case kSemiToRatio:      setSemitonesToRatio();  break;
        case kRatioToSemi:      setRatioToSemitones();  break;
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
    double *input = getInput(0, &size);
    
    requestOutputSize(0, size);
    allocateOutputs();
    
    double *output = getOutput(0, &size);
        
    scale(output, input, size);
}
