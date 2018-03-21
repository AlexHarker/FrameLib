#include "FrameLib_TernaryObjects.h"
#include "FrameLib_MaxClass.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass_Expand<FrameLib_Clip, kDistribute>::makeClass(CLASS_BOX, "fl.clip~");
}

