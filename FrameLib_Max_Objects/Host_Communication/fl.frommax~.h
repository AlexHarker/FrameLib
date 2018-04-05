
#include "FrameLib_FromHost.h"
#include <FrameLib_MaxClass.h>

// Max Class

class FrameLib_MaxClass_FromMax : public FrameLib_MaxClass<FrameLib_Expand<FrameLib_FromHost> >
{
    struct FromHostProxy : public FrameLib_FromHost::Proxy, public FrameLib_MaxProxy {};
    
public:
    
    // Class Initialisation
    
    static void classInit(t_class *c, t_symbol *nameSpace, const char *classname);
    
    // Constructor
    
    FrameLib_MaxClass_FromMax(t_symbol *s, long argc, t_atom *argv);
    
    // Additional handlers
    
    void intHandler(t_atom_long in);
    void floatHandler(double in);
    void list(t_symbol *s, long argc, t_atom *argv);
    void anything(t_symbol *s, long argc, t_atom *argv);
    
private:
    
    FromHostProxy *mProxy;
};
