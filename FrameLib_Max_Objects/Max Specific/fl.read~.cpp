
#include "fl.read~.h"
#include "FrameLib_MaxClass.h"
#include "ibuffer/ibuffer_access.h"

// FIX - abstract out max buffer interaction (buffer name / channel)
// FIX - consider adding anti-alising later....

// Underlying FrameLib Object

// Init

bool FrameLib_MaxRead::sInit = false;

// Constructor

FrameLib_MaxRead::FrameLib_MaxRead(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, &sParamInfo, 1, 1)
{
    mParameters.addString(kBuffer, "buffer", 0);
    
    mParameters.addInt(kChannel, "chan", 0, 1);
    mParameters.setClip(1, 4);
    
    mParameters.addEnum(kMode, "mode");
    mParameters.addEnumItem(kHermite, "hermite");
    mParameters.addEnumItem(kBSpline, "bspline");
    mParameters.addEnumItem(kLagrange, "lagrange");
    mParameters.addEnumItem(kLinear, "linear");
    
    mParameters.addEnum(kUnits, "units");
    mParameters.addEnumItem(kMS, "ms");
    mParameters.addEnumItem(kSeconds, "seconds");
    mParameters.addEnumItem(kSamples, "samples");
        
    mParameters.set(serialisedParameters);
    
    mBufferName = gensym(mParameters.getString(kBuffer));
    mChan = mParameters.getInt(kChannel);
    mMode = (Modes) mParameters.getInt(kMode);
    mUnits = (Units) mParameters.getInt(kUnits);
    
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
    return getInfo("Reads from a buffer~ given an input frame of sample positions: There are different available interpolation types.",
                   "Reads from a buffer~ given an input frame of sample positions.", verbose);
}

std::string FrameLib_MaxRead::inputInfo(unsigned long idx, bool verbose)
{
    return getInfo("Frame of Positions - triggers generation of output", "Frame of Positions", verbose);
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
    void *tempMem = NULL;
    double *fracts;
    intptr_t *offsets;
    
    unsigned long size, vecSize;
    long chan;
    
    bool interp = false;
    
    double *input = getInput(0, &size);
    
    requestOutputSize(0, size);
    allocateOutputs();
    vecSize = size - (size & 0x3);
    
    double *output = getOutput(0, &size);
    
    // Get buffer
    
    void *buffer = ibuffer_get_ptr(mBufferName);
    
    if (buffer && size)
        ibuffer_increment_inuse(buffer);
        
    const ibuffer_data data = ibuffer_info(buffer);
    
    if (buffer && size)
    {
        chan = (mChan - 1) % data.n_chans;
            
        tempMem = mAllocator->alloc(size * (sizeof(double) + sizeof(intptr_t)));
        if (!tempMem)
            ibuffer_decrement_inuse(buffer);
    }
    
    if (tempMem)
    {
        offsets = (intptr_t *) tempMem;
        fracts = (double *) (offsets + size);
        
        double lengthM1 = data.length - 1.0;
        double conversionFactor = 1.0;
        double samplingRate = ibuffer_sample_rate(buffer);
        
        switch (mUnits)
        {
            case kMS:
                conversionFactor = samplingRate / 1000.0;
                break;
                
            case kSeconds:
                conversionFactor = samplingRate;
                break;
                
            case kSamples:
                conversionFactor = 1.0;
                break;
        }
        
        for (unsigned long i = 0; i < size; i++)
        {
            double position = input[i] * conversionFactor;
            
            // FIX - use SSE explictly here?
            
            position = position > lengthM1 ? lengthM1 : position;
            position = position < 0.0 ? 0.0 : position;
            
            intptr_t offset = position;
            double fract = position - offset;
            
            offsets[i] = offset;
            fracts[i] = fract;
            
            // N.B. - Assume that false is zero
            
            interp |= (fract != 0.0);
        }
        
        ibuffer_preprocess_offsets(offsets, data, vecSize);
        
        if (interp)
        {
            switch (mMode)
            {
                case kLinear:
                    ibuffer_double_samps_simd_linear(data, (SSE4Double *) output, offsets, (SSE4Double *) fracts, vecSize, chan, 1.0);
                    ibuffer_double_samps_scalar_linear(data, output + vecSize, offsets + vecSize, fracts + vecSize, size & 0x3, chan, 1.0);
                    break;
                case kBSpline:
                    ibuffer_double_samps_simd_cubic_bspline(data, (AVX256Double *) output, offsets, (AVX256Double *) fracts, vecSize, chan, 1.0);
                    ibuffer_double_samps_scalar_cubic_bspline(data, output + vecSize, offsets + vecSize, fracts + vecSize, size & 0x3, chan, 1.0);
                    break;
                case kHermite:
                    ibuffer_double_samps_simd_cubic_hermite(data, (AVX256Double *) output, offsets, (AVX256Double *) fracts, vecSize, chan, 1.0);
                    ibuffer_double_samps_scalar_cubic_hermite(data, output + vecSize, offsets + vecSize, fracts + vecSize, size & 0x3, chan, 1.0);
                    break;
                case kLagrange:
                    ibuffer_double_samps_simd_cubic_lagrange(data, (AVX256Double *) output, offsets, (AVX256Double *) fracts, vecSize, chan, 1.0);
                    ibuffer_double_samps_scalar_cubic_lagrange(data, output + vecSize, offsets + vecSize, fracts + vecSize, size & 0x3, chan, 1.0);
                    break;
            }
        }
        else
        {
            ibuffer_double_samps_simd_nointerp(data, (vDouble *) output, offsets, vecSize, chan, 1.0);
            ibuffer_double_samps_scalar_nointerp(data, output + vecSize, offsets + vecSize, size & 0x3, chan, 1.0);
        }
        
        mAllocator->dealloc(tempMem);
        ibuffer_decrement_inuse(buffer);
    }
    else
    {
        // Zero output if no buffer or memory
        
        zeroVector(output, size);
    }
}

// Max Object

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass_Expand<FrameLib_MaxRead>::makeClass(CLASS_BOX, "fl.read~");
}
