
#ifndef FRAMELIB_TOHOST_H
#define FRAMELIB_TOHOST_H

#include "FrameLib_DSP.h"

class FrameLib_ToHost : public FrameLib_Processor
{
    // The owner should inherit from this class and override these calls to receive messages
    
public:
    
    struct Proxy : virtual FrameLib_Proxy
    {
        friend FrameLib_ToHost;
        
        virtual void output(const double *values, unsigned long N) {}
        virtual void output(const FrameLib_Parameters::Serial *serial) {}
        
        // These methods should only be used to allocate temporary memory in the overrides of the output methods
        
        template <class T> T *alloc(unsigned long N) { return mObject->alloc<T>(N); }
        template <class T> void dealloc(T *& ptr) { return mObject->dealloc(ptr); }
        
    private:
        
        FrameLib_ToHost *mObject;
    };
    
public:
    
    // Constructor
    
    FrameLib_ToHost(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose);
    std::string inputInfo(unsigned long idx, bool verbose);
    std::string outputInfo(unsigned long idx, bool verbose);
    
private:
    
    void process();
    
    // Data
    
    Proxy *mProxy;
};

#endif
