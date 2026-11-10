
#ifndef MAXBUFFER_H
#define MAXBUFFER_H

#include <ext_buffer.h>

// Buffer Access via RAII

class MaxBufferAccess
{
public:
    
    MaxBufferAccess(t_object *owner, t_symbol *name)
    : mOwner(owner)
    , mBuffer(buffer_ref_new(owner, name))
    , mSamples(nullptr)
    , mLength(0)
    , mChans(0)
    , mSampleRate(0.0)
    {
        acquireInternal();
    }
    
    ~MaxBufferAccess()
    {
        release();
    }
    
    MaxBufferAccess(const MaxBufferAccess &) = delete;
    MaxBufferAccess &operator=(const MaxBufferAccess &other) = delete;
    
    void acquire(t_symbol *name)
    {
        // N.B. - release before acquiring the new buffer
        
        release();
        mBuffer = buffer_ref_new(mOwner, name);
        acquireInternal();
    }
    
    void release()
    {
        if (getBuffer())
            buffer_unlocksamples(getBuffer());
        
        object_free(mBuffer);
        mSamples = nullptr;
        mLength = 0;
        mChans = 0;
        mSampleRate = 0.0;
        mBuffer = nullptr;
    }
    
    void setDirty()
    {
        if (getBuffer())
            object_method(getBuffer(), gensym("dirty"));
    }

    float *samples() const          { return mSamples; };
    t_ptr_int length() const        { return mLength; }
    long chans() const              { return mChans; }
    double sampleRate() const       { return mSampleRate; }
    
    void read(double *output, t_ptr_uint length, t_ptr_int offset, t_ptr_uint chan)
    {
        t_ptr_uint readLength = constrainLength(length, chan, offset);
        t_ptr_uint offsetPad = std::min(handleNegativeOffset(offset), length);

        // Zero remaining samples if needed
                
        std::fill_n(output, offsetPad, 0.0);
        
        float *samples = mSamples + offset * mChans + chan;
        
        for (t_ptr_uint i = 0; i < readLength; i++, samples += mChans)
            output[i + offsetPad] = *samples;
            
        // Zero remaining samples if needed
            
        std::fill_n(output + offsetPad + readLength, length - (offsetPad + readLength), 0.0);
    }
    
    void write(const double *input, t_ptr_uint length, t_ptr_int offset, t_ptr_uint chan)
    {
        length = constrainLength(length, chan, offset);
        t_ptr_uint inputOffset = handleNegativeOffset(offset);
        
        float *samples = mSamples + offset * mChans + chan;
        
        for (t_ptr_uint i = 0; i < length; i++, samples += mChans)
            *samples = static_cast<float>(input[i + inputOffset]);
        
        setDirty();
    }
    
    void display()
    {
        if (getBuffer())
            buffer_view(getBuffer());
    }
    
private:
    
    t_object *getBuffer() const     { return buffer_ref_getobject(mBuffer); }

    void acquireInternal()
    {
        t_buffer_info info;
        
        mSamples = buffer_locksamples(getBuffer());
        buffer_getinfo(getBuffer(), &info);
        mLength = mSamples ? info.b_frames : 0;
        mChans = mSamples ? info.b_nchans : 0;
        mSampleRate = mSamples ? info.b_sr : 0.0;
    }
    
    t_ptr_uint constrainLength(t_ptr_uint length, t_ptr_uint chan, t_ptr_int offset)
    {
        auto prior = handleNegativeOffset(offset);
        
        // Request is entirely outside of the buffer's memory
        
        if (chan >= static_cast<t_ptr_uint>(mChans) || offset >= mLength || length < prior)
            return 0;
        
        // Deal with negative offsets
        
        length -= prior;
        
        // Request is partially after the buffer's memory
        
        if ((offset + length) > static_cast<t_ptr_uint>(mLength))
            return mLength - offset;
        
        return length;
    }
    
    t_ptr_uint handleNegativeOffset(t_ptr_int& offset)
    {
        if (offset < 0)
        {
            t_ptr_uint prior = static_cast<t_ptr_uint>(-offset);
            offset = 0;
            return prior;
        }
        
        return 0;
    }
    
    t_object *mOwner;
    t_buffer_ref *mBuffer;
    
    float *mSamples;
    
    t_ptr_int mLength;
    long mChans;
    double mSampleRate;
};

#endif
