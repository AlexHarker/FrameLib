
#include "FrameLib_Count.h"
#include "Framelib_Max.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxObj<FrameLib_Expand<FrameLib_Count> >::makeClass(CLASS_BOX, "fl.count~");
}