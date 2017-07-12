
#include "FrameLib_SampleRate.h"
#include "FrameLib_MaxClass.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass<FrameLib_Expand<FrameLib_SampleRate> >::makeClass(CLASS_BOX, "fl.samplerate~");
}