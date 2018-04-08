
#include "FrameLib_Reverse.h"
#include "FrameLib_MaxClass.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass_Expand<FrameLib_Reverse>::makeClass(CLASS_BOX, "fl.reverse~");
}
