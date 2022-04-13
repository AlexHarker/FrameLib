
#include "FrameLib_MaxClass.h"
#include "FrameLib_Info.h"
#include "ibuffer/ibuffer_access.hpp"

// Max Class

class FrameLib_MaxClass_Info : public FrameLib_MaxClass_Expand<FrameLib_Info>
{
    struct InfoProxy : public FrameLib_Info::Proxy, public FrameLib_MaxProxy
    {
        InfoProxy(t_object *x)
        : FrameLib_MaxProxy(x)
        , mBuffer(nullptr)
        {}
        
        void update(const char *name) override
        {
            mBufferName = gensym(name);
        }
        
        void acquire(unsigned long& length, double& samplingRate, unsigned long& chans) override
        {
            mBuffer.acquire(mBufferName);
            length = static_cast<unsigned long>(mBuffer.get_length());
            samplingRate = mBuffer.get_sample_rate();
            chans = static_cast<unsigned long>(mBuffer.get_num_chans());
        }
        
        void release() override
        {
            mBuffer.release();
        };
        
        FrameLib_Info::Proxy *clone() const override
        {
            InfoProxy *proxy = new InfoProxy(getOwner<t_object>());
            proxy->mBufferName = mBufferName;
            
            return proxy;
        }
        
    private:
        
        ibuffer_data mBuffer;
        t_symbol *mBufferName;
    };
    
public:
     
    // Constructor
    
    FrameLib_MaxClass_Info(t_object *x, t_symbol *s, long argc, t_atom *argv)
    : FrameLib_MaxClass(x, s, argc, argv, new InfoProxy(x)) {}
};

// Max Object

#ifndef FRAMELIB_MAX_SINGLE_OBJECT

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass_Info::makeClass<FrameLib_MaxClass_Info>("fl.info~");
}

#endif
