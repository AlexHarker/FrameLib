
#ifndef FRAMELIB_TOHOST_H
#define FRAMELIB_TOHOST_H

#include "FrameLib_DSP.h"
#include "FrameLib_HostProxy.h"

class FrameLib_ToHost final : public FrameLib_Processor
{
public:
    
    // The owner should inherit from this class and override these calls to receive messages

    struct Proxy : public FrameLib_HostProxy<FrameLib_ToHost>
    {
        friend FrameLib_ToHost;
        
        Proxy() {}
        
        virtual void sendToHost(unsigned long index, unsigned long stream, const double *values, unsigned long N, FrameLib_TimeFormat time) = 0;
        virtual void sendToHost(unsigned long index, unsigned long stream, const FrameLib_Parameters::Serial *serial, FrameLib_TimeFormat time)= 0;
    };
    
public:
    
    // Constructor / Destructor
    
    FrameLib_ToHost(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    ~FrameLib_ToHost();
    
    // Stream Awareness
    
    void setStream(void *streamOwner, unsigned long stream) override;

    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;
    
private:
    
    void process() override;
    
    // Data
    
    Proxy *mProxy;
    void *mStreamOwner;
    unsigned long mStream;
    unsigned long  mID;
};

#endif
