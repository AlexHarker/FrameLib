
#include "FrameLib_Convolve.h"
#include "Framelib_Max.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxObj<FrameLib_Expand<FrameLib_Convolve> >::makeClass(CLASS_BOX, "fl.convolve~");
}
