
#include "FrameLib_SallenAndKey.h"
#include "Framelib_Max.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxObj<FrameLib_Expand<FrameLib_SallenAndKey> >::makeClass(CLASS_BOX, "fl.sallenkey~");
}
