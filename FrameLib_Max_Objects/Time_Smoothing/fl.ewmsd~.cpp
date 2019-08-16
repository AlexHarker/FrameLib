
#include "FrameLib_EWMSD.h"
#include "FrameLib_MaxClass.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass_Expand<FrameLib_EWMSD>::makeClass(CLASS_BOX, "fl.ewmsd~");
}
