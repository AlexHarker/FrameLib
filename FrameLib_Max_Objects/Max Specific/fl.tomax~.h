
#include "FrameLib_DSP.h"
#include <FrameLib_MaxClass.h>

// Underlying FrameLib Class

class FrameLib_ToMax : public FrameLib_Processor
{
    
public:
    
    // Constructor
    
    FrameLib_ToMax(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
        
    // Info
    
    std::string objectInfo(bool verbose);
    std::string inputInfo(unsigned long idx, bool verbose);
    std::string outputInfo(unsigned long idx, bool verbose);

private:
    
    void process();
    
// Data (cache the outlet)
    
    void *mMaxOwner;
};

// Max Class

class FrameLib_MaxClass_ToMax : public FrameLib_MaxClass<FrameLib_Expand<FrameLib_ToMax> >
{

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
