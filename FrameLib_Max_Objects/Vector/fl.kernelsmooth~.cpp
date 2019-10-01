
#include "FrameLib_KernelSmooth.h"
#include "FrameLib_MaxClass.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass_Expand<FrameLib_KernelSmooth>::makeClass("fl.kernelsmooth~");
}
