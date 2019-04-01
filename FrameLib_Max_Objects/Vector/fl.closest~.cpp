// created by james bradbury 31/03/19
#include "FrameLib_Closest.h"
#include "FrameLib_MaxClass.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass_Expand<FrameLib_Closest>::makeClass(CLASS_BOX, "fl.closest~");
}
