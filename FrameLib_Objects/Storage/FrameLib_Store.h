
#ifndef FRAMELIB_STORE_H
#define FRAMELIB_STORE_H

#include "FrameLib_DSP.h"

// FIX - Threadsafety (is threadsafe for now because is scoped within same thread for now)

class FrameLib_Store : public FrameLib_Processor
{
    // Parameter Enums and Info

	enum ParameterList { kName };
    
public:
	
    // Constructor / Destructor

    FrameLib_Store(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    ~FrameLib_Store();
    
private:
    
    // Process
    
    void process();
    
    // Data
    
    FrameLib_LocalAllocator::Storage *mStorage;
};

#endif