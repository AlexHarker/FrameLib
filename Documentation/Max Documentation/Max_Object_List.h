
#include "FrameLib_TypeList.h"

using FrameLib_DSPList = FrameLib_Typelist<

FrameLib_Expand<FrameLib_0dfSVF>
//FrameLib_Expand<FrameLib_OnePole>,
//FrameLib_Expand<FrameLib_OnePoleZero>,
//FrameLib_Expand<FrameLib_Resonant>,
//FrameLib_Expand<FrameLib_SallenAndKey>

>;

template<>
const char* FrameLib_ObjectName<FrameLib_Expand<FrameLib_0dfSVF>>::name() { return "fl.0dfSVF"; }
