
#include "_##OBJECT_CLASS_FILE##_.h"
#include "FrameLib_MaxClass.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass_Expand<_##OBJECT_CLASS##_>::makeClass("_##MAX_CLASS_NAME##_");
}
