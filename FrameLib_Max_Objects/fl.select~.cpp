
#include "FrameLib_Select.h"
#include "Framelib_Max.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxObj<FrameLib_Expand<FrameLib_Select> >::makeClass(CLASS_BOX, "fl.select~");
}
