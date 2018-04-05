
#include "FrameLib_ToHost.h"
#include <FrameLib_MaxClass.h>
#include <algorithm>

// Max Class

class FrameLib_MaxClass_ToMax : public FrameLib_MaxClass<FrameLib_Expand<FrameLib_ToHost> >
{
    struct ToHostProxy : public FrameLib_ToHost::Proxy, public FrameLib_MaxProxy
    {
        ToHostProxy(FrameLib_MaxClass_ToMax *object) : mObject(object) {}
        
        virtual void output(const double *values, unsigned long N);
        virtual void output(const FrameLib_Parameters::Serial *serial);
        
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
