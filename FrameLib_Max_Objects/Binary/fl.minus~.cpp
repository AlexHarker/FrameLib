
#include "FrameLib_Binary_Objects.h"
#include "FrameLib_MaxClass.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass_Expand<FrameLib_Subtract, kAllInputs>::makeClass("fl.minus~");
}
