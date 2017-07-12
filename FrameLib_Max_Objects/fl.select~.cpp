
#include "FrameLib_Select.h"
#include "FrameLib_MaxClass.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass_Expand<FrameLib_Select>::makeClass(CLASS_BOX, "fl.select~");
}
