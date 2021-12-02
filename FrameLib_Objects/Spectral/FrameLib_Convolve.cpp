
#include "FrameLib_Convolve.h"

FrameLib_Convolve::FrameLib_Convolve(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, &sParamInfo, 2, 1)
, mProcessor(*this, 0)
{
    mParameters.addInt(kMaxLength, "max_length", 16384, 0);
    mParameters.setMin(0);
    mParameters.setInstantiation();
    
    mParameters.addEnum(kMode, "mode", 1);
    mParameters.addEnumItem(kReal, "real");
    mParameters.addEnumItem(kComplex, "complex");
    mParameters.setInstantiation();
    
    mParameters.addEnum(kEdges, "edges", 2);
    mParameters.addEnumItem(Edges::kEdgeLinear, "linear");
    mParameters.addEnumItem(Edges::kEdgeWrap, "circular");
    mParameters.addEnumItem(Edges::kEdgeWrapCentre, "wrap");
    mParameters.addEnumItem(Edges::kEdgeFold, "fold");
    mParameters.addEnumItem(Edges::kEdgeFoldRepeat, "mirror");
    mParameters.setInstantiation();

    mParameters.set(serialisedParameters);
        
    mProcessor.set_max_fft_size(mParameters.getInt(kMaxLength));
    
    mMode = mParameters.getEnum<Modes>(kMode);
    
    if (mMode  == kComplex)
        setIO(4, 2);
}

// Info

std::string FrameLib_Convolve::objectInfo(bool verbose)
{
    return formatInfo("Convolves two inputs (either real or complex): "
                      "The output length depends on the edges parameter, which sets the edge behaviour. "
                      "Output length is M + N - 1 for linear convolution (M and N are the input lengths). "
                      "For other edge behaviours the output length is the maximum of M and N. "
                      "In complex mode pairs of inputs and outputs are used to represent complex values. "
                      "If one of a pair of inputs is shorter than the other it is zero padded to match. "
                      "Internal processing is FFT-based.",
                      "Convolves two inputs (either real or complex).", verbose);
}

std::string FrameLib_Convolve::inputInfo(unsigned long idx, bool verbose)
{
    if (mMode == kReal)
        return formatInfo("Input #", idx);
    else
    {
        unsigned long inIdx = idx / 2;
        
        if (idx % 2)
            return formatInfo("Imaginary Input #", inIdx);
        else
            return formatInfo("Real Input #", inIdx);
    }
}

std::string FrameLib_Convolve::outputInfo(unsigned long idx, bool verbose)
{
    if (mMode == kReal)
        return "Output";
    
    if (idx)
        return "Imaginary Output";
    else
        
        return "Real Output";
}

// Parameter Info

FrameLib_Convolve::ParameterInfo FrameLib_Convolve::sParamInfo;

FrameLib_Convolve::ParameterInfo::ParameterInfo()
{
    add("Sets the maximum processing length. "
        "The processing length is M + N - 1 where M and N are the lengths of the two inputs.");
    add("Sets the type of input and output.");
    add("Sets the edge behaviour of the convolution process: "
        "The basic operation is a linear one with output of the processing length. "
        "For modes other than linear the output length is reduced to the maximum of M and N. "
        "Excess from the linear operation is added back into the available length as specified. "
        "linear - the output is the full processing length with no further processing. "
        "circular - excess at the end is added back to the beginning of the output. "
        "wrap - as circular mode but rotated such that wrapping occurs equally at both ends. "
        "fold - as wrap but folding not wrapping at the edges (edge values are not repeated). "
        "mirror - as fold but with repeated edge values.");
}

// Process

void FrameLib_Convolve::process()
{
    Edges edges = mParameters.getEnum<Edges>(kEdges);
    
    if (mMode == kReal)
    {
        // Get Inputs
        
        unsigned long sizeIn1, sizeIn2;
        const double *input1 = getInput(0, &sizeIn1);
        const double *input2 = getInput(1, &sizeIn2);
        
        // Get Output Size / Check for Processing Size Errors
        
        unsigned long sizeOut = static_cast<unsigned long>(mProcessor.convolved_size(sizeIn1, sizeIn2, edges));
        
        if (sizeOut == 0 && sizeIn1 && sizeIn2)
            getReporter()(ErrorSource::Object, getProxy(), "convolution processing size is larger than maximum processing size (#)", mProcessor.max_fft_size());
        
        // Get Output
        
        requestOutputSize(0, sizeOut);
        
        if (allocateOutputs())
            mProcessor.convolve(getOutput(0, &sizeOut), {input1, sizeIn1}, {input2, sizeIn2}, edges);
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

        unsigned long sizeOut = static_cast<unsigned long>(mProcessor.convolved_size(std::max(sizeR1, sizeI1), std::max(sizeR2, sizeI2), edges));
        
        if (sizeOut == 0 && std::max(sizeR1, sizeI1) && std::max(sizeR2, sizeI2))
            getReporter()(ErrorSource::Object, getProxy(), "convolution processing size is larger than maximum processing size (#)", mProcessor.max_fft_size());
        
        // Get Output
        
        requestOutputSize(0, sizeOut);
        requestOutputSize(1, sizeOut);
        
        if (allocateOutputs())
        {
            double *rOut = getOutput(0, &sizeOut);
            double *iOut = getOutput(1, &sizeOut);
        
            mProcessor.convolve(rOut, iOut, {inR1, sizeR1}, {inI1, sizeI1}, {inR2, sizeR2}, {inI2, sizeI2}, edges);
        }
    }
}
