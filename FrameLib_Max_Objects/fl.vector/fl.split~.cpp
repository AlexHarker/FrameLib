
#include "FrameLib_Split.h"
#include "FrameLib_MaxClass.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass<FrameLib_Expand<FrameLib_Split> >::makeClass(CLASS_BOX, "fl.split~");
}
