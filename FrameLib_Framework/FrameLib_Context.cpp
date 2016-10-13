
#include "FrameLib_Context.h"
#include "FrameLib_Global.h"

// Methods to retrieve common objects

FrameLib_LocalAllocator *FrameLib_Context::getAllocator()
{
    return mGlobal->getAllocator(mReference);
}

void *FrameLib_Context::getConnectionQueue()
{
    return mGlobal->getConnectionQueue(mReference);
}

void *FrameLib_Context::getDSPQueue()
{
    return mGlobal->getDSPQueue(mReference);
}

// Methods to release common objects

void FrameLib_Context::releaseAllocator()
{
    mGlobal->releaseAllocator(mReference);
}

void FrameLib_Context::releaseConnectionQueue()
{
    mGlobal->releaseConnectionQueue(mReference);
}

void FrameLib_Context::releaseDSPQueue()
{
    mGlobal->releaseDSPQueue(mReference);
}

