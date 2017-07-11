
#include "FrameLib_Objects.h"

#define OBJECT_ARGS_SET_ALL_INPUTS 1
#include "Framelib_Max.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxObj<FrameLib_Expand<FrameLib_Hypot> >::makeClass(CLASS_BOX, "fl.hypot~");
}
