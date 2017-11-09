
#include "fl.read~.h"
#include "FrameLib_MaxClass.h"
#include "ibuffer/ibuffer_access.h"

// FIX - abstract out max buffer interaction (buffer name / channel)
// FIX - consider adding anti-alising later....

// Underlying FrameLib Object

// Init

bool FrameLib_MaxRead::sInit = false;

// Constructor

FrameLib_MaxRead::FrameLib_MaxRead(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, owner, &sParamInfo, 2, 1)
{
    mParameters.addString(kBuffer, "buffer", 0);
    
    mParameters.addInt(kChannel, "chan", 0, 1);
    mParameters.setClip(1, 4);
    
    mParameters.addEnum(kMode, "mode");
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
    
    mBufferName = gensym(mParameters.getString(kBuffer));
    mChan = mParameters.getInt(kChannel);
    mMode = (Modes) mParameters.getInt(kMode);
    mUnits = (Units) mParameters.getInt(kUnits);
    
    setParameterInput(1);
    
    assert(false == 0 && "False does not equal zero");
    
    if (!sInit)
    {
        ibuffer_init();
        sInit = true;
    }
}

// Info

std::string FrameLib_MaxRead::objectInfo(bool verbose)
{
    return formatInfo("Reads from a buffer~ given an input frame of sample positions: There are different available interpolation types.",
                   "Reads from a buffer~ given an input frame of sample positions.", verbose);
}

std::string FrameLib_MaxRead::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return parameterInputInfo(verbose);
    else
        return formatInfo("Frame of Positions - triggers generation of output", "Frame of Positions", verbose);
}

std::string FrameLib_MaxRead::outputInfo(unsigned long idx, bool verbose)
{
    return "Output Frame";
}

// Parameter Info

FrameLib_MaxRead::ParameterInfo FrameLib_MaxRead::sParamInfo;

FrameLib_MaxRead::ParameterInfo::ParameterInfo()
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

// Process

void FrameLib_MaxRead::process()
{
    double *positions = NULL;
    
    unsigned long size;
    long chan;
    
    bool interp = false;
    
    const double *input = getInput(0, &size);
    
    requestOutputSize(0, size);
    allocateOutputs();
    
    double *output = getOutput(0, &size);
    
    // Get buffer
    
    void *buffer = ibuffer_get_ptr(mBufferName);
    
    const ibuffer_data data = ibuffer_info(buffer);
    
    if (buffer && size && data.samples)
    {
        chan = (mChan - 1) % data.num_chans;
        positions = alloc<double>(size);
    }
    
    if (positions)
    {
        double lengthM1 = data.length - 1.0;
        double conversionFactor = 1.0;
        double samplingRate = ibuffer_sample_rate(buffer);
        
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
            switch (mMode)
            {
                case kNone:         break;
                case kLinear:       interpType = kInterpLinear;             break;
                case kLagrange:     interpType = kInterpCubicLagrange;      break;
                case kHermite:      interpType = kInterpCubicHermite;       break;
                case kBSpline:      interpType = kInterpCubicBSpline;       break;
            }
        }
        
        ibuffer_read(data, output, positions, size, chan, 1.0, interpType);
        dealloc(positions);
    }
    else
    {
        // Zero output if no buffer or memory
        
        zeroVector(output, size);
    }
    
    ibuffer_release_ptr(buffer);

}

// Max Object

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass_Expand<FrameLib_MaxRead>::makeClass(CLASS_BOX, "fl.read~");
}
