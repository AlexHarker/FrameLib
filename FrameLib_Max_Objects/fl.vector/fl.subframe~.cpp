
#include "FrameLib_Subframe.h"
#include "Framelib_Max.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxObj<FrameLib_Expand<FrameLib_Subframe> >::makeClass(CLASS_BOX, "fl.subframe~");
}
