
#include "FrameLib_Context.h"

// Methods to retrieve common objects

FrameLib_LocalAllocator *FrameLib_Context::getAllocator() const
{
    return mGlobal->getAllocator(mReference);
}

FrameLib_ConnectionQueue *FrameLib_Context::getConnectionQueue() const
{
    return mGlobal->getConnectionQueue(mReference);
}

FrameLib_DSPQueue *FrameLib_Context::getDSPQueue() const
{
    return mGlobal->getDSPQueue(mReference);
}

// Methods to release common objects

void FrameLib_Context::releaseAllocator() const
{
    mGlobal->releaseAllocator(mReference);
}

void FrameLib_Context::releaseConnectionQueue() const
{
    mGlobal->releaseConnectionQueue(mReference);
}

void FrameLib_Context::releaseDSPQueue() const
{
    mGlobal->releaseDSPQueue(mReference);
}
