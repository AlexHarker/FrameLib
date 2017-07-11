
#include "FrameLib_Recall.h"
#include "Framelib_Max.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxObj<FrameLib_Expand<FrameLib_Recall> >::makeClass(CLASS_BOX, "fl.recall~");
}
