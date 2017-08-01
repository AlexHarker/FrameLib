
#include "FrameLib_Shift.h"
#include "FrameLib_MaxClass.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass_Expand<FrameLib_Shift>::makeClass(CLASS_BOX, "fl.shift~");
}
