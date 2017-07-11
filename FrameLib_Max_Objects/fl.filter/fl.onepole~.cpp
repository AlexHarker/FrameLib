
#include "FrameLib_OnePole.h"
#include "Framelib_Max.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxObj<FrameLib_Expand<FrameLib_OnePole> >::makeClass(CLASS_BOX, "fl.onepole~");
}
