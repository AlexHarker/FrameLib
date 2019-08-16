
#include "FrameLib_AccumPoint.h"
#include "FrameLib_MaxClass.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass_Expand<FrameLib_AccumPoint>::makeClass(CLASS_BOX, "fl.accumpoint~");
}
