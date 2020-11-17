
#include "FrameLib_BiquadCoeff.h"
#include "FrameLib_MaxClass.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass_Expand<FrameLib_BiquadCoeff>::makeClass("fl.biquadcoeff~");
}
