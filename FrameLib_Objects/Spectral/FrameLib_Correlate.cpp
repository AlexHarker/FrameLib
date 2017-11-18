
#include "FrameLib_Correlate.h"
#include "FrameLib_Spectral_Functions.h"

FrameLib_Correlate::FrameLib_Correlate(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, owner, &sParamInfo, 2, 1), Spectral(context)
{
    mParameters.addInt(kMaxLength, "maxlength", 16384, 0);
    mParameters.setMin(0);
    mParameters.setInstantiation();
    mParameters.addEnum(kMode, "mode", 2);
    mParameters.addEnumItem(kReal, "real");
    mParameters.addEnumItem(kComplex, "complex");
    mParameters.setInstantiation();
    
    mParameters.set(serialisedParameters);
        
    setMaxFFTSize(mParameters.getInt(kMaxLength));
    
    mMode = static_cast<Mode>(mParameters.getInt(kMode));
    
    if (mMode  == kComplex)
        setIO(4, 2);
}

// Info

std::string FrameLib_Correlate::objectInfo(bool verbose)
{
    return formatInfo("Correlate two real frames together, (using frequency domain processing internally): "
                   "The result will be a frame of M + N - 1 where M and N are the sizes of the two inputs respectively",
                   "Correlate two time domain frames together, (using frequency domain processing internally).", verbose);
}

std::string FrameLib_Correlate::inputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Input #", "Input #", idx, verbose);
}

std::string FrameLib_Correlate::outputInfo(unsigned long idx, bool verbose)
{
    return "Correlated Output";
}

// Parameter Info

FrameLib_Correlate::ParameterInfo FrameLib_Correlate::sParamInfo;

FrameLib_Correlate::ParameterInfo::ParameterInfo()
{
    add("Sets the maximum output length. The output length will be M + N - 1 where M and N are the sizes of the two inputs respectively");
}

// Process

void FrameLib_Correlate::process()
{
    if (mMode == kReal)
    {
        // Get Inputs
        
        unsigned long sizeIn1, sizeIn2;
        const double *input1 = getInput(0, &sizeIn1);
        const double *input2 = getInput(1, &sizeIn2);
        
        // Get Output Size
        
        unsigned long sizeOut = sizeIn1 + sizeIn2 - 1;
        
        if (!sizeIn1 || !sizeIn2 || !checkSize(sizeOut))
            sizeOut = 0;
        
        // Get output
        
        requestOutputSize(0, sizeOut);
        
        if (allocateOutputs())
            correlateReal(getOutput(0, &sizeOut), input1, sizeIn1, input2, sizeIn2);
    }
    else
    {
        // Get Inputs
        
        unsigned long sizeR1, sizeI1, sizeR2, sizeI2;
        
        const double *inR1 = getInput(0, &sizeR1);
        const double *inI1 = getInput(1, &sizeI1);
        const double *inR2 = getInput(0, &sizeR2);
        const double *inI2 = getInput(1, &sizeI2);
        
        // Get Output Size
        
        unsigned long sizeIn1 = std::max(sizeR1, sizeI1);
        unsigned long sizeIn2 = std::max(sizeR2, sizeI2);
        unsigned long sizeOut = sizeIn1 + sizeIn2 - 1;
        
        if (!sizeIn1 || !sizeIn2 || !checkSize(sizeOut))
            sizeOut = 0;
        
        // Get output
        
        requestOutputSize(0, sizeOut);
        requestOutputSize(1, sizeOut);
        
        if (allocateOutputs())
            correlate(getOutput(0, &sizeOut), getOutput(1, &sizeOut), inR1, sizeR1, inI1, sizeI1, inR2, sizeR2, inI2, sizeI2);
    }
}
