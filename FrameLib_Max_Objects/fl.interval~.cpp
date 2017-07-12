
#include "FrameLib_Interval.h"
#include "FrameLib_MaxClass.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass<FrameLib_Expand <FrameLib_Interval> >::makeClass(CLASS_BOX, "fl.interval~");
}
