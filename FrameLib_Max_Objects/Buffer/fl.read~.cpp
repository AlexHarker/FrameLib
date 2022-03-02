
#include "FrameLib_MaxClass.h"
#include "FrameLib_Read.h"
#include "ibuffer/ibuffer_access.hpp"

// Max Class

class FrameLib_MaxClass_Read : public FrameLib_MaxClass_Expand<FrameLib_Read>
{
    struct ReadProxy : public FrameLib_Read::Proxy, public FrameLib_MaxProxy
    {
        ReadProxy(t_object *x)
        : FrameLib_MaxProxy(x)
        , mBuffer(nullptr)
        {}
        
        void update(const char *name) override
        {
            mBufferName = gensym(name);
        }
        
        void acquire(unsigned long& length, double& samplingRate) override
        {
            mBuffer.acquire(mBufferName);
            length = static_cast<unsigned long>(mBuffer.get_length());
            samplingRate = mBuffer.get_sample_rate();
        }
        
        void release() override
        {
            mBuffer.release();
        };
        
        void read(double *output, const double *positions, unsigned long size, long chan, InterpType interp, EdgeMode edges, bool bound) override
        {
            chan = std::max(0L, std::min(chan, static_cast<long>(mBuffer.get_num_chans() - 1)));
            ibuffer_read_edges(mBuffer, output, positions, size, chan, 1.0, interp, edges, bound);
        }
        
        FrameLib_Read::Proxy *clone() const override
        {
            ReadProxy *proxy = new ReadProxy(getOwner<t_object>());
            proxy->mBufferName = mBufferName;
            
            return proxy;
        }
        
    private:
        
        ibuffer_data mBuffer;
        t_symbol *mBufferName;
    };
    
public:
     
    // Constructor
    
    FrameLib_MaxClass_Read(t_object *x, t_symbol *s, long argc, t_atom *argv)
    : FrameLib_MaxClass(x, s, argc, argv, new ReadProxy(x)) {}
};

// Max Object

#ifndef FRAMELIB_MAX_SINGLE_OBJECT

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass_Read::makeClass<FrameLib_MaxClass_Read>("fl.read~");
}

#endif
