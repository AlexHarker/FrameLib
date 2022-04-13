
#ifndef FRAMELIB_PARAMALIAS_H
#define FRAMELIB_PARAMALIAS_H

#include "FrameLib_DSP.h"

class FrameLib_ParamAlias : public FrameLib_Processor
{
    using Parameters = FrameLib_Parameters;
    using Serial = FrameLib_Parameters::Serial;
    using Type = FrameLib_Parameters::Type;
    using NumericType = FrameLib_Parameters::NumericType;
    using ClipMode = FrameLib_Parameters::ClipMode;
    
    struct Alias
    {
        Alias() : mIndex(0) {}
        Alias(unsigned long idx, const char* inTag, const char* outTag, long argumentIdx) : mIndex(idx), mInTag(inTag), mOutTag(outTag)
        {
            if (argumentIdx >= 0)
                mArgumentStr = FrameLib_StringMaker<>(argumentIdx);
        }
        
        unsigned long mIndex;
        std::string mArgumentStr;
        std::string mInTag;
        std::string mOutTag;
    };
    
public:
    
    // Constructor
    
    FrameLib_ParamAlias(FrameLib_Context context, unsigned long numOuts, FrameLib_Proxy *proxy, FrameLib_Parameters::Info *info);
    
    FrameLib_Parameters::Serial *aliasForConstruction(Serial *parametersIn, unsigned long idx);
    void initialise();

    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;
    
protected:
    
    void addAlias(unsigned long idx, const char* inTag, const char* outTag, long argumentIdx = -1);
    
private:
    
    // Process
    
    void process() override;
    
    // Data

    FrameLib_Parameters::AutoSerial mSerial;
    std::vector<Alias> mAliases;
};

#endif
