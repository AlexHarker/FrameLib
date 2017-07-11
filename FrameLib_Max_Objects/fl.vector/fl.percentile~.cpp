
#include "FrameLib_Percentile.h"
#include "Framelib_Max.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxObj<FrameLib_Expand<FrameLib_Percentile> >::makeClass(CLASS_BOX, "fl.percentile~");
}
