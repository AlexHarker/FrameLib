
#include "FrameLib_Context.h"
#include "FrameLib_Global.h"

FrameLib_Local_Allocator *FrameLib_Context::getAllocator() { return mGlobal->getAllocator(mReference); }
void *FrameLib_Context::getConnectionQueue() { return mGlobal->getConnectionQueue(mReference); }
void *FrameLib_Context::getDSPQueue() { return mGlobal->getDSPQueue(mReference); }
    
void FrameLib_Context::releaseAllocator() { mGlobal->releaseAllocator(mReference); }
void FrameLib_Context::releaseConnectionQueue() { mGlobal->releaseConnectionQueue(mReference); }
void FrameLib_Context::releaseDSPQueue() { mGlobal->releaseDSPQueue(mReference); }


