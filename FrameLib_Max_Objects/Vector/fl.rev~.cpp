
#include "FrameLib_Rev.h"
#include "FrameLib_MaxClass.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass_Expand<FrameLib_Rev>::makeClass(CLASS_BOX, "fl.rev~");
}
