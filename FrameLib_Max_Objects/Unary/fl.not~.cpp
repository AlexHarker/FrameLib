
#include "FrameLib_Unary_Objects.h"
#include "FrameLib_MaxClass.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass_Expand<FrameLib_LogicalNot, true>::makeClass(CLASS_BOX, "fl.not~");
}
