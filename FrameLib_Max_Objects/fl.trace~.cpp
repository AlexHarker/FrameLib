
#include "FrameLib_Trace.h"
#include "Framelib_Max.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxObj<FrameLib_Expand<FrameLib_Trace> >::makeClass(CLASS_BOX, "fl.trace~");
}
