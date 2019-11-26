
#include "FrameLib_Complex_Binary_Objects.h"
#include "FrameLib_MaxClass.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass_Expand<FrameLib_Complex_Subtract, kAllInputs>::makeClass("fl.complex.minus~");
}
