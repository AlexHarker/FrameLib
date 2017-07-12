
#include "FrameLib_Window.h"
#include "FrameLib_MaxClass.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass<FrameLib_Expand<FrameLib_Window> >::makeClass(CLASS_BOX, "fl.window~");
}
