
#include "FrameLib_TimeStdDev.h"
#include "FrameLib_MaxClass.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass_Expand<FrameLib_TimeStdDev>::makeClass(CLASS_BOX, "fl.timestddev~");
}
