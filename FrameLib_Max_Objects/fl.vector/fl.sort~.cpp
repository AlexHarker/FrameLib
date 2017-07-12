
#include "FrameLib_Sort.h"
#include "FrameLib_MaxClass.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass<FrameLib_Expand<FrameLib_Sort> >::makeClass(CLASS_BOX, "fl.sort~");
}
