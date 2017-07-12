
#include "FrameLib_Chop.h"
#include "FrameLib_MaxClass.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass<FrameLib_Expand<FrameLib_Chop> >::makeClass(CLASS_BOX, "fl.chop~");
}
