
#include "FrameLib_Source.h"
#include "FrameLib_MaxClass.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass<FrameLib_Expand<FrameLib_Source> >::makeClass(CLASS_BOX, "fl.source~");
}
