
#include "FrameLib_FilterTags.h"
#include "FrameLib_MaxClass.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass_Expand<FrameLib_FilterTags>::makeClass(CLASS_BOX, "fl.filtertags~");
}

