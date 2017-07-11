
#include "FrameLib_OnePoleZero.h"
#include "Framelib_Max.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxObj<FrameLib_Expand<FrameLib_OnePoleZero> >::makeClass(CLASS_BOX, "fl.onepolezero~");
}
