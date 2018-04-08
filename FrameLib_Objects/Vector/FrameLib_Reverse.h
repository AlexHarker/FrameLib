
#ifndef FRAMELIB_REV_H
#define FRAMELIB_REV_H

#include "FrameLib_DSP.h"

class FrameLib_Reverse : public FrameLib_Processor
{
    
public:
	
    // Constructor

    FrameLib_Reverse(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose);
    std::string inputInfo(unsigned long idx, bool verbose);
    std::string outputInfo(unsigned long idx, bool verbose);

private:
    
    // Process
    
    void process();
};

#endif
