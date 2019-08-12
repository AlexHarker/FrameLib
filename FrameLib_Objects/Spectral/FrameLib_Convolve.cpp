
#include "FrameLib_Convolve.h"

FrameLib_Convolve::FrameLib_Convolve(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_Processor(context, proxy, &sParamInfo, 2, 1), Spectral_Processor(*this)
{
    mParameters.addInt(kMaxLength, "maxlength", 16384, 0);
    mParameters.setMin(0);
    mParameters.setInstantiation();
    mParameters.addEnum(kMode, "mode", 2);
    mParameters.addEnumItem(kReal, "real");
    mParameters.addEnumItem(kComplex, "complex");
    mParameters.setInstantiation();
    mParameters.addEnum(kEdgeMode, "edges");
    mParameters.addEnumItem(kEdgeLinear, "linear");
    mParameters.addEnumItem(kEdgeWrap, "circular");
    mParameters.addEnumItem(kEdgeWrapCentre, "wrap");
    mParameters.addEnumItem(kEdgeFold, "fold");
    mParameters.setInstantiation();

    mParameters.set(serialisedParameters);
        
    setMaxFFTSize(mParameters.getInt(kMaxLength));
    
    mMode = static_cast<Mode>(mParameters.getInt(kMode));
    
    if (mMode  == kComplex)
        setIO(4, 2);
}

// Info

std::string FrameLib_Convolve::objectInfo(bool verbose)
{
    return formatInfo("Convolve two input frames, (using frequency domain processing internally): "
                   "The output is a frame of length M + N - 1 where M and N are the lengths of the two inputs respectively",
                   "Convolve two input frames, (using frequency domain processing internally).", verbose);
}

std::string FrameLib_Convolve::inputInfo(unsigned long idx, bool verbose)
{
    if (mMode == kReal)
        return formatInfo("Input #", "Input #", idx, verbose);
    else
    {
        unsigned long inIdx = idx / 2;
        
        if (idx % 2)
            return formatInfo("Imaginary Input #", "Imag Input #", inIdx, verbose);
        else
            return formatInfo("Real Input #", "Real Input #", inIdx, verbose);
    }
}

std::string FrameLib_Convolve::outputInfo(unsigned long idx, bool verbose)
{
    if (mMode == kReal)
        return "Correlated Output";
    
    if (idx)
        return formatInfo("Correlated Imaginary Output", "Correlated Imag Output", idx, verbose);
    else
        
        return formatInfo("Correlated Real Output", "Correlated Real Output", idx, verbose);
}

// Parameter Info

FrameLib_Convolve::ParameterInfo FrameLib_Convolve::sParamInfo;

FrameLib_Convolve::ParameterInfo::ParameterInfo()
{
    add("Sets the maximum output length. The output length will be M + N - 1 where M and N are the sizes of the two inputs respectively");
    add("Sets the type of input expected / output produced.");
}

// Process

void FrameLib_Convolve::process()
{
    EdgeMode edgeMode = static_cast<EdgeMode>(mParameters.getInt(kEdgeMode));
    
    if (mMode == kReal)
    {
        // Get Inputs
        
        unsigned long sizeIn1, sizeIn2;
        const double *input1 = getInput(0, &sizeIn1);
        const double *input2 = getInput(1, &sizeIn2);
        
        // Get Output Size
        
        unsigned long sizeOut = calcSize(sizeIn1, sizeIn2, edgeMode);
        
        // Get output
        
        requestOutputSize(0, sizeOut);
        
        if (allocateOutputs())
            convolveReal(getOutput(0, &sizeOut), input1, sizeIn1, input2, sizeIn2, edgeMode);
    }
    else
    {
        // Get Inputs
        
        unsigned long sizeR1, sizeI1, sizeR2, sizeI2;
        
        const double *inR1 = getInput(0, &sizeR1);
        const double *inI1 = getInput(1, &sizeI1);
        const double *inR2 = getInput(2, &sizeR2);
        const double *inI2 = getInput(3, &sizeI2);
        
        // Get Output Size

        unsigned long sizeOut = calcSize(std::max(sizeR1, sizeI1), std::max(sizeR2, sizeI2), edgeMode);
        
        // Get output
        
        requestOutputSize(0, sizeOut);
        requestOutputSize(1, sizeOut);

        if (allocateOutputs())
            convolve(getOutput(0, &sizeOut), getOutput(1, &sizeOut), inR1, sizeR1, inI1, sizeI1, inR2, sizeR2, inI2, sizeI2, edgeMode);
    }
}
