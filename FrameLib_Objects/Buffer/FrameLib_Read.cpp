
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
    mParameters.addEnumItem(kZeroPad, "zero");
    mParameters.addEnumItem(kExtend, "extend");
    mParameters.addEnumItem(kWrap, "wrap");
    mParameters.addEnumItem(kFold, "fold");
    mParameters.addEnumItem(kMirror, "mirror");
    mParameters.addEnumItem(kExtrapolate, "extrapolate");

    mParameters.addBool(kBound, "bound", true, 5);
    
    mParameters.set(serialisedParameters);
        
    setParameterInput(1);
        
    if (mProxy)
        mProxy->update(mParameters.getString(kBuffer));
}

// Info

std::string FrameLib_Read::objectInfo(bool verbose)
{
    return formatInfo("Reads from a buffer given an input frame of sample positions: "
                      "There are different available interpolation types.",
                      "Reads from a buffer given an input frame of sample positions.", verbose);
}

std::string FrameLib_Read::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return parameterInputInfo(verbose);
    else
        return formatInfo("Frame of Positions - triggers generation of output", "Frame of Positions", verbose);
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
    add("Sets the interpolation mode: "
        "none - no interpolation"
        "linear - linear interpolation. "
        "hermite - cubic hermite interpolation. "
        "bspline - cubic bspline interpolation. "
        "lagrange - cubic lagrange interpolation.");
    add("Sets the units for the position input.");
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
    EdgeType edges = mParameters.getEnum<EdgeType>(kEdges);
    Interpolation interpolation = mParameters.getEnum<Interpolation>(kInterpolation);
    InterpType interpType = kInterpNone;

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
        bool adjustScaling = edges == kWrap || edges == kMirror;
        
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

        if (doInterpolation || interpolation == kBSpline || edges == kExtrapolate)
        {
            switch (interpolation)
            {
                case kNone:         break;
                case kLinear:       interpType = kInterpLinear;             break;
                case kHermite:      interpType = kInterpCubicHermite;       break;
                case kBSpline:      interpType = kInterpCubicBSpline;       break;
                case kLagrange:     interpType = kInterpCubicLagrange;      break;
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
