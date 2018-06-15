
#ifndef FRAMELIB_TOHOST_H
#define FRAMELIB_TOHOST_H

#include "FrameLib_DSP.h"
#include "FrameLib_HostProxy.h"

class FrameLib_ToHost : public FrameLib_Processor
{
    
public:
    
    // The owner should inherit from this class and override these calls to receive messages

    struct Proxy : public FrameLib_HostProxy<FrameLib_ToHost>
    {
        friend FrameLib_ToHost;
        
        Proxy() : mObject(nullptr) {}
        
        virtual void sendToHost(unsigned long index, unsigned long stream, const double *values, unsigned long N) = 0;
        virtual void sendToHost(unsigned long index, unsigned long stream, const FrameLib_Parameters::Serial *serial)= 0;
        
        // These methods should only be used to allocate temporary memory in the overrides of the output methods
        
        template <class T> T *alloc(unsigned long N) { return mObject->alloc<T>(N); }
        template <class T> void dealloc(T *& ptr) { return mObject->dealloc(ptr); }
        
    private:
        
        FrameLib_ToHost *mObject;
    };
    
public:
    
    // Constructor / Destructor
    
    FrameLib_ToHost(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    ~FrameLib_ToHost();
    
    // Stream Awareness
    
    virtual void setStream(void *streamOwner, unsigned long stream);

    // Info
    
    std::string objectInfo(bool verbose);
    std::string inputInfo(unsigned long idx, bool verbose);
    std::string outputInfo(unsigned long idx, bool verbose);
    
private:
    
    void process();
    
    // Data
    
    Proxy *mProxy;
    void *mStreamOwner;
    unsigned long mStream;
    unsigned long  mID;
};

#endif
