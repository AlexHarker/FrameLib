
#include "FrameLib_Read.h"

// Constructor

FrameLib_Read::FrameLib_Read(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, &sParamInfo, 2, 1)
, mProxy(cloneProxy<Proxy>(proxy))
{
    mParameters.addString(kBuffer, "buffer", 0);
    
    mParameters.addInt(kChannel, "channel", 1, 1);
    mParameters.setMin(1);
    
    mParameters.addEnum(kUnits, "units", 2);
    mParameters.addEnumItem(kSamples, "samples");
    mParameters.addEnumItem(kMS, "ms", true);
    mParameters.addEnumItem(kSeconds, "seconds");
    mParameters.addEnumItem(kNormalised, "normalised");
    
    mParameters.addEnum(kInterpolation, "interp", 3);
    mParameters.addEnumItem(kNone, "none");
    mParameters.addEnumItem(kLinear, "linear");
    mParameters.addEnumItem(kHermite, "hermite", true);
    mParameters.addEnumItem(kBSpline, "bspline");
    mParameters.addEnumItem(kLagrange, "lagrange");
    
    mParameters.addEnum(kEdges, "edges", 4);
    mParameters.addEnumItem(static_cast<unsigned long>(EdgeMode::ZeroPad), "zero");
    mParameters.addEnumItem(static_cast<unsigned long>(EdgeMode::Extend), "extend");
    mParameters.addEnumItem(static_cast<unsigned long>(EdgeMode::Wrap), "wrap");
    mParameters.addEnumItem(static_cast<unsigned long>(EdgeMode::Fold), "fold");
    mParameters.addEnumItem(static_cast<unsigned long>(EdgeMode::Mirror), "mirror");
    mParameters.addEnumItem(static_cast<unsigned long>(EdgeMode::Extrapolate), "extrapolate");

    mParameters.addBool(kBound, "bound", true, 5);
    
    mParameters.set(serialisedParameters);
        
    setParameterInput(1);
        
    if (mProxy)
        mProxy->update(mParameters.getString(kBuffer));
}

// Info

std::string FrameLib_Read::objectInfo(bool verbose)
{
    return formatInfo("Reads from a specified host buffer given an input frame of positions: "
                      "The output is the same length as the input. "
                      "The scale, edges and bound parameters control the mapping of the input to the buffer. "
                      "For hosts supporting multichannel buffers the channel read can also be selected. "
                      "The reading can be performed with different types of interpolation (or none).",
                      "Reads from a specified host buffer given an input frame of positions.", verbose);
}

std::string FrameLib_Read::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return parameterInputInfo(verbose);
    else
        return formatInfo("Input - positions to read from the buffer", "Input", verbose);
}

std::string FrameLib_Read::outputInfo(unsigned long idx, bool verbose)
{
    return "Output";
}

// Parameter Info

FrameLib_Read::ParameterInfo FrameLib_Read::sParamInfo;

FrameLib_Read::ParameterInfo::ParameterInfo()
{
    add("Sets the buffer to use.");
    add("Sets the buffer channel to use.");
    add("Sets the units used for interpreting the input. "
        "Note that the edge parameter is also accounted for normalised mode. "
        "This adjusts the scaling to work sensibly with cyclical modes.");
    add("Sets the interpolation mode: "
        "none - no interpolation. "
        "linear - linear interpolation. "
        "hermite - cubic hermite interpolation. "
        "bspline - cubic bspline interpolation. "
        "lagrange - cubic lagrange interpolation.");
    add("Sets the edge behaviour for both interpolation and reading: "
        "zero - values beyond the edges of the buffer are read as zeros. "
        "extend - the edge values are extended infinitely in either direction. "
        "wrap - values are read as wrapped or cyclical. "
        "fold - values are folded at edges without repetition of the edge values. "
        "mirror - values are mirrored at edges with the edge values repeated. "
        "extrapolate - values out of range are extrapolated via interpolation.");
    add("Sets whether reading is bounded to the edges of the buffer, or can extend beyond it.");
}

// Update

void FrameLib_Read::update()
{
    if (mProxy)
        mProxy->update(mParameters.getString(kBuffer));
}

// Process

void FrameLib_Read::process()
{
    EdgeMode edges = mParameters.getEnum<EdgeMode>(kEdges);
    Interpolation interpolation = mParameters.getEnum<Interpolation>(kInterpolation);
    InterpType interpType = InterpType::None;

    unsigned long size;
    long chan = mParameters.getInt(kChannel) - 1;
    bool bound = mParameters.getBool(kBound);
    bool doInterpolation = false;
    
    const double *input = getInput(0, &size);
    
    requestOutputSize(0, size);
    allocateOutputs();
    
    double *output = getOutput(0, &size);
    
    double samplingRate = 0.0;
    unsigned long length = 0;
    
    // Get buffer
    
    if (mProxy)
         mProxy->acquire(length, samplingRate);
    
    auto positions = allocAutoArray<double>(size && length ? size : 0);
    
    if (positions)
    {
        bool adjustScaling = edges == EdgeMode::Wrap || edges == EdgeMode::Mirror;
        
        double scale = 1.0;
        double lengthM1 = length - 1.0;
        
        if (!samplingRate)
            samplingRate = mSamplingRate;
        
        switch (mParameters.getEnum<Units>(kUnits))
        {
            case kSamples:      scale = 1.0;                                    break;
            case kMS:           scale = samplingRate / 1000.0;                  break;
            case kSeconds:      scale = samplingRate;                           break;
            case kNormalised:   scale = lengthM1 + (adjustScaling ? 1 : 0);     break;
        }

        for (unsigned long i = 0; i < size; i++)
        {
            positions[i] = input[i] * scale;
            
            // N.B. - Assume that false is zero
            
            doInterpolation |= ((positions[i] - ((int32_t) positions[i])) != 0.0);
        }

        if (doInterpolation || interpolation == kBSpline || edges == EdgeMode::Extrapolate)
        {
            switch (interpolation)
            {
                case kNone:         break;
                case kLinear:       interpType = InterpType::Linear;            break;
                case kHermite:      interpType = InterpType::CubicHermite;      break;
                case kBSpline:      interpType = InterpType::CubicBSpline;      break;
                case kLagrange:     interpType = InterpType::CubicLagrange;     break;
            }
        }
                
        mProxy->read(output, positions, size, chan, interpType, edges, bound);
    }
    else
    {
        // Zero output if no buffer or memory
        
        zeroVector(output, size);
    }
    
    if (mProxy)
        mProxy->release();
}
