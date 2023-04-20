
#include "_##CLASS##_.h"
#include "FrameLib_MaxClass.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass_Expand<_##CLASS##_>::makeClass("_##CLASSNAME##_");
}
