
#include "FrameLib_Join.h"
#include "Framelib_Max.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxObj<FrameLib_Expand<FrameLib_Join> >::makeClass(CLASS_BOX, "fl.join~");
}
