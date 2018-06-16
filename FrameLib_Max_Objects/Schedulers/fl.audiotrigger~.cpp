
#include "FrameLib_AudioTrigger.h"
#include "FrameLib_MaxClass.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass_Expand<FrameLib_AudioTrigger>::makeClass(CLASS_BOX, "fl.audiotrigger~");
}
