
#include "FrameLib_TimeMean.h"
#include "FrameLib_MaxClass.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass_Expand<FrameLib_TimeMean>::makeClass(CLASS_BOX, "fl.timemean~");
}
