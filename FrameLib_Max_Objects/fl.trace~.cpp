
#include "FrameLib_Trace.h"
#include "FrameLib_MaxClass.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass<FrameLib_Expand<FrameLib_Trace> >::makeClass(CLASS_BOX, "fl.trace~");
}
