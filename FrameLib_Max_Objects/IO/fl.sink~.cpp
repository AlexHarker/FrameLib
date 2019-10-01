
#include "FrameLib_Sink.h"
#include "FrameLib_MaxClass.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass_Expand<FrameLib_Sink>::makeClass("fl.sink~");
}
