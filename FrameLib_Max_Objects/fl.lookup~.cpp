
#include "FrameLib_Lookup.h"
#include "Framelib_Max.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxObj<FrameLib_Expand<FrameLib_Lookup> >::makeClass(CLASS_BOX, "fl.lookup~");
}

