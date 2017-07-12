
#include "FrameLib_Recall.h"
#include "FrameLib_MaxClass.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass<FrameLib_Expand<FrameLib_Recall> >::makeClass(CLASS_BOX, "fl.recall~");
}
