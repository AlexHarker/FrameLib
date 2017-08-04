
#include "FrameLib_Future.h"
#include "FrameLib_MaxClass.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass<FrameLib_Expand <FrameLib_Future> >::makeClass(CLASS_BOX, "fl.future~");
}
