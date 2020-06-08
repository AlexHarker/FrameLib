
#include "FrameLib_FromHost.h"
#include "FrameLib_MaxClass.h"

// Max Class

class FrameLib_MaxClass_FromMax : public FrameLib_MaxClass_Expand<FrameLib_FromHost>
{
    struct FromHostProxy : public FrameLib_FromHost::Proxy, public FrameLib_MaxProxy
    {
        FromHostProxy() : FrameLib_FromHost::Proxy(true, true) {}
    };
    
public:
    
    // Class Initialisation
    
    static void classInit(t_class *c, t_symbol *nameSpace, const char *classname);
    
    // Constructor
    
    FrameLib_MaxClass_FromMax(t_object *x, t_symbol *s, long argc, t_atom *argv);
    
    // Additional handlers
    
    void intHandler(t_atom_long in);
    void floatHandler(double in);
    void list(t_symbol *s, long argc, t_atom *argv);
    void anything(t_symbol *s, long argc, t_atom *argv);
    
private:
    
    FromHostProxy *mProxy;
};

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

FrameLib_MaxClass_FromMax::FrameLib_MaxClass_FromMax(t_object *x, t_symbol *s, long argc, t_atom *argv)
    : FrameLib_MaxClass(x, s, argc, argv, new FromHostProxy())
{
    mProxy = dynamic_cast<FromHostProxy *>(mFrameLibProxy.get());
}

// Additional handlers

void FrameLib_MaxClass_FromMax::intHandler(t_atom_long in)
{
    floatHandler(static_cast<double>(in));
}

void FrameLib_MaxClass_FromMax::floatHandler(double in)
{
    mProxy->sendFromHost(0, &in, 1);
}

void FrameLib_MaxClass_FromMax::list(t_symbol *s, long argc, t_atom *argv)
{
    std::vector<double> temporary(argc);
    
    for (long i = 0; i < argc; i++)
        temporary[i] = atom_getfloat(argv++);
        
    mProxy->sendFromHost(0, temporary.data(), argc);
}

void FrameLib_MaxClass_FromMax::anything(t_symbol *s, long argc, t_atom *argv)
{
    if (argc > 1 && atom_gettype(argv) == A_SYM)
        object_error(*this, "too many arguments for string value");
    
    if (argc && atom_gettype(argv) == A_SYM)
        mProxy->sendFromHost(0, s->s_name, atom_getsym(argv)->s_name);
    else
    {
        std::vector<double> temporary(argc);
        
        for (long i = 0; i < argc; i++)
            temporary[i] = atom_getfloat(argv++);
        
        mProxy->sendFromHost(0, s->s_name, temporary.data(), argc);
    }
}

// Main

#ifndef FRAMELIB_MAX_SINGLE_OBJECT

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass_FromMax::makeClass<FrameLib_MaxClass_FromMax>("fl.frommax~");
}

#endif
