
#include "FrameLib_ToHost.h"
#include <FrameLib_MaxClass.h>
#include <algorithm>

// Max Class

class FrameLib_MaxClass_ToMax : public FrameLib_MaxClass<FrameLib_Expand<FrameLib_ToHost> >
{
    struct ToHostProxy : public FrameLib_ToHost::Proxy, public FrameLib_MaxProxy
    {
        ToHostProxy(FrameLib_MaxClass_ToMax *object) : mObject(object){}
        
        void sendToHost(unsigned long index, unsigned long stream, const double *values, unsigned long N)  override;
        void sendToHost(unsigned long index, unsigned long stream, const FrameLib_Parameters::Serial *serial)  override;
        
    private:
        
        unsigned long limitSize(unsigned long N) { return std::min(N, 32767UL); }
        
        FrameLib_MaxClass_ToMax *mObject;
    };
    
public:
    
    // Class Initialisation
    
    static void classInit(t_class *c, t_symbol *nameSpace, const char *classname);
    
    // Constructor
    
    FrameLib_MaxClass_ToMax(t_symbol *s, long argc, t_atom *argv);
    
    // Send to the Outlet
    
    static void toOutletExternal(FrameLib_MaxClass_ToMax *x, t_symbol *s, short ac, t_atom *av) { x->toOutlet(s, ac, av); }
    
    void toOutlet(t_symbol *s, short ac, t_atom *av);
    
private:
    
    // Data
    
    void *mOutlet;
};

// Proxy Class

void FrameLib_MaxClass_ToMax::ToHostProxy::sendToHost(unsigned long index, unsigned long stream, const double *values, unsigned long N)
{    
    if ((N = limitSize(N)))
    {
        t_atom *output = alloc<t_atom>(N);

        for (unsigned long i = 0; i < N; i++)
            atom_setfloat(output + i, values[i]);
        
        schedule_delay(mObject, (method) &FrameLib_MaxClass_ToMax::toOutletExternal, 0.0, nullptr, N, output);
        
        dealloc(output);
    }
    else
        schedule_delay(mObject, (method) &FrameLib_MaxClass_ToMax::toOutletExternal, 0.0, nullptr, 0, nullptr);
}

void FrameLib_MaxClass_ToMax::ToHostProxy::sendToHost(unsigned long index, unsigned long stream, const FrameLib_Parameters::Serial *serial)
{
    unsigned long maxSize = 0;
    
    for (auto it = serial->begin(); it != serial->end(); it++)
    {
        if (it.getType() == kVector)
        {
            unsigned long size = it.getVectorSize();
            maxSize = std::max(size, maxSize);
        }
    }
    
    maxSize = limitSize(maxSize);
    t_atom *output = alloc<t_atom>(maxSize);

    for (auto it = serial->begin(); it != serial->end(); it++)
    {
        if (it.getType() == kVector)
        {
            t_symbol *tag = gensym(it.getTag());
            unsigned long size = 0;
            const double *vector = it.getVector(&size);
            
            for (unsigned long i = 0; i < size; i++)
                atom_setfloat(output + i, vector[i]);
            
            schedule_delay(mObject, (method) &FrameLib_MaxClass_ToMax::toOutletExternal, 0.0, tag, size, output);
        }
        else
        {
            t_atom a;
            t_symbol *tag = gensym(it.getTag());
            atom_setsym(&a, gensym(it.getString()));
            
            schedule_delay(mObject, (method) &FrameLib_MaxClass_ToMax::toOutletExternal, 0.0, tag, 1, &a);
        }
    }
    
    dealloc(output);
}

// Max Class

// Class Initisation

void FrameLib_MaxClass_ToMax::classInit(t_class *c, t_symbol *nameSpace, const char *classname)
{
    FrameLib_MaxClass::classInit(c, nameSpace, classname);
}

// Constructor

FrameLib_MaxClass_ToMax::FrameLib_MaxClass_ToMax(t_symbol *s, long argc, t_atom *argv)
    : FrameLib_MaxClass(s, argc, argv, new ToHostProxy(this))
{
    mOutlet = outlet_new(this, 0L);
}

// To Outlet

void FrameLib_MaxClass_ToMax::toOutlet(t_symbol *s, short ac, t_atom *av)
{
    if (!ac)
        outlet_bang(mOutlet);
    else if (s)
        outlet_anything(mOutlet, s, ac, av);
    else
        outlet_list(mOutlet, nullptr, ac, av);
}

// Main

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass_ToMax::makeClass<FrameLib_MaxClass_ToMax>(CLASS_BOX, "fl.tomax~");
}
