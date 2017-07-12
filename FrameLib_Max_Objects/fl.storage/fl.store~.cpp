
#include "FrameLib_Store.h"
#include "FrameLib_MaxClass.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass<FrameLib_Expand<FrameLib_Store> >::makeClass(CLASS_BOX, "fl.store~");
}
