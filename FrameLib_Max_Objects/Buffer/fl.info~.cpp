
#include "FrameLib_MaxClass.h"
#include "FrameLib_Info.h"
#include "ibuffer/ibuffer_access.hpp"

// Max Class

class FrameLib_MaxClass_Info : public FrameLib_MaxClass_Expand<FrameLib_Info>
{
    struct ReadProxy : public FrameLib_Info::Proxy, public FrameLib_MaxProxy
    {
        ReadProxy();
        
        void update(const char *name) override
        {
            mBufferName = gensym(name);
        }
        
        void acquire(unsigned long& length, double& samplingRate, unsigned long& chans) override
        {
            mBuffer.acquire(mBufferName);
            length = mBuffer.get_length();
            samplingRate = mBuffer.get_sample_rate();
            chans = mBuffer.get_num_chans();
        }
        
        void release() override
        {
            mBuffer.release();
        };
    // Remove read function as fl.info~ is not reading the buffer just returning its features. 25/11/18
        
    private:
        
        ibuffer_data mBuffer;
        t_symbol *mBufferName;
        static bool sInit;
    };
    
public:
     
    // Constructor
    
    FrameLib_MaxClass_Info(t_symbol *s, long argc, t_atom *argv) : FrameLib_MaxClass(s, argc, argv, new ReadProxy()) {}
};

// Proxy Init

bool FrameLib_MaxClass_Info::ReadProxy::sInit = false;

// Proxy Constructor

FrameLib_MaxClass_Info::ReadProxy::ReadProxy() : mBuffer(nullptr)
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
    FrameLib_MaxClass_Info::makeClass<FrameLib_MaxClass_Info>(CLASS_BOX, "fl.info~");
}
