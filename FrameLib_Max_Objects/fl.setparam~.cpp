
#include "FrameLib_SetParam.h"
#include "Framelib_Max.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxObj<FrameLib_Expand<FrameLib_SetParam> >::makeClass(CLASS_BOX, "fl.setparam~");
}

