
#include "FrameLib_Source.h"
#include "Framelib_Max.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxObj<FrameLib_Expand<FrameLib_Source> >::makeClass(CLASS_BOX, "fl.source~");
}
