
#ifndef FRAMELIB_PAD_H
#define FRAMELIB_PAD_H

#include "FrameLib_DSP.h"

class FrameLib_Pad : public FrameLib_Processor, private FrameLib_Info
{
    // Parameter Enums and Info

	enum ParameterList { kPadding, kStart, kEnd, kUnits };
    enum Units { kSamples, kRatio };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
	
    // Constructor
    
    FrameLib_Pad(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    
    // Info
    
    const char *objectInfo(bool verbose);
    const char *inputInfo(unsigned long idx, bool verbose);
    const char *outputInfo(unsigned long idx, bool verbose);

private:
    
    // Process
    
    void process ();
    
    // Data
    
    static ParameterInfo sParamInfo;
};

#endif
