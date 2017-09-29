
#include "fl.tomax~.h"

// Underlying FrameLib Class

// Constructor

FrameLib_ToMax::FrameLib_ToMax(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, owner, NULL, 1, 0)
{
    mParameters.set(serialisedParameters);
    
    mMaxOwner = ((FrameLib_MaxClass_ToMax *) owner);
}

// Info

std::string FrameLib_ToMax::objectInfo(bool verbose)
{
    return formatInfo("Turn frames into max messages.",
                   "Turn frames into max messages.", verbose);
}

std::string FrameLib_ToMax::inputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Input Frames - converted to max message", "Input Frames", verbose);
}

std::string FrameLib_ToMax::outputInfo(unsigned long idx, bool verbose)
{
    return "Max Message Output";
}

// Process

void FrameLib_ToMax::process()
{
    unsigned long sizeIn;
    double *input = getInput(0, &sizeIn);

    if (sizeIn > 32767)
        sizeIn = 32767;
    
    t_atom *output = alloc<t_atom>(sizeIn);

    // FIX - support parameter frames!
    
    if (sizeIn && output)
    {
        for (unsigned long i = 0; i < sizeIn; i++)
            atom_setfloat(output + i, input[i]);
        
        schedule_delay(mMaxOwner, (method) &FrameLib_MaxClass_ToMax::toOutletExternal, 0.0, NULL, sizeIn, output);
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

FrameLib_MaxClass_ToMax::FrameLib_MaxClass_ToMax(t_symbol *s, long argc, t_atom *argv) : FrameLib_MaxClass(s, argc, argv)
{
    mOutlet = listout(this);
}

// To Outlet

void FrameLib_MaxClass_ToMax::toOutlet(t_symbol *s, short ac, t_atom *av)
{
    outlet_list(mOutlet, s, ac, av);
}

// Main

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass_ToMax::makeClass<FrameLib_MaxClass_ToMax>(CLASS_BOX, "fl.tomax~");
}
