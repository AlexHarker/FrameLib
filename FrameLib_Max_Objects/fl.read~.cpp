
#include "ext.h"
#include "ibuffer/ibuffer_access.h"
#include "FrameLib_DSP.h"

// FIX - abstract out max buffer interaction (buffer name / channel)
// FIX - consider adding anti-alising later....

class FrameLib_MaxRead : public FrameLib_Processor
{
    enum AttributeList {kBuffer, kChannel, kMode, kUnits};
    enum Modes {kHermite, kBSpline, kLagrange, kLinear};
    enum Units {kMS, kSeconds, kSamples};

    static bool sInit;
    
public:
    
    FrameLib_MaxRead (DSPQueue *queue, FrameLib_Attributes::Serial *serialisedAttributes) : FrameLib_Processor(queue, 1, 1)
    {
        mAttributes.addString(kBuffer, "buffer", "", 0);
        
        mAttributes.addDouble(kChannel, "chan", 0.0, 1);
        mAttributes.setClip(1.0, 4.0);
        
        mAttributes.addEnum(kMode, "mode");
        mAttributes.addEnumItem(kHermite, "hermite");
        mAttributes.addEnumItem(kBSpline, "bspline");
        mAttributes.addEnumItem(kLagrange, "lagrange");
        mAttributes.addEnumItem(kLinear, "linear");
        
        mAttributes.addEnum(kUnits, "units");
        mAttributes.addEnumItem(kMS, "ms");
        mAttributes.addEnumItem(kSeconds, "seconds");
        mAttributes.addEnumItem(kSamples, "samples");
        
        mAttributes.set(serialisedAttributes);

        mBufferName = gensym(mAttributes.getString(kBuffer));
        mChan = mAttributes.getInt(kChannel);
        mMode = (Modes) mAttributes.getInt(kMode);
        mUnits = (Units) mAttributes.getInt(kUnits);
        
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
            ibuffer_info(buffer, &bufferSamples, &length, &nChans, &format);
            chan = (mChan - 1) % nChans;
            
            paddedSize = (size + 0x3) & ~0x3;
            
            tempMemSize = (mMode == kLinear) ? paddedSize * (2 * sizeof(float) + sizeof(double) + sizeof(AH_SIntPtr)) : paddedSize * (4 * sizeof(float) + sizeof(double) + sizeof(AH_SIntPtr));
            tempMem = mAllocator->alloc(tempMemSize);
            
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
            // Zero output if no buffer
            
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
