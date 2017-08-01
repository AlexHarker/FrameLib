
#ifndef FRAMELIB_PAD_H
#define FRAMELIB_PAD_H

#include "FrameLib_DSP.h"

class FrameLib_Pad : public FrameLib_Processor
{
    // Parameter Enums and Info

	enum ParameterList { kPadding, kStart, kEnd, kUnits };
    enum Units { kSamples, kRatio };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
	
    // Constructor
    
    FrameLib_Pad(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    
    // Info
    
    std::string objectInfo(bool verbose);
    std::string inputInfo(unsigned long idx, bool verbose);
    std::string outputInfo(unsigned long idx, bool verbose);

private:
    
    // Process
    
    void process ();
    
    // Data
    
    static ParameterInfo sParamInfo;
};

#endif
