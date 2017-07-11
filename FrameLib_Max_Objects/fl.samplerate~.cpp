
#include "FrameLib_SampleRate.h"
#include "Framelib_Max.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxObj<FrameLib_Expand<FrameLib_SampleRate> >::makeClass(CLASS_BOX, "fl.samplerate~");
}