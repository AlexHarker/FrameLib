#ifndef FrameLib_NanFilter_h
#define FrameLib_NanFilter_h

#include "FrameLib_DSP.h"

class FrameLib_NanFilter: public FrameLib_Processor
{
    
    template <typename E>
    constexpr auto enum_value(E e) const -> typename std::underlying_type<E>::type
    {
       return static_cast<typename std::underlying_type<E>::type>(e);
    }
    
    template <typename E>
    constexpr E enum_lookup(typename std::underlying_type<E>::type idx) const
    {
        return static_cast<E>(idx);
    }
    
    
    enum class ParameterList{kMode, kValue};
    enum class Modes {kReplace,kRemove};
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };
    
public:
    
    // Constructor
    
    FrameLib_NanFilter(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
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

#endif /* FrameLib_NanFilter_h */
