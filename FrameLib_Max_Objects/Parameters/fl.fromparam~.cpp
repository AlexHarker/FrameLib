
#include "FrameLib_Untag.h"
#include "FrameLib_MaxClass.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass_Expand<FrameLib_Untag>::makeClass(CLASS_BOX, "fl.untag~");
}

