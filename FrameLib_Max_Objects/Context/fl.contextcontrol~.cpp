
#include "FrameLib_MaxClass.h"
#include "../FrameLib_Exports/FrameLib_TypeAliases.h"

// A max class to communicate with the current context

class FrameLib_MaxClass_ContextControl : public MaxClass_Base
{
    using MaxClass = FrameLib_MaxClass<void>;
    
public:
    
    static void classInit(t_class *c, t_symbol *nameSpace, const char *classname)
    {
        addMethod<FrameLib_MaxClass_ContextControl, &FrameLib_MaxClass_ContextControl::multithread>(c, "multithread");
        addMethod<FrameLib_MaxClass_ContextControl, &FrameLib_MaxClass_ContextControl::assist>(c, "assist");
        
        class_addmethod(c, (method) &extTimeOut, "timeout", A_DEFFLOAT, A_DEFFLOAT, 0);
        class_addmethod(c, (method) &extCodeExport, "export", A_SYM, A_SYM, 0);
        
        CLASS_ATTR_SYM(c, "id", ATTR_FLAGS_NONE, FrameLib_MaxClass_ContextControl, mMaxContext.mName);
        CLASS_ATTR_ACCESSORS(c, "id", 0, &FrameLib_MaxClass_ContextControl::idSet);
        
        CLASS_ATTR_ATOM_LONG(c, "rt", ATTR_FLAGS_NONE, FrameLib_MaxClass_ContextControl, mMaxContext.mRealtime);
        CLASS_ATTR_ACCESSORS(c, "rt", 0, &FrameLib_MaxClass_ContextControl::rtSet);
    }
    
    FrameLib_MaxClass_ContextControl(t_object *x, t_symbol *sym, long argc, t_atom *argv)
    : mMaxContext{ true, FrameLib_MaxClass<void>::contextPatch(x, true), gensym("") }
    , mContext(mGlobal->makeContext(mMaxContext))
    , mContextPatchConfirmed(false)
    {
        attr_args_process(this, static_cast<short>(argc), argv);
    }
    
    ~FrameLib_MaxClass_ContextControl()
    {
        mGlobal->releaseContext(mContext);
    }
    
    // Attributes
    
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
    
    // Time out
    
    static void extTimeOut(FrameLib_MaxClass_ContextControl *x, double relative, double absolute)
    {
        x->timeOut(relative, absolute);
    }
    
    // Export

    static void extCodeExport(FrameLib_MaxClass_ContextControl *x, t_symbol *className, t_symbol *path)
    {
        t_atom argv[2];
        
        atom_setsym(argv + 0, className);
        atom_setsym(argv + 1, path);
        
        defer_low(x, (method) deferredCodeExport, nullptr, 2, argv);
    }

    // Assist
    
    void assist(void *b, long m, long a, char *s)
    {
        sprintf(s,"(messages) Commands In" );
    }
    
private:
    
    // Multithreading
    
    void multithread(t_atom_long on)
    {
        checkContextPatch();
        FrameLib_Context::ProcessingQueue processingQueue(mContext);
        processingQueue->setMultithreading(on);
    }
    
    // Time out

    void timeOut(double relative, double absolute)
    {
        checkContextPatch();
        FrameLib_Context::ProcessingQueue processingQueue(mContext);
        processingQueue->setTimeOuts(relative / 100.0, absolute / 1000.0);
    }
    
    // Export
    
    static void deferredCodeExport(FrameLib_MaxClass_ContextControl *x, t_symbol *s, short argc, t_atom *argv)
    {
        t_symbol *className = atom_getsym(argv + 0);
        t_symbol *path = atom_getsym(argv + 1);
        
        x->codeExport(className, path);
    }
    
    void codeExport(t_symbol *className, t_symbol *path)
    {
        char conformedPath[MAX_PATH_CHARS];
        
        checkContextPatch();
        
        // Get the first object and its underlying framelib object
        
        t_object *object = searchPatch(mMaxContext.mPatch, getAssociation(mMaxContext.mPatch));
        FrameLib_Multistream *flObject = toFLObject(object);
        
        if (!object || !flObject)
        {
            object_error(*this, "couldn't find any framelib objects in the current context");
            return;
        }
        
        objectMethod(object, FrameLib_MaxPrivate::messageResolveContext());
        
        auto replace = FrameLib_TypeAliases::makeReplaceStrings();
        
        path_nameconform(path->s_name, conformedPath, PATH_STYLE_NATIVE, PATH_TYPE_BOOT);
        ExportError error = exportGraph(flObject, conformedPath, className->s_name, &replace);
        
        if (error == ExportError::PathError)
            object_error(*this, "couldn't write to or find specified path");
        else if (error == ExportError::WriteError)
            object_error(*this, "couldn't write file");
    }
    
    // Context
    
    void checkContextPatch()
    {
        if (mContextPatchConfirmed)
            return;

        t_object *patch = FrameLib_MaxClass<void>::contextPatch(*this, false);
        
        if (patch && patch != mMaxContext.mPatch)
        {
            mMaxContext.mPatch = patch;
            updateContext();
        }
        
        mContextPatchConfirmed = true;
    }
    
    void updateContext()
    {
        mGlobal->releaseContext(mContext);
        mContext = mGlobal->makeContext(mMaxContext);
    }
    
    // Convert an object to an FLObject
    
    FrameLib_Multistream *toFLObject(t_object *x)
    {
        return FrameLib_MaxPrivate::toFrameLibObject(x);
    }
    
    // Search a patch
    
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
    
    // Members
        
    FrameLib_MaxGlobals::ManagedPointer mGlobal;
    FrameLib_MaxContext mMaxContext;
    FrameLib_Context mContext;
    
    bool mContextPatchConfirmed;
};

// Main

#ifndef FRAMELIB_MAX_SINGLE_OBJECT

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass_ContextControl::makeClass<FrameLib_MaxClass_ContextControl>(CLASS_BOX, "fl.contextcontrol~");
}

#endif
