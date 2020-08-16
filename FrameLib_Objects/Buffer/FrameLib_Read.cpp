
#include "FrameLib_Read.h"

// Constructor

FrameLib_Read::FrameLib_Read(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, &sParamInfo, 2, 1)
, mProxy(cloneProxy<Proxy>(proxy))
{
    mParameters.addString(kBuffer, "buffer", 0);
    
    mParameters.addInt(kChannel, "chan", 1, 1);
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
    mParameters.addEnumItem(kExtrapolate, "extrapolate");
    mParameters.addEnumItem(kExtend, "extend");
    mParameters.addEnumItem(kWrap, "wrap");
    mParameters.addEnumItem(kZeroPad, "zero", true);
    
    mParameters.addBool(kBound, "bound", true, 5);
    
    mParameters.set(serialisedParameters);
    
    mChan = mParameters.getInt(kChannel);
    mInterpolation = (Interpolation) mParameters.getInt(kInterpolation);
    mUnits = (Units) mParameters.getInt(kUnits);
    
    setParameterInput(1);
        
    if (mProxy)
        mProxy->update(mParameters.getString(kBuffer));
}

// Info

std::string FrameLib_Read::objectInfo(bool verbose)
{
    return formatInfo("Reads from a buffer~ given an input frame of sample positions: "
                      "There are different available interpolation types.",
                      "Reads from a buffer~ given an input frame of sample positions.", verbose);
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
    add("Sets the buffer~ name to use.");
    add("Sets the buffer~ channel to use.");
    add("Sets the interpolation mode: "
        "hermite - cubic hermite interpolation. "
        "bspline - cubic bspline interpolation. "
        "lagrange - cubic lagrange interpolation. "
        "linear - linear interpolation.");
    add("Sets the units for the position input.");
}

// Update

void FrameLib_Read::update()
{
    if (mProxy)
        mProxy->update(mParameters.getString(kBuffer));
    
    mChan = mParameters.getInt(kChannel);
    mInterpolation = (Interpolation) mParameters.getInt(kInterpolation);
    mUnits = (Units) mParameters.getInt(kUnits);
}

// Process

void FrameLib_Read::process()
{
    EdgeType edges = static_cast<EdgeType>(mParameters.getInt(kEdges));
    InterpType interpType = kInterpNone;

    double *positions = nullptr;

    unsigned long size;
    long chan = mChan - 1;
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
    
    if (size && length)
        positions = alloc<double>(size);
    
    if (positions)
    {
        double scale = 1.0;
        double lengthM1 = length - 1.0;
        
        if (!samplingRate)
            samplingRate = mSamplingRate;
        
        switch (mUnits)
        {
            case kSamples:      scale = 1.0;                                    break;
            case kMS:           scale = samplingRate / 1000.0;                  break;
            case kSeconds:      scale = samplingRate;                           break;
            case kNormalised:   scale = lengthM1 + (edges == kWrap ? 1 : 0);    break;
        }

        for (unsigned long i = 0; i < size; i++)
        {
            positions[i] = input[i] * scale;
            
            // N.B. - Assume that false is zero
            
            doInterpolation |= ((positions[i] - ((int32_t) positions[i])) != 0.0);
        }

        if (doInterpolation || edges == kExtrapolate)
        {
            switch (mInterpolation)
            {
                case kNone:         break;
                case kLinear:       interpType = kInterpLinear;             break;
                case kHermite:      interpType = kInterpCubicHermite;       break;
                case kBSpline:      interpType = kInterpCubicBSpline;       break;
                case kLagrange:     interpType = kInterpCubicLagrange;      break;
            }
        }
                
        mProxy->read(output, positions, size, chan, interpType, edges, bound);
        
        dealloc(positions);
    }
    else
    {
        // Zero output if no buffer or memory
        
        zeroVector(output, size);
    }
    
    if (mProxy)
        mProxy->release();
}
