
#include "FrameLib_Resonant.h"
#include "Framelib_Max.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxObj<FrameLib_Expand<FrameLib_Resonant> >::makeClass(CLASS_BOX, "fl.resonant~");
}
