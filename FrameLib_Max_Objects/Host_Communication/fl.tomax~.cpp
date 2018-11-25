
#include "FrameLib_ToHost.h"
#include <FrameLib_MaxClass.h>
#include <algorithm>

// Max Class

class FrameLib_MaxClass_ToMax : public FrameLib_MaxClass_Expand<FrameLib_ToHost>
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
    
    std::vector<void *> mOutlets;
};

// Proxy Class

void FrameLib_MaxClass_ToMax::ToHostProxy::sendToHost(unsigned long index, unsigned long stream, const double *values, unsigned long N)
{    
    N = limitSize(N + 1);
    t_atom *output = alloc<t_atom>(N);
    
    if (output)
    {
        atom_setlong(output, stream);
        
        for (unsigned long i = 1; i < N; i++)
            atom_setfloat(output + i, values[i - 1]);
        
        schedule_delay(mObject, (method) &FrameLib_MaxClass_ToMax::toOutletExternal, 0.0, nullptr, N, output);
        
        dealloc(output);
    }
}

void FrameLib_MaxClass_ToMax::ToHostProxy::sendToHost(unsigned long index, unsigned long stream, const FrameLib_Parameters::Serial *serial)
{
    // Determine maximum required size
    
    unsigned long maxSize = 0;
    
    for (auto it = serial->begin(); it != serial->end(); it++)
    {
        if (it.getType() == kVector)
        {
            unsigned long size = it.getVectorSize();
            maxSize = std::max(size, maxSize);
        }
    }
    
    maxSize = std::max(limitSize(maxSize + 1), 2UL);
    t_atom *output = alloc<t_atom>(maxSize);

    if (!output)
        return;
    
    // Store stream
    
    atom_setlong(output + 0, stream);

    // Iterate over tags
    
    for (auto it = serial->begin(); it != serial->end(); it++)
    {
        t_symbol *tag = gensym(it.getTag());
        unsigned long size = 0;
        
        if (it.getType() == kVector)
        {
            const double *vector = it.getVector(&size);
            size = size >= maxSize ? maxSize : size + 1;
            
            for (unsigned long i = 1; i < size; i++)
                atom_setfloat(output + i, vector[i - 1]);
        }
        else
        {
            size = 2;
            atom_setsym(output + 1, gensym(it.getString()));
        }
        
        schedule_delay(mObject, (method) &FrameLib_MaxClass_ToMax::toOutletExternal, 0.0, tag, size, output);
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
    unsigned long nStreams = getNumStreams();
    
    mOutlets.resize(nStreams);
    
    for (unsigned long i = nStreams; i > 0; i--)
        mOutlets[i - 1] = outlet_new(this, 0L);
}

// To Outlet

void FrameLib_MaxClass_ToMax::toOutlet(t_symbol *s, short ac, t_atom *av)
{
    long idx = atom_getlong(av) % mOutlets.size();
    ac--;
    
    if (!ac)
        outlet_bang(mOutlets[idx]);
    else if (s)
        outlet_anything(mOutlets[idx], s, ac, av + 1);
    else
        outlet_list(mOutlets[idx], nullptr, ac, av + 1);
}

// Assist

template<>
void FrameLib_MaxClass_Expand<FrameLib_ToHost>::assist(void *b, long m, long a, char *s)
{
    if (m == ASSIST_OUTLET)
        sprintf(s,"(message) Message Outlet %ld", a + 1);
    else
        sprintf(s,"(frame) %s", mObject->inputInfo(a).c_str());
}


// Main

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass_ToMax::makeClass<FrameLib_MaxClass_ToMax>(CLASS_BOX, "fl.tomax~");
}
