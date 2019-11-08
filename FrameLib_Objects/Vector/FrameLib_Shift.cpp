
#include "FrameLib_Shift.h"

// Constructor

FrameLib_Shift::FrameLib_Shift(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_Processor(context, proxy, &sParamInfo, 2, 1)
{
    mParameters.addDouble(kShift, "shift", 0.0, 0);
    
    mParameters.addDouble(kPadding, "pad", 0.0, 1);
    
    mParameters.addEnum(kMode, "mode", 2);
    mParameters.addEnumItem(kPad, "pad");
    mParameters.addEnumItem(kWrap, "wrap");
    
    mParameters.addEnum(kUnits, "units", 3);
    mParameters.addEnumItem(kSamples, "samples");
    mParameters.addEnumItem(kRatio, "ratios");
    
    mParameters.set(serialisedParameters);
    
    setParameterInput(1);
}

// Info

std::string FrameLib_Shift::objectInfo(bool verbose)
{
    return formatInfo("Shifts samples within a frame either with padding, or with wrapping: "
                      "The output is the same length as the input. Output can be shifted left or right in comparison to the input. "
                      "The mode parameter is used to set either padding or wrapping behaviour. "
                      "When in pad mode, values are moved out on one side and padded on the other. "
                      "When in wrap mode the shift is cyclical.",
                      "Shifts samples within a frame either with padding, or or with wrapping.", verbose);
}

std::string FrameLib_Shift::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return parameterInputInfo(verbose);
    else
        return "Input";
}

std::string FrameLib_Shift::outputInfo(unsigned long idx, bool verbose)
{
    return "Output";
}

// Parameter Info

FrameLib_Shift::ParameterInfo FrameLib_Shift::sParamInfo;

FrameLib_Shift::ParameterInfo::ParameterInfo()
{
    add("Sets the shift amount (-ve for left shift / +ve for right shift).");
    add("Sets the padding value.");
    add("Sets the shift mode. pad - values are replaced with the padding value. wrap - values are wrapped cyclically.");
    add("Sets units for the shift (samples or ratio of the input length [-1 to 1]).");
}

// Process

void FrameLib_Shift::process()
{
    // Get Input
    
    long shift;
    unsigned long sizeIn, sizeOut;
    const double *input = getInput(0, &sizeIn);
    double padValue = mParameters.getValue(kPadding);
    Units units = (Units) mParameters.getInt(kUnits);
    
    requestOutputSize(0, sizeIn);
    allocateOutputs();
    
    double *output = getOutput(0, &sizeOut);
    
    // Calculate shift amount
    
    if (units == kSamples)
        shift = mParameters.getInt(kShift);
    else
        shift = roundToInt(mParameters.getValue(kShift) * sizeIn);
    
    unsigned long absShift = std::abs(shift);
    
    // Copy to output (with padding if necessary)
    
    if (sizeOut)
    {
        if ((Modes) mParameters.getInt(kMode) == kWrap)
        {
            // Wrap in case of large shift sizes
            
            absShift = absShift % sizeIn;
            
            if (shift >= 0)
            {
                copyVector(output, input + (sizeIn - absShift), absShift);
                copyVector((output + absShift), input, (sizeIn - absShift));
            }
            else
            {
                copyVector(output, input + absShift, (sizeIn - absShift));
                copyVector((output + (sizeIn - absShift)), input, absShift);
            }
        }
        else
        {
            // Limit abs shift to maximum (all padding - doesn't matter which direction)
            
            absShift = (absShift >= sizeIn) ? sizeIn : absShift;
            
            if (shift >= 0)
            {
                for (unsigned long i = 0; i < absShift; i++)
                    output[i] = padValue;
                copyVector((output + absShift), input, (sizeIn - absShift));
            }
            else
            {
                copyVector(output, input + absShift, (sizeIn - absShift));
                for (unsigned long i = (sizeIn - absShift); i < sizeIn; i++)
                    output[i] = padValue;
            }
        }
    }
}

