
#include "FrameLib_Once.h"
#include "FrameLib_MaxClass.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass<FrameLib_Expand <FrameLib_Once> >::makeClass(CLASS_BOX, "fl.once~");
}
