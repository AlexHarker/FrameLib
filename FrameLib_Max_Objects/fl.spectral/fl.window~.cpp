
#include "FrameLib_Window.h"
#include "Framelib_Max.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxObj<FrameLib_Expand<FrameLib_Window> >::makeClass(CLASS_BOX, "fl.window~");
}
