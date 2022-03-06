
#include "FrameLib_Sink.h"
#include "FrameLib_MaxClass.h"


#include "FrameLib_FromHost.h"
#include "FrameLib_MaxClass.h"

// Max Class

class FrameLib_MaxClass_Sink : public FrameLib_MaxClass_Expand<FrameLib_Sink>
{
    struct SinkProxy : public FrameLib_Sink::Proxy, public FrameLib_MaxProxy
    {
    };
    
public:
    
    // Class Initialisation
    
    static void classInit(t_class *c, t_symbol *nameSpace, const char *classname);
    
    // Constructor
    
    FrameLib_MaxClass_Sink(t_object *x, t_symbol *s, long argc, t_atom *argv);
    
    // Additional handler
    
    void clear();
    
private:
    
    SinkProxy *mProxy;
};

// Max Class

// Class Initialisation

void FrameLib_MaxClass_Sink::classInit(t_class *c, t_symbol *nameSpace, const char *classname)
{
    // FIX - need to pass through from the wrapper
    
    FrameLib_MaxClass::classInit(c, nameSpace, classname);
    
    addMethod<FrameLib_MaxClass_Sink, &FrameLib_MaxClass_Sink::clear>(c, "clear");
}

// Constructor

FrameLib_MaxClass_Sink::FrameLib_MaxClass_Sink(t_object *x, t_symbol *s, long argc, t_atom *argv)
: FrameLib_MaxClass(x, s, argc, argv, new SinkProxy())
{
    mProxy = dynamic_cast<SinkProxy *>(mFrameLibProxy.get());
}

// Additional handlers

void FrameLib_MaxClass_Sink::clear()
{
    mProxy->clear();
}

// Main

#ifndef FRAMELIB_MAX_SINGLE_OBJECT

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass_Sink::makeClass<FrameLib_MaxClass_Sink>("fl.sink~");
}

#endif

