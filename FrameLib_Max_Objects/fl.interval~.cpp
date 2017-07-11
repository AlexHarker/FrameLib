
#include "FrameLib_Interval.h"
#include "Framelib_Max.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxObj<FrameLib_Expand <FrameLib_Interval> >::makeClass(CLASS_BOX, "fl.interval~");
}
