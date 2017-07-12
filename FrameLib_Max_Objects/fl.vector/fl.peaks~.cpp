
#include "FrameLib_Peaks.h"
#include "FrameLib_MaxClass.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass<FrameLib_Expand<FrameLib_Peaks> >::makeClass(CLASS_BOX, "fl.peaks~");
}
