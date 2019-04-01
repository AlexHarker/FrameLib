
#include "FrameLib_Scramble.h"
#include "FrameLib_MaxClass.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass_Expand<FrameLib_Scramble>::makeClass(CLASS_BOX, "fl.scramble~");
}
