
#include "FrameLib_Convolve.h"

FrameLib_Convolve::FrameLib_Convolve(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, &sParamInfo, 2, 1)
, mProcessor(*this)
{
    mParameters.addInt(kMaxLength, "maxlength", 16384, 0);
    mParameters.setMin(0);
    mParameters.setInstantiation();
    mParameters.addEnum(kMode, "mode", 1);
    mParameters.addEnumItem(kReal, "real");
    mParameters.addEnumItem(kComplex, "complex");
    mParameters.setInstantiation();
    mParameters.addEnum(kEdgeMode, "edges", 2);
    mParameters.addEnumItem(EdgeMode::kEdgeLinear, "linear");
    mParameters.addEnumItem(EdgeMode::kEdgeWrap, "circular");
    mParameters.addEnumItem(EdgeMode::kEdgeWrapCentre, "wrap");
    mParameters.addEnumItem(EdgeMode::kEdgeFold, "fold");
    mParameters.addEnumItem(EdgeMode::kEdgeFoldRepeat, "mirror");
    mParameters.setInstantiation();

    mParameters.set(serialisedParameters);
        
    mProcessor.set_max_fft_size(mParameters.getInt(kMaxLength));
    
    mMode = static_cast<Modes>(mParameters.getInt(kMode));
    
    if (mMode  == kComplex)
        setIO(4, 2);
}

// Info

std::string FrameLib_Convolve::objectInfo(bool verbose)
{
    return formatInfo("Convolve two inputs (either real or complex): "
                      "The output length depends on the edges parameter, which sets the edge behaviour. "
                      "It will be no longer than M + N - 1 where M and N are the lengths of the two inputs respectively.",
                      "Convolve two inputs (either real or complex).", verbose);
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
        return "Output";
    
    if (idx)
        return formatInfo("Imaginary Output", "Imag Output", idx, verbose);
    else
        
        return formatInfo("Real Output", "Real Output", idx, verbose);
}

// Parameter Info

FrameLib_Convolve::ParameterInfo FrameLib_Convolve::sParamInfo;

FrameLib_Convolve::ParameterInfo::ParameterInfo()
{
    add("Sets the maximum processing length. "
        "The processing length is M + N - 1 where M and N are the sizes of the two inputs respectively.");
    add("Sets the type of input and output mode.");
    add("Sets the edge behaviour of the correlation process. "
        "linear - the output is the full processing length without wrapping or folding. "
        "circular - output length is the maximum of the input lengths with excess wrapped/added to the beginning. "
        "wrap - similar to circular mode, but rotated such that wrapping occurs equally at both ends. "
        "fold - as wrap but folding at the edges. "
        "mirror - as fold but with repeated edge values.");
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
        
        // Get Output Size / Check for Processing Size Errors
        
        unsigned long sizeOut = static_cast<unsigned long>(mProcessor.convolved_size(sizeIn1, sizeIn2, edgeMode));
        
        if (sizeOut == 0 && sizeIn1 && sizeIn2)
            getReporter()(kErrorObject, getProxy(), "convolution processing size is larger than maximum processing size (#)", mProcessor.max_fft_size());
        
        // Get Output
        
        requestOutputSize(0, sizeOut);
        
        if (allocateOutputs())
            mProcessor.convolve(getOutput(0, &sizeOut), {input1, sizeIn1}, {input2, sizeIn2}, edgeMode);
    }
    else
    {
        // Get Inputs
        
        unsigned long sizeR1, sizeI1, sizeR2, sizeI2;
        
        const double *inR1 = getInput(0, &sizeR1);
        const double *inI1 = getInput(1, &sizeI1);
        const double *inR2 = getInput(2, &sizeR2);
        const double *inI2 = getInput(3, &sizeI2);
        
        // Get Output Size / Check for Processing Size Errors

        unsigned long sizeOut = static_cast<unsigned long>(mProcessor.convolved_size(std::max(sizeR1, sizeI1), std::max(sizeR2, sizeI2), edgeMode));
        
        if (sizeOut == 0 && std::max(sizeR1, sizeI1) && std::max(sizeR2, sizeI2))
            getReporter()(kErrorObject, getProxy(), "convolution processing size is larger than maximum processing size (#)", mProcessor.max_fft_size());
        
        // Get Output
        
        requestOutputSize(0, sizeOut);
        requestOutputSize(1, sizeOut);
        
        if (allocateOutputs())
        {
            double *rOut = getOutput(0, &sizeOut);
            double *iOut = getOutput(1, &sizeOut);
        
            mProcessor.convolve(rOut, iOut, {inR1, sizeR1}, {inI1, sizeI1}, {inR2, sizeR2}, {inI2, sizeI2}, edgeMode);
        }
    }
}
