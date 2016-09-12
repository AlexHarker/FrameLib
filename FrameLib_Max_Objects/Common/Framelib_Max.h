
#include "Max_Base.h"

#include "FrameLib_Multichannel.h"
#include "FrameLib_DSP.h"
#include "FrameLib_Globals.h"

#include <vector>

// FIX - sort out formatting style/ naming?
// FIX - improve reporting of extra connections + look into feedback detection...
// FIX - think about adding assist helpers for this later...
// FIX - threadsafety??

// FIX - is this right?
t_class *objectClass = NULL;
t_class *wrapperClass = NULL;
t_class *mutatorClass = NULL;

//////////////////////////////////////////////////////////////////////////
///////////////////////////// Sync Check Class ///////////////////////////
//////////////////////////////////////////////////////////////////////////

typedef struct _syncinfo
{
    void *object;
    long time;
    
} t_syncinfo;

t_syncinfo sync_info;

void sync_set_info(void *object, long time)
{
    sync_info.object = object;
    sync_info.time = time;
    gensym("__FrameLib__SYNC__")->s_thing = (t_object *) (object ? &sync_info : NULL);
}

t_syncinfo *sync_get_info()
{
    return (t_syncinfo *) gensym("__FrameLib__SYNC__")->s_thing;
}

struct SyncCheck
{
    SyncCheck() : mTime(-1), mObject(NULL) {}
    
    bool operator()()
    {
        t_syncinfo *info = sync_get_info();
        
        if (!info)
            return false;
        
        void *object = info->object;
        long time = info->time;
        
        if (time == mTime && object == mObject)
            return false;
        
        mTime = time;
        mObject = object;
        
        return true;
    }

    
private:
    
    long mTime;
    void *mObject;
    
};

//////////////////////////////////////////////////////////////////////////
////////////////////// Mutator for Synchronisation ///////////////////////
//////////////////////////////////////////////////////////////////////////

class Mutator : public MaxBase
{
    
public:
    
    Mutator(t_symbol *sym, long ac, t_atom *av)
    {
        mOutlet = outlet_new(this, "sync");
    }
    
    static void classInit(t_class *c, t_symbol *nameSpace, const char *classname)
    {
        addMethod<Mutator, &Mutator::mutate>(c, "signal");
    }
    
    void mutate(t_symbol *sym, long ac, t_atom *av)
    {
        sync_set_info(this, gettime());
        outlet_anything(mOutlet, gensym("sync"), 0, 0);
        sync_set_info(NULL, gettime());
    }
    
private:
    
    void *mOutlet;
};

//////////////////////////////////////////////////////////////////////////
////////////////////// Wrapper for Synchronisation ///////////////////////
//////////////////////////////////////////////////////////////////////////

template <class T> class Wrapper : public MaxBase
{
    
public:
    
    Wrapper(t_symbol *s, long argc, t_atom *argv)
    {
        // Create Patcher (you must report this as a subpatcher to get audio working)
        
        t_dictionary *d = dictionary_new();
        t_atom a;
        t_atom *av = NULL;
        long ac = 0;
        
        atom_setparse(&ac, &av, "@defrect 0 0 300 300");
        attr_args_dictionary(d, ac, av);
        atom_setobj(&a, d);
        patch_internal = (t_object *)object_new_typed(CLASS_NOBOX, gensym("jpatcher"),1, &a);
        
        // Create Objects
        
        // FIX - this should not be a macor for the name (needs to be stored somehow?? with the class...
        
        char name[256];
        sprintf(name, "unsynced.%s", OBJECT_NAME);
        
        // FIX - make me better
        
        char *text = NULL;
        long textsize = 0;
        
        atom_gettext(argc, argv, &textsize, &text, 0);
        obj_internal = jbox_get_object((t_object *) newobject_sprintf(patch_internal, "@maxclass newobj @text \"%s %s\" @patching_rect 0 0 30 10", name, text));
        obj_mutator = (t_object *) object_new_typed(CLASS_NOBOX, gensym("__fl.signal.mutator"), 0, NULL);
        
        // Free resources we no longer need
        
        sysmem_freeptr(av);
        freeobject((t_object *)d);
        sysmem_freeptr(text);
        
        // Get the object itself (typed)
        
        T *internal = (T *) obj_internal;
        
        long num_ins = internal->getNumIns();
        long num_outs = internal->getNumOuts();
        long num_audio_ins = internal->getNumAudioIns();
        long num_audio_outs = internal->getNumAudioOuts();
        
        internal->user_object = *this;
        
        // Create I/O
        
        in_outlets.resize(num_ins + num_audio_ins - 1);
        proxy_ins.resize(num_ins + num_audio_ins - 1);
        audio_outs.resize(num_audio_outs - 1);
        outs.resize(num_outs);
        
        // Inlets for messages/signals
        
        sync_in = (t_object *) outlet_new(NULL, NULL);
        
        for (long i = num_ins + num_audio_ins - 2; i >= 0 ; i--)
        {
            in_outlets[i] = (t_object *) outlet_new(NULL, NULL);
            proxy_ins[i] = (t_object *)  (i ? proxy_new(this, i, &unused) : NULL);
        }
        
        // Outlets for messages/signals
        
        for (long i = num_outs - 1; i >= 0 ; i--)
            outs[i] = (t_object *) outlet_new(this, NULL);
        for (long i = num_audio_outs - 2; i >= 0 ; i--)
            audio_outs[i] = (t_object *) outlet_new(this, "signal");
        
        // Object pointer types for internal object and mutator
        
        // Create Connections
        
        // Connect the audio sync in to the object and through to the mutator
        
        outlet_add(sync_in, inlet_nth(obj_internal, 0));
        outlet_add(outlet_nth(obj_internal, 0), inlet_nth(obj_mutator, 0));
        
        // Connect inlets (all types)
        
        for (long i = 0; i < num_audio_ins + num_ins - 1; i++)
            outlet_add(in_outlets[i], inlet_nth(obj_internal, i + 1));
        
        // Connect outlets (audio then frame and sync message outlets)
        
        for (long i = 0; i < num_audio_outs - 1; i++)
            outlet_add(outlet_nth(obj_internal, i + 1), audio_outs[i]);
        
        for (long i = 0; i < num_outs; i++)
        {
            outlet_add(outlet_nth(obj_internal, i + num_audio_outs), outs[i]);
            outlet_add(outlet_nth(obj_mutator, 0), outs[i]);
        }
    }
    
    ~Wrapper()
    {
        // Delete ins and proxies
        
        for (std::vector <t_object *>::iterator it = proxy_ins.begin(); it != proxy_ins.end(); it++)
        {
            if (*it)
                freeobject(*it);
        }
        
        for (std::vector <t_object *>::iterator it = in_outlets.begin(); it != in_outlets.end(); it++)
            freeobject(*it);
        
        // Free objects - N.B. - free the patch, but not the object within it (which will be freed by deleting the patch)
        
        freeobject(obj_mutator);
        freeobject(sync_in);
        freeobject(patch_internal);
    }
    
    void assist(void *b, long m, long a, char *s)
    {
        ((T *)obj_internal)->assist(b, m, a + 1, s);
    }
    
    void *subpatcher(long index, void *arg)
    {
        if ((t_ptr_uint) arg <= 1 || NOGOOD(arg))
            return NULL;
        
        return (index == 0) ? (void *) patch_internal : NULL;
    }
    
    void sync(t_symbol *sym, long ac, t_atom *av)
    {
        if (sync_check())
            outlet_anything(sync_in, gensym("signal"), ac, av);
    }
    
    void anything(t_symbol *sym, long ac, t_atom *av)
    {
        long inlet = getInlet();
        
        outlet_anything(in_outlets[inlet], sym, ac, av);
    }
    
    static void classInit(t_class *c, t_symbol *nameSpace, const char *classname)
    {
        addMethod<Wrapper<T>, &Wrapper<T>::sync>(c, "sync");
        addMethod<Wrapper<T>, &Wrapper<T>::anything>(c, "anything");
        addMethod<Wrapper<T>, &Wrapper<T>::subpatcher>(c, "subpatcher");
        addMethod<Wrapper<T>, &Wrapper<T>::assist>(c, "assist");
        
        // N.B. MUST add signal handling after dspInit to override the builtin responses
        
        dspInit(c);
        addMethod<Wrapper<T>, &Wrapper<T>::anything>(c, "signal");
        
        // Make sure the mutator class exists
        
        const char mutatorClassName[] = "__fl.signal.mutator";
                
        if (!class_findbyname(CLASS_NOBOX, gensym(mutatorClassName)))
            Mutator::makeClass<Mutator, &mutatorClass>(CLASS_NOBOX, mutatorClassName);
    }
    
private:
    
    // Objects (need freeing except the internal object which is owned by the patch)
    
    t_object *patch_internal;
    t_object *obj_internal;
    t_object *obj_mutator;
    
    // Inlets (must be freed)
    
    std::vector <t_object *> in_outlets;
    std::vector <t_object *> proxy_ins;
    t_object *sync_in;
    
    // Outlets (don't need to free - only the arrays need freeing)
    
    std::vector <t_object *> audio_outs;
    std::vector <t_object *> outs;
    
    // Sync Check
    
    SyncCheck sync_check;
    
    // Dummy for stuffloc on proxies
    
    long unused;
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

// Max Object

template <class T> class FrameLib_MaxObj : public MaxBase
{
    struct Input
    {
        t_object *proxy;
        
        FrameLib_MaxObj *object;
        unsigned long index;
        bool report_error;
    };

    FrameLib_MultiChannel *object;
    
    std::vector <Input> inputs;
    std::vector <void *> outputs;
    
    long proxy_num;
    long confirm_index;
    bool confirm;
    
    t_object *top_level_patcher;
    
public:
    t_object *user_object;
private:
    
    SyncCheck sync_check;

    t_symbol *ps_frame_connection_info() { return gensym("__frame__connection__info__"); }
    
public:
    
//////////////////////////////////////////////////////////////////////////
//////////////// Max Framelib Global and Common Functions ////////////////
//////////////////////////////////////////////////////////////////////////

    FrameLib_Global *getGlobal()
    {
        return (FrameLib_Global *) gensym("__FrameLib__Global__")->s_thing;
    }

    void setGlobal(FrameLib_Global *global)
    {
       gensym("__FrameLib__Global__")->s_thing = (t_object *) global;
    }

    FrameLib_Common *getCommon()
    {
        char str[256];
        sprintf(str, "__FrameLib__Common__%llx", (unsigned long long) top_level_patcher);
        return (FrameLib_Common *) gensym(str)->s_thing;
    }

    void setCommon(FrameLib_Common *common)
    {
        char str[256];
        sprintf(str, "__FrameLib__Common__%llx", (unsigned long long) top_level_patcher);
        gensym(str)->s_thing = (t_object *) common;
    }

    FrameLib_MultiChannel::ConnectionQueue *getConnectionQueue()
    {
        return getCommon()->mConnectionQueue;
    }

    FrameLib_DSP::DSPQueue *getDSPQueue()
    {
        return getCommon()->mDSPQueue;
    }

    FrameLib_Global_Allocator *getGlobalAllocator()
    {
        return getGlobal()->mAllocator;
    }

    FrameLib_Local_Allocator *getLocalAllocator()
    {
        return getCommon()->mAllocator;
    }

    void commonInit()
    {
        FrameLib_Global *global;
        FrameLib_Common *common;
        
        top_level_patcher = jpatcher_get_toppatcher(gensym("#P")->s_thing);
        
        if (!(global = getGlobal()))
            setGlobal(new FrameLib_Global);
        else
            getGlobal()->increment();
        
        if (!(common = getCommon()))
            setCommon(new FrameLib_Common(getGlobalAllocator()));
        else
            common->increment();
    }

    void commonFree()
    {
        setCommon(getCommon()->decrement());
        setGlobal(getGlobal()->decrement());
    }

//////////////////////////////////////////////////////////////////////////
/////////////////////////// Attribute Parsing ////////////////////////////
//////////////////////////////////////////////////////////////////////////

    bool isAttributeTag(t_symbol *sym)
    {
        return (sym && sym->s_name[0] == '#' && strlen(sym->s_name) > 1);
    }

    bool isInputTag(t_symbol *sym)
    {
        return (sym && sym->s_name[0] == '/' && strlen(sym->s_name) > 1);
    }
                
    FrameLib_Attributes::Serial *parseAttributes(long argc, t_atom *argv)
    {
        t_symbol *sym;
        double array[4096];
        char argNames[64];
        long i, j;
       
        // Allocate
        
        FrameLib_Attributes::Serial *serialisedAttributes = new FrameLib_Attributes::Serial();
        
        // Parse arguments
        
        for (i = 0; i < argc; i++)
        {
            sprintf(argNames, "%ld", i);
            
            sym = atom_getsym(argv + i);
            
            if (isAttributeTag(sym) || isInputTag(sym))
                break;
            
    #ifndef OBJECT_ARGS_SET_ALL_INPUTS
            if (sym != gensym(""))
                serialisedAttributes->write(argNames, sym->s_name);
            else
            {
                array[0] = atom_getfloat(argv + i);
                serialisedAttributes->write(argNames, array, 1);
            }
    #endif
        }
        
        // Parse attributes
        
        while (i < argc)
        {
            // Strip stray items
            
            for (j = 0; i < argc; i++, j++)
            {
                if (isAttributeTag(atom_getsym(argv + i)) || isInputTag(atom_getsym(argv + i)))
                {
                    sym = atom_getsym(argv + i);
                    break;
                }
            
                if (j == 0)
                    object_error(user_object, "stray items after entry %s", sym->s_name);
            }
            
            // Check for lack of values or end of list
            
            if ((++i >= argc) || isAttributeTag(atom_getsym(argv + i)) || isInputTag(atom_getsym(argv + i)))
            {
                if (i < (argc + 1))
                    object_error(user_object, "no values given for entry %s", sym->s_name);
                continue;
            }
            
            // Do strings (if not an input)
            
            if (!isInputTag(sym) && atom_getsym(argv + i) != gensym(""))
            {
                serialisedAttributes->write(sym->s_name + 1, atom_getsym(argv + i++)->s_name);
                continue;
            }
            
            // Collect doubles
            
            for (j = 0; i < argc; i++, j++)
            {
                if (isAttributeTag(atom_getsym(argv + i)) || isInputTag(atom_getsym(argv + i)))
                    break;
                    
                if (atom_getsym(argv + i) != gensym(""))
                    object_error(user_object, "string %s in entry list where value expected", atom_getsym(argv + i)->s_name);
                
                array[j] = atom_getfloat(argv + i);
            }
            
            if (!isInputTag(sym))
                serialisedAttributes->write(sym->s_name + 1, array, j);
        }

        return serialisedAttributes;
    }

//////////////////////////////////////////////////////////////////////////
///////////////////////////// Input Parsing //////////////////////////////
//////////////////////////////////////////////////////////////////////////


    unsigned long inputNumber(t_symbol *sym)
    {
        return atoi(sym->s_name + 1) - 1;
    }

    void parseInputs(long argc, t_atom *argv)
    {
        t_symbol *sym;
        double array[4096];
        long i, j;
        
        // Parse arguments
        
        for (i = 0; i < argc; i++)
        {
            sym = atom_getsym(argv + i);
            
            if (isAttributeTag(sym) || isInputTag(sym))
                break;
            
    #ifdef OBJECT_ARGS_SET_ALL_INPUTS
            array[i] = atom_getfloat(argv + i);
    #endif
        }

    #ifdef OBJECT_ARGS_SET_ALL_INPUTS
        if (i)
        {
            for (j = 0; j < object->getNumIns(); j++)
                object->setFixedInput(j, array, i);
        }
    #endif

        // Parse attributes
        
        while (i < argc)
        {
            // Strip stray items
            
            for (j = 0; i < argc; i++, j++)
            {
                if (isAttributeTag(atom_getsym(argv + i)) || isInputTag(atom_getsym(argv + i)))
                {
                    sym = atom_getsym(argv + i);
                    break;
                }
            }
            
            // Check for lack of values or end of list
            
            if ((++i >= argc) || isAttributeTag(atom_getsym(argv + i)) || isInputTag(atom_getsym(argv + i)))
                continue;
            
            // Do strings (if not an input)
            
            if (!isInputTag(sym) && atom_getsym(argv + i) != gensym(""))
                continue;
            
            // Collect doubles
            
            for (j = 0; i < argc; i++, j++)
            {
                if (isAttributeTag(atom_getsym(argv + i)) || isInputTag(atom_getsym(argv + i)))
                    break;
        
                array[j] = atom_getfloat(argv + i);
            }
            
            if (isInputTag(sym))
                object->setFixedInput(inputNumber(sym), array, j);
        }
    }


//////////////////////////////////////////////////////////////////////////
/////////////////////// Main / New / Free / Assist ///////////////////////
//////////////////////////////////////////////////////////////////////////

    template <class U> static void makeClass(t_symbol *nameSpace, const char *className)
    {
        // If handles audio/scheduler then make wrapper class and name the inner object differently..
        
        char internalClassName[256];
        
        if (T::handlesAudio())
        {
            Wrapper<U>:: template makeClass<Wrapper<U>, &wrapperClass>(CLASS_BOX, className);
            sprintf(internalClassName, "unsynced.%s", className);
        }
        else
            strcpy(internalClassName, className);
        
        MaxBase::makeClass<U, &objectClass>(nameSpace, internalClassName);
    }
    
    static void classInit(t_class *c, t_symbol *nameSpace, const char *classname)
    {
        addMethod(c, (method) &connectionCall, "internal_connect");
        addMethod<FrameLib_MaxObj<T>, &FrameLib_MaxObj<T>::assist>(c, "assist");
        addMethod<FrameLib_MaxObj<T>, &FrameLib_MaxObj<T>::frame>(c, "frame");
        addMethod<FrameLib_MaxObj<T>, &FrameLib_MaxObj<T>::sync>(c, "sync");
        addMethod<FrameLib_MaxObj<T>, &FrameLib_MaxObj<T>::dsp>(c);
        
        dspInit(c);
    }

    long getNumAudioIns()
    {
        return (long) object->getNumAudioIns() + (T::handlesAudio() ? 1 : 0);
    }

    long getNumAudioOuts()
    {
        return (long) object->getNumAudioOuts() + (T::handlesAudio() ? 1 : 0);
    }

    long getNumIns()
    {
        return (long) object->getNumIns();
    }

    long getNumOuts()
    {
        return (long) object->getNumOuts();
    }

    FrameLib_MaxObj(t_symbol *s, long argc, t_atom *argv)
    {
        // Init
        
        commonInit();
        user_object = (t_object *)this;

        // Object creation with attributes and arguments
        
        FrameLib_Attributes::Serial *serialisedAttributes = parseAttributes(argc, argv);

        object = new T(getConnectionQueue(), getDSPQueue(), serialisedAttributes, this);
        
        delete serialisedAttributes;
        
        parseInputs(argc, argv);
        
        inputs.resize(getNumIns());
        outputs.resize(getNumOuts());
        
        for (long i = getNumIns() - 1; i >= 0; i--)
        {
            // N.B. - we create a proxy if the inlet is not the first inlet (not the first frame input or the object handles audio)
            
            inputs[i].proxy = (t_object *) ((i || T::handlesAudio()) ? proxy_new(this, getNumAudioIns() + i, &proxy_num) : NULL);
            inputs[i].object = NULL;
            inputs[i].index = 0;
            inputs[i].report_error = TRUE;
        }
        
        for (unsigned long i = getNumOuts(); i > 0; i--)
            outputs[i - 1] = outlet_new(this, NULL);
        
        // Setup for audio, even if the object doesn't handle it, so that dsp recompile works correctly
        
        dspSetup(getNumAudioIns());
 
        for (unsigned long i = 0; i < getNumAudioOuts(); i++)
            outlet_new(this, "signal");
        
        confirm_index = -1;
        confirm = false;
    }

    ~FrameLib_MaxObj()
    {
        dspFree();

        for (typename std::vector <Input>::iterator it = inputs.begin(); it != inputs.end(); it++)
            if (it->proxy)
                object_free(it->proxy);
        
        delete object;
        
        commonFree();
    }

    void assist (void *b, long m, long a, char *s)
    {
        if (m == ASSIST_OUTLET)
        {
            sprintf(s,"(signal) Output %ld", a + (T::handlesAudio() ? 0 : 1));
        }
        else
        {
            sprintf(s,"(signal) Input %ld", a + (T::handlesAudio() ? 0 : 1));
        }
    }


//////////////////////////////////////////////////////////////////////////
//////////////////////// Perform / DSP Routines //////////////////////////
//////////////////////////////////////////////////////////////////////////

    void perform(t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
    {
        // N.B. Plus one due to sync inputs
        
        object->blockProcess(ins + 1, outs + 1, vec_size);
    }


    void dsp(t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
    {
        // Check / make connections
        
        connections();

        // Set sampling rate
        
        object->setSamplingRate(samplerate);
        
        // Add a perform routine to the chain if the object handles audio
        
        if (T::handlesAudio())
            addPerform<FrameLib_MaxObj, &FrameLib_MaxObj<T>::perform>(dsp64);
    }


//////////////////////////////////////////////////////////////////////////
////////////////////////// Connection Routines ///////////////////////////
//////////////////////////////////////////////////////////////////////////

    // Connection Mode Enum

    enum ConnectMode { kConnect, kConfirm, kDoubleCheck };

    struct ConnectionInfo
    {
        ConnectionInfo(FrameLib_MaxObj *obj, unsigned long idx, t_object *topPatch, ConnectMode type) :
        object(obj), index(idx), top_level_patcher(topPatch), mode(type) {}
        
        FrameLib_MaxObj *object;
        unsigned long index;
        t_object *top_level_patcher;
        ConnectMode mode;
        
    };

    static void connectionCall(FrameLib_MaxObj *x, unsigned long index, ConnectMode mode)
    {
        x->connect(index, mode);
    }

    void connectExternal(FrameLib_MaxObj *x, unsigned long index, ConnectMode mode)
    {
        object_method(x, gensym("internal_connect"), index, mode);
    }
    
    ConnectionInfo* getConnectionInfo()
    {
        return (ConnectionInfo *) ps_frame_connection_info()->s_thing;
    }
    
    void setConnectionInfo(ConnectionInfo *info)
    {
        ps_frame_connection_info()->s_thing = (t_object *) info;
    }
    
    void connect(unsigned long index, ConnectMode mode)
    {
        ConnectionInfo info(this, index, top_level_patcher, mode);
        
        setConnectionInfo(&info);
        outlet_anything(outputs[index], gensym("frame"), 0, NULL);
        setConnectionInfo(NULL);
    }

    void connections()
    {
        // Check input connections
        
        for (unsigned long i = 0; i < getNumIns(); i++)
        {
            inputs[i].report_error = true;
            
            if (inputs[i].object)
            {
                confirm = false;
                confirm_index = i;
                
                if (object->isConnected(i))
                    connectExternal(inputs[i].object, inputs[i].index, kConfirm);
                
                if (!confirm)
                {
                    // Object is no longer connected as before
                    
                    inputs[i].object = NULL;
                    inputs[i].index = 0;
                    
                    if (object->isConnected(i))
                        object->deleteConnection(i);
                }
                
                confirm = false;
                confirm_index = -1;
            }
        }
        
        // Make output connections
        
        for (unsigned long i = getNumOuts(); i > 0; i--)
            connect(i - 1, kConnect);
        
        // Reset DSP
        
        object->reset();
    }

    void frame()
    {
        ConnectionInfo *info = getConnectionInfo();
        
        long index = getInlet() - getNumAudioIns();
        
        if (!info || index < 0)
            return;
        
        switch (info->mode)
        {
            case kConnect:
            {
                bool connection_change = (inputs[index].object != info->object || inputs[index].index != info->index);
                bool valid = (info->top_level_patcher == top_level_patcher && info->object != this);
                
                // Confirm that the object is valid
                
                if (!valid)
                {
                    if (info->object == this)
                        object_error(user_object, "direct feedback loop detected");
                    else
                        object_error(user_object, "cannot connect objects from different top level patchers");
                }
                
                // Check for double connection *only* if the internal object is connected (otherwise the previously connected object has been deleted)
                
                if (inputs[index].object && inputs[index].report_error && connection_change && object->isConnected(index))
                {
                    confirm_index = index;
                    connectExternal(inputs[index].object, inputs[index].index, kDoubleCheck);
                    confirm_index = -1;
                    inputs[index].report_error = false;
                }
                
                // Always change the connection if the new object is valid (only way to ensure new connections work)
                
                if (connection_change && valid)
                {
                    inputs[index].object = info->object;
                    inputs[index].index = info->index;
                    
                    object->addConnection(info->object->object, info->index, index);
                }
            }
                break;
                
            case kConfirm:
                if (index == confirm_index && inputs[index].object == info->object && inputs[index].index == info->index)
                    confirm = true;
                break;
                
            case kDoubleCheck:
                if (index == confirm_index && inputs[index].object == info->object && inputs[index].index == info->index)
                    object_error(user_object, "extra connection to input %ld", index + 1);
                break;
        }
    }

    void sync()
    {
        if (sync_check())
            for (unsigned long i = getNumOuts(); i > 0; i--)
                outlet_anything(outputs[i - 1], gensym("sync"), 0, NULL);
    }
};

#ifndef CUSTOM_OBJECT
extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxObj<OBJECT_CLASS>::makeClass<FrameLib_MaxObj<OBJECT_CLASS> >(CLASS_BOX, OBJECT_NAME);
}
#endif

