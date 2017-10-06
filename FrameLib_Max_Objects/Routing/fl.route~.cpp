
#include "FrameLib_Route.h"
#include "FrameLib_MaxClass.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass_Expand<FrameLib_Route>::makeClass(CLASS_BOX, "fl.route~");
}
