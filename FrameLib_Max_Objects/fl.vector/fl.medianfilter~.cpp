
#include "FrameLib_MedianFilter.h"
#include "FrameLib_MaxClass.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass_Expand<FrameLib_MedianFilter>::makeClass(CLASS_BOX, "fl.medianfilter~");
}
