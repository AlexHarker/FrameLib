
#include "FrameLib_Objects.h"
#include "Framelib_Max.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxObj<FrameLib_Expand<FrameLib_Cbrt> >::makeClass(CLASS_BOX, "fl.cbrt~");
}
