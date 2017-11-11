
#include "FrameLib_SmoothMedian.h"
#include "FrameLib_MaxClass.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass_Expand<FrameLib_SmoothMedian>::makeClass(CLASS_BOX, "fl.smoothmedian~");
}
