
#include <FrameLib_MaxClass.h>

// A max class to communicate with the current context

class FrameLib_MaxClass_Context : public MaxClass_Base
{

public:
    
    static void classInit(t_class *c, t_symbol *nameSpace, const char *classname)
    {
        addMethod<FrameLib_MaxClass_Context, &FrameLib_MaxClass_Context::multithread>(c, "multithread");
        addMethod<FrameLib_MaxClass_Context, &FrameLib_MaxClass_Context::assist>(c, "assist");
    }
    
    FrameLib_MaxClass_Context(t_object *x, t_symbol *sym, long ac, t_atom *av)
    : mContext(mGlobal->makeContext(FrameLib_MaxClass<void>::parseContext(FrameLib_MaxClass<void>::contextPatcher(gensym("#P")->s_thing), ac, av)))
    , mProcessingQueue(mContext)
    {}
    
    ~FrameLib_MaxClass_Context()
    {
        mGlobal->releaseContext(mContext);
    }
    
    // Multithreading
    
    void multithread(t_atom_long on)
    {
        mProcessingQueue->setMultithreading(on);
    }
    
    // Assist
    
    void assist(void *b, long m, long a, char *s)
    {
        sprintf(s,"(messages) Commands In" );
    }
    
private:
    
    // Members
        
    FrameLib_MaxGlobals::ManagedPointer mGlobal;
    FrameLib_Context mContext;
    FrameLib_Context::ProcessingQueue mProcessingQueue;
};

// Main

#ifndef FRAMELIB_MAX_SINGLE_OBJECT

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass_Context::makeClass<FrameLib_MaxClass_Context>(CLASS_BOX, "fl.context~");
}

#endif
