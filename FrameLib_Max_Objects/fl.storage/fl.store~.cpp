
#include "FrameLib_Store.h"
#include "Framelib_Max.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxObj<FrameLib_Expand<FrameLib_Store> >::makeClass(CLASS_BOX, "fl.store~");
}
