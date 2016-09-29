
#include "FrameLib_Context.h"
#include "FrameLib_Global.h"

FrameLib_Global_Allocator *FrameLib_Context::getGlobalAllocator() { return mGlobal->getGlobalAllocator(); }
FrameLib_Local_Allocator *FrameLib_Context::getLocalAllocator() { return mGlobal->getLocalAllocator(mReference); }
void *FrameLib_Context::getConnectionQueue() { return mGlobal->getConnectionQueue(mReference); }
void *FrameLib_Context::getDSPQueue() { return mGlobal->getDSPQueue(mReference); }
    
void FrameLib_Context::releaseLocalAllocator() { mGlobal->releaseLocalAllocator(mReference); }
void FrameLib_Context::releaseConnectionQueue() { mGlobal->releaseConnectionQueue(mReference); }
void FrameLib_Context::releaseDSPQueue() { mGlobal->releaseDSPQueue(mReference); }


