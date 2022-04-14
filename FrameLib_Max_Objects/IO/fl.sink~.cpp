
#include "FrameLib_Sink.h"
#include "FrameLib_MaxClass.h"


#include "FrameLib_FromHost.h"
#include "FrameLib_MaxClass.h"

// Max Class

class FrameLib_MaxClass_Sink : public FrameLib_MaxClass_Expand<FrameLib_Sink>
{
    struct SinkProxy : public FrameLib_Sink::Proxy, public FrameLib_MaxProxy
    {
        SinkProxy(t_object *x) : FrameLib_MaxProxy(x) {}
    };
    
public:
    
    // Class Initialisation
    
    static void classInit(t_class *c, t_symbol *nameSpace, const char *classname);
    
    // Constructor
    
    FrameLib_MaxClass_Sink(t_object *x, t_symbol *s, long argc, t_atom *argv);
    
    // Additional handler
    
    void clear();
    
private:
    
    SinkProxy *mSinkProxy;
};

// Wrapper Class

using WrapperType = Wrapper<FrameLib_MaxClass_Sink>;

// Clear Handler

void clearHandler(WrapperType *x)
{
    x->message(gensym("clear"),  0, nullptr);
}

// Class Initialisation

template <>
void WrapperType::classInit(t_class *c, t_symbol *nameSpace, const char *classname)
{
    WrapperType::classInitCore(c, nameSpace, classname);
    
    class_addmethod(c, (method) &clearHandler, "clear", 0);
}

// Max Class

// Class Initialisation

void FrameLib_MaxClass_Sink::classInit(t_class *c, t_symbol *nameSpace, const char *classname)
{
    FrameLib_MaxClass::classInit(c, nameSpace, classname);
    
    addMethod<FrameLib_MaxClass_Sink, &FrameLib_MaxClass_Sink::clear>(c, "clear");
}

// Constructor

FrameLib_MaxClass_Sink::FrameLib_MaxClass_Sink(t_object *x, t_symbol *s, long argc, t_atom *argv)
: FrameLib_MaxClass(x, s, argc, argv, new SinkProxy(x))
{
    mSinkProxy = static_cast<SinkProxy *>(mProxy.get());
}

// Additional handlers

void FrameLib_MaxClass_Sink::clear()
{
    mSinkProxy->clear();
}

// Main

#ifndef FRAMELIB_MAX_SINGLE_OBJECT

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass_Sink::makeClass<FrameLib_MaxClass_Sink>("fl.sink~");
}

#endif

