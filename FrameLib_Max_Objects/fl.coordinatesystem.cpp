
#include "FrameLib_CoordinateSystem.h"
#include "Framelib_Max.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxObj<FrameLib_Expand<FrameLib_CoordinateSystem> >::makeClass(CLASS_BOX, "fl.coordinatesystem~");
}
