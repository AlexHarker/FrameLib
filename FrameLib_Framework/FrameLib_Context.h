
#ifndef FRAMELIB_CONTEXT_H
#define FRAMELIB_CONTEXT_H

// Forward Definitions

class FrameLib_Global;
class FrameLib_GlobalAllocator;
class FrameLib_LocalAllocator;
class FrameLib_DSPQueue;


// The context object (used to define non-connectable areas in the host ennvironment

class FrameLib_Context
{
    
public:
    
    // Constructor - the reference should be a suitable reference address in the host environment
    
    FrameLib_Context(FrameLib_Global *global, void *reference) : mGlobal(global), mReference(reference) {}
    
    // Methods to retrieve common objects

    FrameLib_LocalAllocator *getAllocator() const;
    
    // FIX - hack for now...
    
    void *getConnectionQueue() const;
    FrameLib_DSPQueue *getDSPQueue() const;
    
    // Methods to release common objects

    void releaseAllocator() const;
    void releaseConnectionQueue() const;
    void releaseDSPQueue() const;
    
private:
    
    FrameLib_Global *mGlobal;
    void *mReference;
};

#endif