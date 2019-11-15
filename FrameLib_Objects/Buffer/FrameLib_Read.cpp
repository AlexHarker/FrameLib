
#include "FrameLib_Read.h"

// FIX - consider adding anti-alising later....

// Constructor

FrameLib_Read::FrameLib_Read(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_Processor(context, proxy, &sParamInfo, 2, 1), mProxy(dynamic_cast<Proxy *>(proxy)->clone())
{
    mParameters.addString(kBuffer, "buffer", 0);
    
    mParameters.addInt(kChannel, "chan", 1, 1);
    mParameters.setMin(1);
    
    mParameters.addEnum(kInterpolation, "interp");
    mParameters.addEnumItem(kHermite, "hermite");
    mParameters.addEnumItem(kBSpline, "bspline");
    mParameters.addEnumItem(kLagrange, "lagrange");
    mParameters.addEnumItem(kLinear, "linear");
    mParameters.addEnumItem(kNone, "none");
    
    mParameters.addEnum(kUnits, "units");
    mParameters.addEnumItem(kMS, "ms");
    mParameters.addEnumItem(kSeconds, "seconds");
    mParameters.addEnumItem(kSamples, "samples");
        
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
    double *positions = nullptr;
    
    unsigned long size;
    long chan = mChan - 1;
    
    bool interp = false;
    
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
        double conversionFactor = 1.0;
        double lengthM1 = length - 1.0;
        
        if (!samplingRate)
            samplingRate = mSamplingRate;
        
        switch (mUnits)
        {
            case kMS:           conversionFactor = samplingRate / 1000.0;       break;
            case kSeconds:      conversionFactor = samplingRate;                break;
            case kSamples:      conversionFactor = 1.0;                         break;
        }
        
        for (unsigned long i = 0; i < size; i++)
        {
            double position = input[i] * conversionFactor;
            
            // FIX - use SSE explictly here?
            
            position = position > lengthM1 ? lengthM1 : position;
            position = position < 0.0 ? 0.0 : position;
    
            positions[i] = position;
            
            // N.B. - Assume that false is zero
            
            interp |= ((position - ((int32_t) position)) != 0.0);
        }
                
        InterpType interpType = kInterpNone;
        
        if (interp)
        {
            switch (mInterpolation)
            {
                case kNone:         break;
                case kLinear:       interpType = kInterpLinear;             break;
                case kLagrange:     interpType = kInterpCubicLagrange;      break;
                case kHermite:      interpType = kInterpCubicHermite;       break;
                case kBSpline:      interpType = kInterpCubicBSpline;       break;
            }
        }
        
        mProxy->read(output, positions, size, chan, interpType);
        
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
