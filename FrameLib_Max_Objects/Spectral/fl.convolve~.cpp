
#include "FrameLib_Convolve.h"
#include "FrameLib_MaxClass.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass_Expand<FrameLib_Convolve>::makeClass("fl.convolve~");
}
