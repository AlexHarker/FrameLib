
#include "FrameLib_SallenAndKey.h"
#include "FrameLib_MaxClass.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass<FrameLib_Expand<FrameLib_SallenAndKey> >::makeClass(CLASS_BOX, "fl.sallenkey~");
}
