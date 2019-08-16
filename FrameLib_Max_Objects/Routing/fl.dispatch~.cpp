
#include "FrameLib_Dispatch.h"
#include "FrameLib_MaxClass.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass_Expand<FrameLib_Dispatch>::makeClass(CLASS_BOX, "fl.dispatch~");
}
