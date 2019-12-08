
#include <FrameLib_MaxClass.h>

// A max class to communicate with the current context

class FrameLib_MaxClass_Context : public MaxClass_Base
{

public:
    
    static void classInit(t_class *c, t_symbol *nameSpace, const char *classname)
    {
        addMethod<FrameLib_MaxClass_Context, &FrameLib_MaxClass_Context::multithread>(c, "multithread");
        addMethod<FrameLib_MaxClass_Context, &FrameLib_MaxClass_Context::assist>(c, "assist");
        
        CLASS_ATTR_SYM(c, "id", ATTR_FLAGS_NONE, FrameLib_MaxClass_Context, mMaxContext.mName);
        CLASS_ATTR_ACCESSORS(c, "id", 0, &FrameLib_MaxClass_Context::idSet);
        
        CLASS_ATTR_ATOM_LONG(c, "rt", ATTR_FLAGS_NONE, FrameLib_MaxClass_Context, mMaxContext.mRealtime);
        CLASS_ATTR_ACCESSORS(c, "rt", 0, &FrameLib_MaxClass_Context::rtSet);
 }
    
    FrameLib_MaxClass_Context(t_symbol *sym, long argc, t_atom *argv)
    : mMaxContext{ false, FrameLib_MaxClass<void>::contextPatcher(gensym("#P")->s_thing), gensym("")}
    , mContext(mGlobal->makeContext(mMaxContext))
    {
        attr_args_process(this, static_cast<short>(argc), argv);
    }
    
    ~FrameLib_MaxClass_Context()
    {
        mGlobal->releaseContext(mContext);
    }
    
    void updateContext()
    {
        mGlobal->releaseContext(mContext);
        mContext = mGlobal->makeContext(mMaxContext);
    }
    
    // id attribute
    
    static t_max_err idSet(FrameLib_MaxClass_Context *x, t_object *attr, long argc, t_atom *argv)
    {
        x->mMaxContext.mName = argv ? atom_getsym(argv) : gensym("");
        x->updateContext();
        
        return MAX_ERR_NONE;
    }
    
    // rt attribute
    
    static t_max_err rtSet(FrameLib_MaxClass_Context *x, t_object *attr, long argc, t_atom *argv)
    {
        x->mMaxContext.mRealtime = argv ? atom_getlong(argv) : 0;
        x->updateContext();
        
        return MAX_ERR_NONE;
    }
    
    // Multithreading
    
    void multithread(t_atom_long on)
    {
        FrameLib_Context::ProcessingQueue processingQueue(mContext);
        processingQueue->setMultithreading(on);
    }
    
    // Assist
    
    void assist(void *b, long m, long a, char *s)
    {
        sprintf(s,"(messages) Commands In" );
    }
    
private:
    
    // Members
        
    FrameLib_MaxGlobals::ManagedPointer mGlobal;
    FrameLib_MaxContext mMaxContext;
    FrameLib_Context mContext;
};

// Main

#ifndef FRAMELIB_MAX_SINGLE_OBJECT

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass_Context::makeClass<FrameLib_MaxClass_Context>(CLASS_BOX, "fl.context~");
}

#endif
