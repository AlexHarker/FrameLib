
#include "FrameLib_Shift.h"
#include "Framelib_Max.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxObj<FrameLib_Expand<FrameLib_Shift> >::makeClass(CLASS_BOX, "fl.shift~");
}
