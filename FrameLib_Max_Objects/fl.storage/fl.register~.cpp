
#include "FrameLib_Register.h"
#include "Framelib_Max.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxObj<FrameLib_Expand<FrameLib_Register> >::makeClass(CLASS_BOX, "fl.register~");
}
