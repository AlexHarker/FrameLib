
#include "ext.h"
#include "ibuffer/ibuffer_access.h"
#include "FrameLib_DSP.h"

// FIX - abstract out max buffer interaction (buffer name / channel)
// FIX - consider adding anti-alising later....

class FrameLib_MaxRead : public FrameLib_Processor
{
    enum ParameterList {kBuffer, kChannel, kMode, kUnits};
    enum Modes {kHermite, kBSpline, kLagrange, kLinear};
    enum Units {kMS, kSeconds, kSamples};

    static bool sInit;
    
public:
    
    FrameLib_MaxRead (FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, 1, 1)
    {
        mParameters.addString(kBuffer, "buffer", "", 0);
        
        mParameters.addDouble(kChannel, "chan", 0.0, 1);
        mParameters.setClip(1.0, 4.0);
        
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
        
        assert(FALSE == 0 && "False does not equal zero");
        
        if (!sInit)
        {
            ibuffer_init();
            sInit = TRUE;
        }
    }
    
protected:
    
    void process()
    {
        void *temp[4];
        void *tempMem = NULL;
        void *bufferSamples;
        double *fracts;
        AH_SIntPtr *offsets;
        
        AH_SIntPtr length;
        unsigned long size, vecSize, paddedSize;
        long chan, nChans, format;
        size_t tempMemSize;
        
        bool interp = FALSE;
        
        double *input = getInput(0, &size);
        
        requestOutputSize(0, size);
        allocateOutputs();
        vecSize = size - (size & 0x3);
        
        double *output = getOutput(0, &size);
        
        // Get buffer
        
        void *buffer = ibuffer_get_ptr(mBufferName);
        
        if (buffer && size)
        {
            ibuffer_increment_inuse(buffer);
            
            if (ibuffer_info(buffer, &bufferSamples, &length, &nChans, &format))
            {
                chan = (mChan - 1) % nChans;
            
                paddedSize = (size + 0x3) & ~0x3;
            
                tempMemSize = (mMode == kLinear) ? paddedSize * (2 * sizeof(float) + sizeof(double) + sizeof(AH_SIntPtr)) : paddedSize * (4 * sizeof(float) + sizeof(double) + sizeof(AH_SIntPtr));
                tempMem = mAllocator->alloc(tempMemSize);
            }
            
            if (!tempMem)
                ibuffer_decrement_inuse(buffer);
        }
        
        if (tempMem)
        {
            temp[0] = ((float *) tempMem);
            temp[1] = ((float *) temp[0]) + paddedSize;
            temp[2] = ((float *) temp[1]) + paddedSize;
            temp[3] = ((float *) temp[2]) + paddedSize;
            
            offsets = (AH_SIntPtr *) ((mMode == kLinear) ? (((float *) temp[1]) + paddedSize) : (((float *) temp[3]) + paddedSize));
            fracts = (double *) (offsets + paddedSize);

            double lengthM1 = length - 1.0;
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
            
            for (unsigned int i = 0; i < size; i++)
            {
                double position = input[i] * conversionFactor;
                
                // FIX - use SSE explictly here?
                
                position = position > lengthM1 ? lengthM1 : position;
                position = position < 0.0 ? 0.0 : position;
                
                AH_SIntPtr offset = position;
                double fract = position - offset;
                
                offsets[i] = offset;
                fracts[i] = fract;

                // N.B. - Assume that FALSE is zero
                
                interp |= (fract != 0.0);
            }
            
            ibuffer_preprocess_offsets (offsets, vecSize, nChans, format);
            
            if (interp)
            {
                switch (mMode)
                {
                    case kLinear:
                        ibuffer_double_samps_simd_linear(bufferSamples, (vDouble *) output, offsets, (vDouble *) fracts, temp, vecSize, nChans, chan, format, 1.0);
                        ibuffer_double_samps_scalar_linear(bufferSamples, output + vecSize, offsets + vecSize, fracts + vecSize, size & 0x3, nChans, chan, format, 1.0);
                        break;
                    case kBSpline:
                        ibuffer_double_samps_simd_cubic_bspline(bufferSamples, (vDouble *) output, offsets, (vDouble *) fracts, temp, vecSize, nChans, chan, format, 1.0);
                        ibuffer_double_samps_scalar_cubic_bspline(bufferSamples, output + vecSize, offsets + vecSize, fracts + vecSize, size & 0x3, nChans, chan, format, 1.0);
                        break;
                    case kHermite:
                        ibuffer_double_samps_simd_cubic_hermite(bufferSamples, (vDouble *) output, offsets, (vDouble *) fracts, temp, vecSize, nChans, chan, format, 1.0);
                        ibuffer_double_samps_scalar_cubic_hermite(bufferSamples, output + vecSize, offsets + vecSize, fracts + vecSize, size & 0x3, nChans, chan, format, 1.0);
                        break;
                    case kLagrange:
                        ibuffer_double_samps_simd_cubic_lagrange(bufferSamples, (vDouble *) output, offsets, (vDouble *) fracts, temp, vecSize, nChans, chan, format, 1.0);
                        ibuffer_double_samps_scalar_cubic_lagrange(bufferSamples, output + vecSize, offsets + vecSize, fracts + vecSize, size & 0x3, nChans, chan, format, 1.0);
                        break;
                }
            }
            else
            {
                ibuffer_double_samps_simd_nointerp(bufferSamples, (vDouble *) output, offsets, vecSize, nChans, chan, format, 1.0);
                ibuffer_double_samps_scalar_nointerp(bufferSamples, output + vecSize, offsets + vecSize, size & 0x3, nChans, chan, format, 1.0);
            }
            
            mAllocator->dealloc(tempMem);
            ibuffer_decrement_inuse(buffer);
        }
        else
        {
            // Zero output if no buffer or memory
            
            for (unsigned int i = 0; i < size; i++)
                output[i] = 0.0;
        }
    }
    
private:
    
    t_symbol *mBufferName;
    long mChan;
    Modes mMode;
    Units mUnits;
};

bool FrameLib_MaxRead::sInit = FALSE;

#define OBJECT_CLASS FrameLib_Expand<FrameLib_MaxRead>
#define OBJECT_NAME "fl.read~"

#include "Framelib_Max.h"
