
#include "FrameLib_Chop.h"
#include "Framelib_Max.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxObj<FrameLib_Expand<FrameLib_Chop> >::makeClass(CLASS_BOX, "fl.chop~");
}
