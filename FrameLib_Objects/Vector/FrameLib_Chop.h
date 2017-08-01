
#ifndef FRAMELIB_CHOP_H
#define FRAMELIB_CHOP_H

#include "FrameLib_DSP.h"

class FrameLib_Chop : public FrameLib_Processor
{
    // Parameter Enums and Info

	enum ParameterList { kNumOuts, kSize, kUnits };
    enum Units { kSamples, kRatio };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
	
    // Constructor
    
    FrameLib_Chop(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    
    // Info
    
    std::string objectInfo(bool verbose);
    std::string inputInfo(unsigned long idx, bool verbose);
    std::string outputInfo(unsigned long idx, bool verbose);

private:
    
    // Process
    
    void process();
    
    // Data
    
    unsigned long mNumOuts;
    
    static ParameterInfo sParamInfo;
};

#endif
