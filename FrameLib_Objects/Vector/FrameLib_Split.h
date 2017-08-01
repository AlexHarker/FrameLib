
#ifndef FRAMELIB_SPLIT_H
#define FRAMELIB_SPLIT_H

#include "FrameLib_DSP.h"

// FIX - consider multi-output version later

class FrameLib_Split : public FrameLib_Processor
{
    // Parameter Enums and Info

	enum ParameterList { kSplit, kUnits };
    enum Units { kSamples, kRatio };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
	
    // Constructor
    
    FrameLib_Split(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    
    // Info
    
    std::string objectInfo(bool verbose);
    std::string inputInfo(unsigned long idx, bool verbose);
    std::string outputInfo(unsigned long idx, bool verbose);

private:
    
    // Process

    void process();

    // Data
    
    static ParameterInfo sParamInfo;
};

#endif
