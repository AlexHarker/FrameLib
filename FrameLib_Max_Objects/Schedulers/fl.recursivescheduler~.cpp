
#include "FrameLib_Recursive.h"
#include "FrameLib_MaxClass.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass_Expand<FrameLib_Recursive>::makeClass("fl.recursive~");
    FrameLib_MaxClass_Expand<FrameLib_Recursive::Time>::makeClass("fl.nexttime~");
}
