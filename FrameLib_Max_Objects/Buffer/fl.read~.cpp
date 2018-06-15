
#include "FrameLib_MaxClass.h"
#include "FrameLib_Read.h"
#include "ibuffer/ibuffer_access.hpp"

// Max Class

class FrameLib_MaxClass_Read : public FrameLib_MaxClass<FrameLib_Expand<FrameLib_Read> >
{
    struct ReadProxy : public FrameLib_Read::Proxy, public FrameLib_MaxProxy
    {
        ReadProxy();
        
        virtual void update(const char *name)
        {
            mBufferName = gensym(name);
        }
        
        virtual void acquire(unsigned long& length, double& samplingRate)
        {
            mBuffer.acquire(mBufferName);
            length = mBuffer.get_length();
            samplingRate = mBuffer.get_sample_rate();
        }
        
        virtual void release()
        {
            mBuffer.release();
        };
        
        virtual void read(double *output, const double *positions, unsigned long size, long chan, InterpType interpType)
        {
            ibuffer_read(mBuffer, output, positions, size, chan, 1.0, interpType);
        }
        
    private:
        
        ibuffer_data mBuffer;
        t_symbol *mBufferName;
        static bool sInit;
    };
    
public:
     
    // Constructor
    
    FrameLib_MaxClass_Read(t_symbol *s, long argc, t_atom *argv) : FrameLib_MaxClass(s, argc, argv, new ReadProxy()) {}
};

// Proxy Init

bool FrameLib_MaxClass_Read::ReadProxy::sInit = false;

// Proxy Constructor

FrameLib_MaxClass_Read::ReadProxy::ReadProxy() : mBuffer(nullptr)
{
    if (!sInit)
    {
        ibuffer_init();
        sInit = true;
    }
}

// Max Object

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass_Read::makeClass<FrameLib_MaxClass_Read>(CLASS_BOX, "fl.read~");
}
