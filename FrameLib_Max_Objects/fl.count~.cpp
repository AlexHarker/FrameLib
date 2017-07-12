
#include "FrameLib_Count.h"
#include "FrameLib_MaxClass.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass<FrameLib_Expand<FrameLib_Count> >::makeClass(CLASS_BOX, "fl.count~");
}