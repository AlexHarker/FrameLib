
#ifndef FRAMELIB_GETPARAM_H
#define FRAMELIB_GETPARAM_H

#include "FrameLib_DSP.h"

class FrameLib_GetParam : public FrameLib_Processor
{
    const static int maxNumIns = 10;
    
    // Parameter Enums and Info

    //enum ParameterList { kNumIns, kNames };
    
    //struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_GetParam(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose);
    std::string inputInfo(unsigned long idx, bool verbose);
    std::string outputInfo(unsigned long idx, bool verbose);
    
private:
    
    // Object reset
    
    void objectReset();
    
    // Process
    
    void process();
    
    // Data
    
    FrameLib_Block *mConnectedObject;
    
    //static ParameterInfo sParamInfo;
};

#endif
