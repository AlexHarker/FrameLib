
#include "fl.frommax~.h"

// Max Class

// Class Initisation

void FrameLib_MaxClass_FromMax::classInit(t_class *c, t_symbol *nameSpace, const char *classname)
{
    FrameLib_MaxClass::classInit(c, nameSpace, classname);
    
    addMethod<FrameLib_MaxClass_FromMax, &FrameLib_MaxClass_FromMax::intHandler>(c, "int");
    addMethod<FrameLib_MaxClass_FromMax, &FrameLib_MaxClass_FromMax::floatHandler>(c, "float");
    addMethod<FrameLib_MaxClass_FromMax, &FrameLib_MaxClass_FromMax::list>(c, "list");
    addMethod<FrameLib_MaxClass_FromMax, &FrameLib_MaxClass_FromMax::anything>(c, "anything");
}

// Constructor

FrameLib_MaxClass_FromMax::FrameLib_MaxClass_FromMax(t_symbol *s, long argc, t_atom *argv)
    : FrameLib_MaxClass(s, argc, argv, new FromHostProxy())
{
    mProxy = dynamic_cast<FromHostProxy *>(mFrameLibProxy);
}

// Additional handlers

void FrameLib_MaxClass_FromMax::intHandler(t_atom_long in)
{
    floatHandler(in);
}

void FrameLib_MaxClass_FromMax::floatHandler(double in)
{
    mProxy->write(&in, 1);
}

void FrameLib_MaxClass_FromMax::list(t_symbol *s, long argc, t_atom *argv)
{
    std::vector<double> temporary(argc);
    
    for (unsigned long i = 0; i < argc; i++)
        temporary[i] = atom_getfloat(argv++);
        
    mProxy->write(&temporary[0], argc);
}

void FrameLib_MaxClass_FromMax::anything(t_symbol *s, long argc, t_atom *argv)
{
    if (argc > 1 && atom_gettype(argv) == A_SYM)
        object_error(*this, "too many arguments for string value");
    
    if (atom_gettype(argv) == A_SYM)
        mProxy->write(s->s_name, argc ? atom_getsym(argv)->s_name : "");
    else
    {
        std::vector<double> temporary(argc);
        
        for (unsigned long i = 0; i < argc; i++)
            temporary[i] = atom_getfloat(argv++);
        
        mProxy->write(s->s_name, &temporary[0], argc);
    }
}

// Main

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass_FromMax::makeClass<FrameLib_MaxClass_FromMax>(CLASS_BOX, "fl.frommax~");
}
