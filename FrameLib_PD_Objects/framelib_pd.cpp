
#include "FrameLib_Interval.h"
#include "FrameLib_Source.h"
#include "FrameLib_Sink.h"
#include "FrameLib_Ramp.h"
#include "FrameLib_Random.h"
#include "FrameLib_Window.h"
#include "FrameLib_SampleRate.h"
#include "FrameLib_Unary_Objects.h"
#include "FrameLib_Binary_Objects.h"
#include "FrameLib_PDClass.h"
#include "fl.read_pd~.h"

extern "C" void framelib_pd_setup(void)
{
    FrameLib_PDClass<FrameLib_Expand <FrameLib_Interval> >::makeClass("fl.interval~");
    FrameLib_PDClass<FrameLib_Expand <FrameLib_Sink> >::makeClass("fl.sink~");
    FrameLib_PDClass<FrameLib_Expand <FrameLib_Source> >::makeClass("fl.source~");
    
    FrameLib_PDClass<FrameLib_Expand <FrameLib_Plus> >::makeClass("fl.+~");
    FrameLib_PDClass<FrameLib_Expand <FrameLib_Minus> >::makeClass("fl.-~");
    FrameLib_PDClass<FrameLib_Expand <FrameLib_Multiply> >::makeClass("fl.*~");
    FrameLib_PDClass<FrameLib_Expand <FrameLib_Divide> >::makeClass("fl./~");
    
    FrameLib_PDClass_Expand<FrameLib_PDRead>::makeClass("fl.read~");
    FrameLib_PDClass<FrameLib_Expand <FrameLib_Ramp> >::makeClass("fl.ramp~");
    FrameLib_PDClass<FrameLib_Expand <FrameLib_Random> >::makeClass("fl.random~");
    FrameLib_PDClass<FrameLib_Expand <FrameLib_SampleRate> >::makeClass("fl.samplrate~");
    FrameLib_PDClass<FrameLib_Expand <FrameLib_Window> >::makeClass("fl.window~");
}
