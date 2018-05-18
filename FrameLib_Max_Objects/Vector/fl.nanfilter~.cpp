

#include "FrameLib_NanFilter.h"
#include "FrameLib_MaxClass.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass_Expand<FrameLib_NanFilter>::makeClass(CLASS_BOX, "fl.nanfilter~");
}
