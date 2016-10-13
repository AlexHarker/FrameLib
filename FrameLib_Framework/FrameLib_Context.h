
#ifndef FRAMELIB_CONTEXT_H
#define FRAMELIB_CONTEXT_H

// Forward Definitions

class FrameLib_Global;
class FrameLib_GlobalAllocator;
class FrameLib_LocalAllocator;


// The context object (used to define non-connectable areas in the host ennvironment

struct FrameLib_Context
{
    // Constructor - the reference should be a suitable reference address in the host environment
    
    FrameLib_Context(FrameLib_Global *global, void *reference) : mGlobal(global), mReference(reference) {}
    
    // Methods to retrieve common objects

    FrameLib_LocalAllocator *getAllocator();
    
    // FIX - hack for now...
    
    void *getConnectionQueue();
    void *getDSPQueue();
    
    // Methods to release common objects

    void releaseAllocator();
    void releaseConnectionQueue();
    void releaseDSPQueue();
    
private:
    
    FrameLib_Global *mGlobal;
    void *mReference;
};

#endif