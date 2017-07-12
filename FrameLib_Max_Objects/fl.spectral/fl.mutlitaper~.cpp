
#include "FrameLib_Multitaper.h"
#include "FrameLib_MaxClass.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass<FrameLib_Expand<FrameLib_Multitaper> >::makeClass(CLASS_BOX, "fl.multitaper~");
}
