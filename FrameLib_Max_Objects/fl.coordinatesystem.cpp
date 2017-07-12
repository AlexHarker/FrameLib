
#include "FrameLib_CoordinateSystem.h"
#include "FrameLib_MaxClass.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass<FrameLib_Expand<FrameLib_CoordinateSystem> >::makeClass(CLASS_BOX, "fl.coordinatesystem~");
}
