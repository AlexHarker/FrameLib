
#include "FrameLib_ToHost.h"
#include "FrameLib_MaxClass.h"
#include <algorithm>

// Max Class

class FrameLib_MaxClass_ToMax : public FrameLib_MaxClass_Expand<FrameLib_ToHost>
{
    struct ToHostProxy : public FrameLib_ToHost::Proxy, public FrameLib_MaxProxy
    {
        ToHostProxy(FrameLib_MaxClass_ToMax *object) : mObject(object) {}
        
        void sendToHost(unsigned long index, unsigned long stream, const double *values, unsigned long N, FrameLib_TimeFormat time)  override;
        void sendToHost(unsigned long index, unsigned long stream, const FrameLib_Parameters::Serial *serial, FrameLib_TimeFormat time)  override;
        
        void sendMessage(unsigned long stream, t_symbol *s, short ac, t_atom *av) override;
        
    private:
        
        FrameLib_MaxClass_ToMax *mObject;
    };
    
public:
    
    // Class Initialisation
    
    static void classInit(t_class *c, t_symbol *nameSpace, const char *classname);
    
    // Constructor
    
    FrameLib_MaxClass_ToMax(t_object *x, t_symbol *s, long argc, t_atom *argv);
    
private:
    
    // Data
    
    ToHostProxy *mProxy;
    std::vector<void *> mOutlets;
};

// Proxy Class

void FrameLib_MaxClass_ToMax::ToHostProxy::sendToHost(unsigned long index, unsigned long stream, const double *values, unsigned long N, FrameLib_TimeFormat time)
{
    mObject->getHandler()->add(MessageInfo(this, time, reinterpret_cast<t_ptr_uint>(this), stream), values, N);
}

void FrameLib_MaxClass_ToMax::ToHostProxy::sendToHost(unsigned long index, unsigned long stream, const FrameLib_Parameters::Serial *serial, FrameLib_TimeFormat time)
{
    mObject->getHandler()->add(MessageInfo(this, time, reinterpret_cast<t_ptr_uint>(this), stream), serial);
}

void FrameLib_MaxClass_ToMax::ToHostProxy::sendMessage(unsigned long stream, t_symbol *s, short ac, t_atom *av)
{
    auto& outlets = mObject->mOutlets;
    unsigned long idx = stream % outlets.size();
    
    if (s)
        outlet_anything(outlets[idx], s, ac, av);
    else if (!ac)
        outlet_bang(outlets[idx]);
    else
        outlet_list(outlets[idx], nullptr, ac, av);
}

// Max Class

// Class Initialisation

void FrameLib_MaxClass_ToMax::classInit(t_class *c, t_symbol *nameSpace, const char *classname)
{
    FrameLib_MaxClass::classInit(c, nameSpace, classname);
}

// Constructor

FrameLib_MaxClass_ToMax::FrameLib_MaxClass_ToMax(t_object *x, t_symbol *s, long argc, t_atom *argv)
: FrameLib_MaxClass(x, s, argc, argv, new ToHostProxy(this))
{
    unsigned long nStreams = getSpecifiedStreams();
    
    mOutlets.resize(nStreams);
    
    for (unsigned long i = nStreams; i > 0; i--)
        mOutlets[i - 1] = outlet_new(this, 0L);
    
    mProxy = static_cast<ToHostProxy *>(mFrameLibProxy.get());
}

// Assist

template<>
void FrameLib_MaxClass_Expand<FrameLib_ToHost>::assist(void *b, long m, long a, char *s)
{
    if (m == ASSIST_OUTLET)
        sprintf(s,"(message) Message Outlet [%ld]", a + 1);
    else
        sprintf(s,"(frame) %s", mObject->inputInfo(a).c_str());
}

// Main

#ifndef FRAMELIB_MAX_SINGLE_OBJECT

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass_ToMax::makeClass<FrameLib_MaxClass_ToMax>("fl.tomax~");
}

#endif
