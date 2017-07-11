
#include "FrameLib_MedianFilter.h"
#include "Framelib_Max.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxObj<FrameLib_Expand<FrameLib_MedianFilter> >::makeClass(CLASS_BOX, "fl.medianfilter~");
}
