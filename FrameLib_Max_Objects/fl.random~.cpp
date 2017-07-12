
#include "FrameLib_Random.h"
#include "FrameLib_MaxClass.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass<FrameLib_Expand<FrameLib_Random> >::makeClass(CLASS_BOX, "fl.random~");
}

