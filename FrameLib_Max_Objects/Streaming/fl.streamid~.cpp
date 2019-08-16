
#include "FrameLib_StreamID.h"
#include "FrameLib_MaxClass.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass_Expand<FrameLib_StreamID>::makeClass(CLASS_BOX, "fl.streamid~");
}
