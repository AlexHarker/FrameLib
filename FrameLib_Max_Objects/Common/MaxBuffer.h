
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
    
    void setDirty()                 { object_method(getBuffer(), gensym("dirty")); }

    float *samples() const          { return mSamples; };
    t_ptr_int length() const        { return mLength; }
    long chans() const              { return mChans; }
    double sampleRate() const       { return mSampleRate; }
    
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
    
    t_object *mOwner;
    t_buffer_ref *mBuffer;
    
    float *mSamples;
    
    t_ptr_int mLength;
    long mChans;
    double mSampleRate;
};

#endif
