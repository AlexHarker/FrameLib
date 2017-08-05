
#ifndef FRAMELIB_MAP_H
#define FRAMELIB_MAP_H

#include "FrameLib_DSP.h"
#include "FrameLib_Scaling_Functions.h"

class FrameLib_Map : public FrameLib_Processor, private FrameLib_ClipScaler<double>
{
    // Parameter Enums and Info

    enum ParameterList { kMode, kInLo, kInHi, kOutLo, kOutHi, kClip };
    enum Modes { kLinear, kLog, kExp, kDB, kInvDB, kTranspose, kInvTranspose };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };
        
public:
    
    // Constructor

    FrameLib_Map(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    
    // Info
    
    std::string objectInfo(bool verbose);
    std::string inputInfo(unsigned long idx, bool verbose);
    std::string outputInfo(unsigned long idx, bool verbose);

private:
    
    // Helpers
    
    void setScaling();
    
    // Update and Process
    
    void update();
    void process();
    
    // Data
        
    static ParameterInfo sParamInfo;
};

#endif
