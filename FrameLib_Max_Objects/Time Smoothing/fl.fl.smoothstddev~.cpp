
#include "FrameLib_SmoothStdDev.h"
#include "FrameLib_MaxClass.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass_Expand<FrameLib_SmoothStdDev>::makeClass(CLASS_BOX, "fl.smoothstddev~");
}
