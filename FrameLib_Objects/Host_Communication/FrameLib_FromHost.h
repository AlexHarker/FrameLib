
#ifndef FRAMELIB_FROMHOST_H
#define FRAMELIB_FROMHOST_H

#include "FrameLib_DSP.h"

class FrameLib_FromHost : public FrameLib_Processor
{
    // The owner should inherit from this class and use these calls to write to all registered objects
    
public:
    
    struct Proxy : virtual FrameLib_Proxy
    {
        friend FrameLib_FromHost;
        
        void write(const double *values, unsigned long N);
        void write(const FrameLib_Parameters::Serial *serial);
        void write(const char *tag, const char *string);
        void write(const char *tag, const double *values, unsigned long N);

    private:
                   
        void registerObject(FrameLib_FromHost *object);
        void unregisterObject(FrameLib_FromHost *object);

        std::vector<FrameLib_FromHost *> mRegistered;
    };
    
private:
    
    // Parameter Info and Enums
    
    enum ParameterList { kMode };
    enum Modes { kValues, kParams };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor / Destructor
    
    FrameLib_FromHost(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    ~FrameLib_FromHost();
    
    // Info
    
    std::string objectInfo(bool verbose);
    std::string inputInfo(unsigned long idx, bool verbose);
    std::string outputInfo(unsigned long idx, bool verbose);

private:
    
    void process();
    
// Data
    
    SpinLock mLock;
    std::vector<double> *mVectorFrame;
    FrameLib_Parameters::AutoSerial *mSerialFrame;
    
    Modes mMode;
    Proxy *mProxy;
    
    static ParameterInfo sParamInfo;
};

#endif
