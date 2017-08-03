
#include "FrameLib_Time.h"
#include "FrameLib_MaxClass.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass<FrameLib_Expand <FrameLib_Time> >::makeClass(CLASS_BOX, "fl.now~");
}
