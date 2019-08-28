
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
    
    FrameLib_MaxClass_Context(t_symbol *sym, long ac, t_atom *av) : mPatch(gensym("#P")->s_thing), mContext(mGlobal->getGlobal(), FrameLib_MaxClass<void>::contextPatcher(mPatch)), mProcessingQueue(mContext)
    {}
    
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
    
    t_object *mPatch;
    
    FrameLib_MaxGlobals::ManagedPointer mGlobal;
    FrameLib_Context mContext;
    FrameLib_Context::ProcessingQueue mProcessingQueue;
};

// Main

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass_Context::makeClass<FrameLib_MaxClass_Context>(CLASS_BOX, "fl.context~");
}