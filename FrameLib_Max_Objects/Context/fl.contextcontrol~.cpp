
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
        
        CLASS_ATTR_SYM(c, "id", ATTR_FLAGS_NONE, FrameLib_MaxClass_ContextControl, mMaxContext.mName);
        CLASS_ATTR_ACCESSORS(c, "id", 0, &FrameLib_MaxClass_ContextControl::idSet);
        
        CLASS_ATTR_ATOM_LONG(c, "rt", ATTR_FLAGS_NONE, FrameLib_MaxClass_ContextControl, mMaxContext.mRealtime);
        CLASS_ATTR_ACCESSORS(c, "rt", 0, &FrameLib_MaxClass_ContextControl::rtSet);
    }
    
    FrameLib_MaxClass_ContextControl(t_object *x, t_symbol *sym, long argc, t_atom *argv)
    : mMaxContext{ true, FrameLib_MaxClass<void>::contextPatcher(gensym("#P")->s_thing), gensym("") }
    , mContext(mGlobal->makeContext(mMaxContext))
    {
        attr_args_process(this, static_cast<short>(argc), argv);
    }
    
    ~FrameLib_MaxClass_ContextControl()
    {
        mGlobal->releaseContext(mContext);
    }
    
    // Attributes
    
    void updateContext()
    {
        mGlobal->releaseContext(mContext);
        mContext = mGlobal->makeContext(mMaxContext);
    }
    
    // id attribute
    
    static t_max_err idSet(FrameLib_MaxClass_ContextControl *x, t_object *attr, long argc, t_atom *argv)
    {
        x->mMaxContext.mName = argv ? atom_getsym(argv) : gensym("");
        x->updateContext();
        
        return MAX_ERR_NONE;
    }
    
    // rt attribute
    
    static t_max_err rtSet(FrameLib_MaxClass_ContextControl *x, t_object *attr, long argc, t_atom *argv)
    {
        x->mMaxContext.mRealtime = argv ? (atom_getlong(argv) ? 1 : 0) : 0;
        x->updateContext();
        
        return MAX_ERR_NONE;
    }
    
    // Convert an object to an FLObject
    
    FrameLib_Multistream *toFLObject(t_object *x)
    {
        return FrameLib_MaxPrivate::toFrameLibObject(x);
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
        
        if (assoc != contextAssoc && !objectMethod(assoc, FrameLib_MaxPrivate::messageIsWrapper()))
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
        FrameLib_Context::ProcessingQueue processingQueue(x->mContext);

        processingQueue->setTimeOuts(relative / 100.0, absolute / 1000.0);
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
        
        t_object *object = searchPatch(mMaxContext.mPatch, getAssociation(mMaxContext.mPatch));
        FrameLib_Multistream *flObject = toFLObject(object);
        
        if (!object || !flObject)
        {
            object_error(*this, "couldn't find any framelib objects in the current context");
            return;
        }
        
        objectMethod(object, FrameLib_MaxPrivate::messageResolveContext());
        
        path_nameconform(path->s_name, conformedPath, PATH_STYLE_NATIVE, PATH_TYPE_BOOT);
        ExportError error = exportGraph(flObject, conformedPath, className->s_name);
        
        if (error == ExportError::PathError)
            object_error(*this, "couldn't write to or find specified path");
        else if (error == ExportError::WriteError)
            object_error(*this, "couldn't write file");
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
    FrameLib_MaxClass_ContextControl::makeClass<FrameLib_MaxClass_ContextControl>(CLASS_BOX, "fl.contextcontrol~");
}

#endif
