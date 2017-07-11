
#include "FrameLib_Split.h"
#include "Framelib_Max.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxObj<FrameLib_Expand<FrameLib_Split> >::makeClass(CLASS_BOX, "fl.split~");
}
