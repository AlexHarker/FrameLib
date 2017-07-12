
#include "FrameLib_Percentile.h"
#include "FrameLib_MaxClass.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass<FrameLib_Expand<FrameLib_Percentile> >::makeClass(CLASS_BOX, "fl.percentile~");
}
