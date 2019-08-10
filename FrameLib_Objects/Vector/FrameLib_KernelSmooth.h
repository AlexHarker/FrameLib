
#ifndef FRAMELIB_KERNELSMOOTH_H
#define FRAMELIB_KERNELSMOOTH_H

#include "FrameLib_DSP.h"

class FrameLib_KernelSmooth final : public FrameLib_Processor
{
    enum ParameterList { kSmooth };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

    struct Allocator
    {
        Allocator(FrameLib_KernelSmooth& owner) : mOwner(owner) {}
        
        template <typename T>
        T* alloc(size_t size) { return mOwner.alloc<T>(size); }
        
        template <typename T>
        void dealloc(T *ptr) { mOwner.dealloc(ptr); }
        
        FrameLib_KernelSmooth& mOwner;
    };
    
public:
    
    // Constructor

    FrameLib_KernelSmooth(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;

private:
    
    // Process
    
    void process() override;
    
    // Data
    
    static ParameterInfo sParamInfo;
};

#endif
