
#include "FrameLib_Channel.h"
#include "FrameLib_MaxClass.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass_Expand<FrameLib_Channel>::makeClass(CLASS_BOX, "fl.channel~");
}
