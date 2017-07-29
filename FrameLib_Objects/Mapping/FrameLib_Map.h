
#ifndef FRAMELIB_MAP_H
#define FRAMELIB_MAP_H

#include "FrameLib_DSP.h"

class FrameLib_Map : public FrameLib_Processor, private FrameLib_Info
{
    // Parameter Enums and Info

    enum ParameterList { kMode, kInLo, kInHi, kOutLo, kOutHi, kClip };
    enum Modes { kLinear, kLog, kExp, kDB, kInvDB, kTranspose, kInvTranspose };
    enum ScaleMode { kScaleLinear, kScaleExp, kScaleLog };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };
    
public:
    
    // Constructor

    FrameLib_Map(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    
    // Info
    
    const char *objectInfo(bool verbose);
    const char *inputInfo(unsigned long idx, bool verbose);
    const char *outputInfo(unsigned long idx, bool verbose);

private:
    
    // Helpers
    
    void setScaling();
    
    // Update and Process
    
    void update();
    void process();
    
    // Data
    
    ScaleMode mMode;
    
    double mMul;
    double mSub;
    double mMin;
    double mMax;
    
    static ParameterInfo sParamInfo;
};

#endif
