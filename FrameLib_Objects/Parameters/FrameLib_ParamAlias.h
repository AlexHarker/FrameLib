
#ifndef FRAMELIB_PARAMALIAS_H
#define FRAMELIB_PARAMALIAS_H

#include "FrameLib_DSP.h"

class FrameLib_ParamAlias : public FrameLib_Processor
{
    typedef FrameLib_Parameters::Serial Serial;
    typedef FrameLib_Parameters Parameters;
    
    struct Alias
    {
        Alias() : mIndex(0) {}
        Alias(unsigned long idx, const char* inTag, const char* outTag) : mIndex(idx), mInTag(inTag), mOutTag(outTag) {}
        
        unsigned long mIndex;
        std::string mInTag;
        std::string mOutTag;
    };
    
public:
    
    // Constructor
    
    FrameLib_ParamAlias(FrameLib_Context context, unsigned long numOuts, void *owner, FrameLib_Parameters::Info *info);
    
    FrameLib_Parameters::Serial *aliasForConstruction(FrameLib_Parameters::Serial *parametersIn, unsigned long idx);
    void initialise(FrameLib_Parameters::Serial *serialisedParameters);

    // Info
    
    std::string objectInfo(bool verbose);
    std::string inputInfo(unsigned long idx, bool verbose);
    std::string outputInfo(unsigned long idx, bool verbose);
    
protected:
    
    void addAlias(unsigned long idx, const char* inTag, const char* outTag);
    
private:
    
    // Process
    
    void process();
    
    // Data

    FrameLib_Parameters::AutoSerial mSerial;
    std::vector<Alias> mAliases;
};

#endif
