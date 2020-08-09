
#include "FrameLib_MaxClass.h"

// A max class to communicate with the current context

class FrameLib_MaxClass_ContextControl : public MaxClass_Base
{
    using MaxClass = FrameLib_MaxClass<void>;
    
public:
    
    static void classInit(t_class *c, t_symbol *nameSpace, const char *classname)
    {
        addMethod<FrameLib_MaxClass_ContextControl, &FrameLib_MaxClass_ContextControl::multithread>(c, "multithread");
        addMethod<FrameLib_MaxClass_ContextControl, &FrameLib_MaxClass_ContextControl::assist>(c, "assist");
        
        class_addmethod(c, (method) &timeOut, "timeout", A_DEFFLOAT, A_DEFFLOAT, 0);
        class_addmethod(c, (method) &extCodeExport, "export", A_SYM, A_SYM, 0);
    }
    
    FrameLib_MaxClass_ContextControl(t_object *x, t_symbol *sym, long ac, t_atom *av)
    : mContextPatch(MaxClass::contextPatcher(gensym("#P")->s_thing))
    , mContext(mGlobal->makeContext(MaxClass::parseContext(true, mContextPatch, ac, av)))
    , mProcessingQueue(mContext)
    {}
    
    ~FrameLib_MaxClass_ContextControl()
    {
        mGlobal->releaseContext(mContext);
    }
    
    FrameLib_Multistream *toFLObject(t_object *object)
    {
        return objectMethod<FrameLib_Multistream *>(object, gensym("__fl.get_framelib_object"));
    }
    
    // Get the association of a patch
    
    static t_object *getAssociation(t_object *patch)
    {
        t_object *assoc = 0;
        objectMethod(patch, gensym("getassoc"), &assoc);
        return assoc;
    }
    
    t_object *searchPatch(t_patcher *p, t_object *contextAssoc)
    {
        t_object *object = nullptr;
        t_object *assoc = getAssociation(p);
        
        // Avoid recursion into a poly / pfft / etc. - If the subpatcher is a wrapper we do need to deal with it
        
        if (assoc != contextAssoc && !objectMethod(assoc, gensym("__fl.wrapper_is_wrapper")))
            return nullptr;
        
        // Search for subpatchers, or framelib objects
        
        for (t_box *b = jpatcher_get_firstobject(p); b; b = jbox_get_nextobject(b))
        {
            long index = 0;
            
            while (b && (p = (t_patcher *)object_subpatcher(jbox_get_object(b), &index, this)))
                if ((object = searchPatch(p, contextAssoc)))
                    return object;
            
            object = jbox_get_object(b);
            FrameLib_Multistream *flObject = toFLObject(object);
            
            if (flObject && flObject->getContext() == mContext)
                return object;
        }
        
        return nullptr;
    }
    
    // Time out
    
    static void timeOut(FrameLib_MaxClass_ContextControl *x, double relative, double absolute)
    {
        x->mProcessingQueue->setTimeOuts(relative / 100.0, absolute / 1000.0);
    }
    
    // Export

    static void extCodeExport(FrameLib_MaxClass_ContextControl *x, t_symbol *className, t_symbol *path)
    {
        x->codeExport(className, path);
    }

    void codeExport(t_symbol *className, t_symbol *path)
    {
        char conformedPath[MAX_PATH_CHARS];
        
        // Get the first object and it's underlying framelib object
        
        t_object *object = searchPatch(mContextPatch, getAssociation(mContextPatch));
        FrameLib_Multistream *flObject = toFLObject(object);
        
        if (!object || !flObject)
        {
            object_error(*this, "couldn't find any framelib objects in the current context");
            return;
        }
        
        objectMethod(object, gensym("__fl.resolve_context"));
        flObject = objectMethod<FrameLib_Multistream *>(object, gensym("__fl.get_framelib_object"));
        
        path_nameconform(path->s_name, conformedPath, PATH_STYLE_NATIVE, PATH_TYPE_BOOT);
        ExportError error = exportGraph(flObject, conformedPath, className->s_name);
        
        if (error == kExportPathError)
            object_error(*this, "couldn't write to or find specified path");
        else if (error == kExportWriteError)
            object_error(*this, "couldn't write file");
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
        
    t_object *mContextPatch;
    FrameLib_MaxGlobals::ManagedPointer mGlobal;
    FrameLib_Context mContext;
    FrameLib_Context::ProcessingQueue mProcessingQueue;
};

// Main

#ifndef FRAMELIB_MAX_SINGLE_OBJECT

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass_ContextControl::makeClass<FrameLib_MaxClass_ContextControl>(CLASS_BOX, "fl.contextcontrol~");
}

#endif
