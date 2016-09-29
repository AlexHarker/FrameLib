
#ifndef FRAMELIB_CONTEXT_H
#define FRAMELIB_CONTEXT_H

// Forward Definitions

class FrameLib_Global;
class FrameLib_Global_Allocator;
class FrameLib_Local_Allocator;


struct FrameLib_Context
{
    FrameLib_Context(FrameLib_Global *global, void *ref) : mGlobal(global), mReference(ref) {}
    
    FrameLib_Global_Allocator *getGlobalAllocator();
    FrameLib_Local_Allocator *getLocalAllocator();
    
    // FIX - hack for now...
    
    void *getConnectionQueue();
    void *getDSPQueue();
    
    void releaseLocalAllocator();
    void releaseConnectionQueue();
    void releaseDSPQueue();

    
private:
    
    FrameLib_Global *mGlobal;
    void *mReference;
};

#endif