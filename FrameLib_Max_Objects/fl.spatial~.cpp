
#include "FrameLib_Spatial.h"
#include "FrameLib_MaxClass.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass<FrameLib_Expand<FrameLib_Spatial> >::makeClass(CLASS_BOX, "fl.spatial~");
}
