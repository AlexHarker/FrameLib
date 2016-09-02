
#include <ext.h>
#include <ext_obex.h>
#include <ext_obex_util.h>
#include <z_dsp.h>

#include "FrameLib_Multichannel.h"
#include "FrameLib_DSP.h"
#include "FrameLib_Globals.h"

#include <vector>

// FIX - sort out formatting style / template class style / naming?
// FIX - improve reporting of extra connections + look into feedback detection...
// FIX - think about adding assist helpers for this later...
// FIX - threadsafety??

void wrapper_main(const char *name);

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

typedef struct _sync_check
{
    long time;
    void *object;
    
} t_sync_check;

bool sync_valid(t_sync_check *x)
{
    t_syncinfo *info = sync_get_info();
    
    if (!info)
        return false;
    
    void *object = info->object;
    long time = info->time;
    
    if (time == x->time && object == x->object)
        return false;
    
    x->time = time;
    x->object = object;
    return true;
}


//////////////////////////////////////////////////////////////////////////
///////////////////////////// Structures Etc. ////////////////////////////
//////////////////////////////////////////////////////////////////////////

// Object class and structures

t_class *framelib_class;


t_symbol *ps_frame_connection_info;

typedef struct _framelib_input
{
    void *proxy;
    
    struct _framelib *object;
    unsigned long index;
    bool report_error;
    
} t_framelib_input;


typedef struct _framelib
{
    t_pxobject x_obj;
    
    FrameLib_MultiChannel *object;
    
    t_framelib_input *inputs;
    void **outputs;
    
    long proxy_num;
    long confirm_index;
    bool confirm;
    
    t_object *top_level_patcher;
    t_object *user_object;
    
    t_sync_check sync_check;
    
} t_framelib;

//////////////////////////////////////////////////////////////////////////
//////////////// Max Framelib Global and Common Functions ////////////////
//////////////////////////////////////////////////////////////////////////

FrameLib_Global *framelib_get_global()
{
    return (FrameLib_Global *) gensym("__FrameLib__Global__")->s_thing;
}

void framelib_set_global(FrameLib_Global *global)
{
   gensym("__FrameLib__Global__")->s_thing = (t_object *) global;
}

FrameLib_Common *framelib_get_common(t_framelib *x)
{
    char str[256];
    sprintf(str, "__FrameLib__Common__%llx", (unsigned long long) x->top_level_patcher);
    return (FrameLib_Common *) gensym(str)->s_thing;
}

void framelib_set_common(t_framelib *x, FrameLib_Common *common)
{
    char str[256];
    sprintf(str, "__FrameLib__Common__%llx", (unsigned long long) x->top_level_patcher);
    gensym(str)->s_thing = (t_object *) common;
}

FrameLib_MultiChannel::ConnectionQueue *getConnectionQueue(t_framelib *x)
{
    return framelib_get_common(x)->mConnectionQueue;
}

FrameLib_DSP::DSPQueue *getDSPQueue(t_framelib *x)
{
    return framelib_get_common(x)->mDSPQueue;
}

FrameLib_Global_Allocator *getGlobalAllocator()
{
    return framelib_get_global()->mAllocator;
}

FrameLib_Local_Allocator *getLocalAllocator(t_framelib *x)
{
    return framelib_get_common(x)->mAllocator;
}

void framelib_common_init(t_framelib *x)
{
    FrameLib_Global *global;
    FrameLib_Common *common;
    
    x->top_level_patcher = jpatcher_get_toppatcher(gensym("#P")->s_thing);
    
	if (!(global = framelib_get_global()))
        framelib_set_global(new FrameLib_Global);
    else
        framelib_get_global()->increment();
    
    if (!(common = framelib_get_common(x)))
        framelib_set_common(x, new FrameLib_Common(getGlobalAllocator()));
    else
        common->increment();
}

void framelib_common_free(t_framelib *x)
{
    framelib_set_common(x, framelib_get_common(x)->decrement());
    framelib_set_global(framelib_get_global()->decrement());
}

//////////////////////////////////////////////////////////////////////////
//////////////////////// Object Function Protypes ////////////////////////
//////////////////////////////////////////////////////////////////////////

FrameLib_Attributes::Serial *framelib_parse_attributes(t_framelib *x, long argc, t_atom *argv);

void *framelib_new(t_symbol *s, long argc, t_atom *argv);
void framelib_free(t_framelib *x);
void framelib_assist(t_framelib *x, void *b, long m, long a, char *s);

void framelib_perform(t_framelib *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);
void framelib_dsp(t_framelib *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);

void framelib_connections(t_framelib *x);
void framelib_frame(t_framelib *x);
void framelib_sync(t_framelib *x);

//////////////////////////////////////////////////////////////////////////
/////////////////////////// Attribute Parsing ////////////////////////////
//////////////////////////////////////////////////////////////////////////

bool is_attribute_tag(t_symbol *sym)
{
    return (sym && sym->s_name[0] == '#' && strlen(sym->s_name) > 1);
}

FrameLib_Attributes::Serial *framelib_parse_attributes(t_framelib *x, long argc, t_atom *argv)
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
        
        if (is_attribute_tag(sym = atom_getsym(argv + i)))
            break;
        
        if (sym != gensym(""))
            serialisedAttributes->write(argNames, sym->s_name);
        else
        {
            array[0] = atom_getfloat(argv + i);
            serialisedAttributes->write(argNames, array, 1);
        }
    }
    
    // Parse attributes
    
    while (i < argc)
    {
        // Strip stray items
        
        for (j = 0; i < argc; i++, j++)
        {
            if (is_attribute_tag(atom_getsym(argv + i)))
            {
                sym = atom_getsym(argv + i);
                break;
            }
        
            if (j == 0)
                object_error(x->user_object, "stray items after attribute %s", sym->s_name);
        }
        
        // Check for lack of values or end of list
        
        if ((++i >= argc) || is_attribute_tag(atom_getsym(argv + i)))
        {
            if (i < (argc + 1))
                object_error(x->user_object, "no values given for attribute %s", sym->s_name);
            continue;
        }
        
        // Do strings
        
        if (atom_getsym(argv + i) != gensym(""))
        {
            serialisedAttributes->write(sym->s_name + 1, atom_getsym(argv + i++)->s_name);
            continue;
        }
        
        // Collect doubles
        
        for (j = 0; i < argc; i++, j++)
        {
            if (is_attribute_tag(atom_getsym(argv + i)))
                break;
                
            if (atom_getsym(argv + i) != gensym(""))
                object_error(x->user_object, "string %s in attribute list where value expected", atom_getsym(argv + i)->s_name);
            
            array[j] = atom_getfloat(argv + i);
        }
        
        serialisedAttributes->write(sym->s_name + 1, array, j);
    }

    return serialisedAttributes;
}

//////////////////////////////////////////////////////////////////////////
/////////////////////// Main / New / Free / Assist ///////////////////////
//////////////////////////////////////////////////////////////////////////


extern "C" int C74_EXPORT main(void)
{
    // If handles audio/scheduler then make wrapper class and name the inner object differently..
    
    char main_name[256];

    if (OBJECT_CLASS::handlesAudio())
    {
        wrapper_main(OBJECT_NAME);
        sprintf(main_name, "unsynced.%s", OBJECT_NAME);
    }
    else
        strcpy(main_name, OBJECT_NAME);
    
	framelib_class = class_new(main_name,
							(method)framelib_new,
							(method)framelib_free,
							sizeof(t_framelib),
							0L,
							A_GIMME,
							0);
    
	class_addmethod(framelib_class, (method)framelib_assist, "assist", A_CANT, 0L);
	class_addmethod(framelib_class, (method)framelib_dsp, "dsp64", A_CANT, 0L);
	class_addmethod(framelib_class, (method)framelib_frame, "frame", 0L);
    class_addmethod(framelib_class, (method)framelib_sync, "sync", 0L);
    
	class_dspinit(framelib_class);
	
	class_register(CLASS_BOX, framelib_class);
		
    ps_frame_connection_info = gensym("__frame__connection__info__");
    
    return 0;
}

long get_num_audio_ins(t_framelib *x)
{
    return (long) x->object->getNumAudioIns() + (OBJECT_CLASS::handlesAudio() ? 1 : 0);
}

long get_num_audio_outs(t_framelib *x)
{
    return (long) x->object->getNumAudioOuts() + (OBJECT_CLASS::handlesAudio() ? 1 : 0);
}

long get_num_ins(t_framelib *x)
{
    return (long) x->object->getNumIns();
}

long get_num_outs(t_framelib *x)
{
    return (long) x->object->getNumOuts();
}

void *framelib_new(t_symbol *s, long argc, t_atom *argv)
{
    t_framelib *x = (t_framelib *)object_alloc (framelib_class);
    
    // Init
    
    framelib_common_init(x);
    x->user_object = (t_object *)x;

    // Object creation with attributes and arguments
    
    FrameLib_Attributes::Serial *serialisedAttributes = framelib_parse_attributes(x, argc, argv);

    x->object = new OBJECT_CLASS(getConnectionQueue(x), getDSPQueue(x), serialisedAttributes);
    
    delete serialisedAttributes;
    
    x->inputs = (t_framelib_input *) malloc(sizeof(t_framelib_input) * get_num_ins(x));
    x->outputs = (void **) malloc(sizeof(void *) * get_num_outs(x));
    
    for (long i = get_num_ins(x) - 1; i >= 0; i--)
    {
        // N.B. - we create a proxy if the inlet is not the first inlet (not the first frame input or the object handles audio)
        
        x->inputs[i].proxy = (i || OBJECT_CLASS::handlesAudio()) ? proxy_new(x, get_num_audio_ins(x) + i, &x->proxy_num) : NULL;
        x->inputs[i].object = NULL;
        x->inputs[i].index = 0;
        x->inputs[i].report_error = TRUE;
    }
    
    for (unsigned long i = get_num_outs(x); i > 0; i--)
        x->outputs[i - 1] = outlet_new(x, NULL);
    
    // Setup for audio, even if the object doesn't handle it, so that dsp recompile works correctly
    
    dsp_setup((t_pxobject *)x, get_num_audio_ins(x));
    
    x->x_obj.z_misc = Z_NO_INPLACE;
    
    for (unsigned long i = 0; i < get_num_audio_outs(x); i++)
        outlet_new(x, "signal");
    
    x->confirm_index = -1;
    x->confirm = FALSE;
    
    x->sync_check.object = NULL;
    x->sync_check.time = 0;
    
	return x;
}


void framelib_free(t_framelib *x)
{
	dsp_free((t_pxobject *)x);

    for (unsigned long i = 0; i < get_num_ins(x); i++)
        if (x->inputs[i].proxy != NULL)
            object_free((t_object *) x->inputs[i].proxy);
    
    free(x->inputs);
    free(x->outputs);
    
    delete x->object;
    
    framelib_common_free(x);
}


void framelib_assist (t_framelib *x, void *b, long m, long a, char *s)
{
	if (m == ASSIST_OUTLET)
	{
		sprintf(s,"(signal) Output %ld", a + 1);
	}
	else
	{
		sprintf(s,"(signal) Input %ld", a + 1);
	}
}


//////////////////////////////////////////////////////////////////////////
//////////////////////// Perform / DSP Routines //////////////////////////
//////////////////////////////////////////////////////////////////////////


void framelib_perform(t_framelib *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{
    // N.B. Plus one due to sync inputs
    
	x->object->blockProcess(ins + 1, outs + 1, vec_size);
}


void framelib_dsp(t_framelib *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{
    // Check / make connections
    
    framelib_connections(x);

    // Set sampling rate
    
    x->object->setSamplingRate(samplerate);
    
    // Add a perform routine to the chain if the object handles audio
    
	if (OBJECT_CLASS::handlesAudio())
		object_method(dsp64, gensym("dsp_add64"), x, framelib_perform, 0, NULL);
}


//////////////////////////////////////////////////////////////////////////
////////////////////////// Connection Routines ///////////////////////////
//////////////////////////////////////////////////////////////////////////

// Connection Mode Enum

enum ConnectMode { kConnect, kConfirm, kDoubleCheck };


typedef struct _framelib_connection_info
{
    struct _framelib *object;
    unsigned long index;
    t_object *top_level_patcher;
    ConnectMode mode;
    
} t_framelib_connection_info;


void framelib_connect(t_framelib *x, unsigned long index, ConnectMode mode)
{
    // FIX - check safety - this now seems good, as long as we make a reasonable check that the object exists before calling this on another object
    
    if (!x->outputs)
        return;
    
    t_framelib_connection_info info;
    
    info.object = x;
    info.index = index;
    info.top_level_patcher = x->top_level_patcher;
    info.mode = mode;
    
    ps_frame_connection_info->s_thing = (t_object *) &info;
    outlet_anything(x->outputs[index], gensym("frame"), 0, NULL);
    ps_frame_connection_info->s_thing = NULL;
}


void framelib_connections(t_framelib *x)
{
    // Check input connections
    
    for (unsigned long i = 0; i < get_num_ins(x); i++)
    {
        x->inputs[i].report_error = TRUE;
        
        if (x->inputs[i].object != NULL)
        {
            x->confirm = FALSE;
            x->confirm_index = i;
            
            if (x->object->isConnected(i))
                framelib_connect(x->inputs[i].object, x->inputs[i].index, kConfirm);
            
            if (!x->confirm)
            {
                // Object is no longer connected as before
                
                x->inputs[i].object = NULL;
                x->inputs[i].index = 0;
                
                if (x->object->isConnected(i))
                    x->object->deleteConnection(i);
            }
            
            x->confirm = FALSE;
            x->confirm_index = -1;
        }
    }
    
    // Make output connections
    
    for (unsigned long i = get_num_outs(x); i > 0; i--)
        framelib_connect(x, i - 1, kConnect);
    
    // Reset DSP
    
    x->object->reset();
}

void framelib_frame(t_framelib *x)
{
    t_framelib_connection_info *info = (t_framelib_connection_info *) ps_frame_connection_info->s_thing;
    
    long confirm_index = x->confirm_index;
    long index = proxy_getinlet((t_object *) x) - get_num_audio_ins(x);
    
    if (!info || index < 0)
        return;
    
    switch (info->mode)
    {
        case kConnect:
        {            
            bool connection_change = (x->inputs[index].object != info->object || x->inputs[index].index != info->index);
            bool valid = (info->top_level_patcher == x->top_level_patcher && info->object != x);
            
            // Confirm that the object is valid
            
            if (!valid)
            {
                if (info->object == x)
                    object_error(x->user_object, "direct feedback loop detected");
                else
                    object_error(x->user_object, "cannot connect objects from different top level patchers");
            }
            
            // Check for double connection *only* if the internal object is connected (otherwise the previously connected object has been deleted)
            
            if (x->inputs[index].object && x->inputs[index].report_error && connection_change && x->object->isConnected(index))
            {
                x->confirm_index = index;
                framelib_connect(x->inputs[index].object, x->inputs[index].index, kDoubleCheck);
                x->confirm_index = -1;
                x->inputs[index].report_error = FALSE;
            }
            
            // Always change the connection if the new object is valid (only way to ensure new connections work)
            
            if (connection_change && valid)
            {
                x->inputs[index].object = info->object;
                x->inputs[index].index = info->index;
                
                x->object->addConnection(info->object->object, info->index, index);
            }
        }
            break;
            
        case kConfirm:
            if (index == confirm_index && x->inputs[index].object == info->object && x->inputs[index].index == info->index)
                x->confirm = TRUE;
            break;
            
        case kDoubleCheck:
            if (index == confirm_index && x->inputs[index].object == info->object && x->inputs[index].index == info->index)
                object_error(x->user_object, "extra connection to input %ld", index + 1);
            break;
    }
}

void framelib_sync(t_framelib *x)
{
    if (sync_valid(&x->sync_check))
        for (unsigned long i = get_num_outs(x); i > 0; i--)
            outlet_anything(x->outputs[i - 1], gensym("sync"), 0, NULL);
}

//////////////////////////////////////////////////////////////////////////
////////////////////// Mutator for Synchronisation ///////////////////////
//////////////////////////////////////////////////////////////////////////

t_class *mutator_class;

typedef struct _mutator
{
    t_pxobject x_obj;
    void *out;
    
} t_mutator;

void signal_mutate(t_mutator *x, t_symbol *sym, long ac, t_atom *av)
{
    sync_set_info(x, gettime());
    outlet_anything(x->out, gensym("sync"), 0, 0);
    sync_set_info(NULL, gettime());
}

void *create_mutator()
{
    t_mutator *x = (t_mutator *)object_alloc(mutator_class);
    x->out = outlet_new((t_object *)x, "sync");
    return x;
}

void mutator_main()
{    
    if (!(mutator_class = class_findbyname(CLASS_NOBOX, gensym("__fl.signal.mutator"))))
    {
        mutator_class = class_new("__fl.signal.mutator", (method)create_mutator, NULL, sizeof(t_mutator), NULL, 0);
        class_addmethod(mutator_class, (method)signal_mutate, "signal", A_GIMME, 0);
        class_register(CLASS_NOBOX, mutator_class);
    }
}

//////////////////////////////////////////////////////////////////////////
////////////////////// Wrapper for Synchronisation ///////////////////////
//////////////////////////////////////////////////////////////////////////

t_class *wrapper_class;

typedef struct _wrapper
{
    t_pxobject x_obj;
    
    // Objects
    
    t_object *patch_internal;
    t_object *obj_internal;
    t_object *obj_mutator;
    
    // Inlets (must be freed)
    
    void **in_outlets;
    void **proxy_ins;
    void *sync_in;
    
    // Outlets (don't need to free - only the arrays need freeing)
    
    void **audio_outs;
    void **outs;

    // Sync Check
    
    t_sync_check sync_check;

    // Dummy for stuffloc on proxies
    
    long unused;
    
} t_wrapper;

void *wrapper_new(t_symbol *s, long argc, t_atom *argv)
{
    t_wrapper *x = (t_wrapper *)object_alloc(wrapper_class);
    
    // Create Patcher (you must report this as a subpatcher to get audio working)
    
    t_dictionary *d = dictionary_new();
    t_atom a;
    t_atom *av = NULL;
    long ac = 0;
    
    atom_setparse(&ac, &av, "@defrect 0 0 300 300");
    attr_args_dictionary(d, ac, av);
    atom_setobj(&a, d);
    x->patch_internal = (t_object *)object_new_typed(CLASS_NOBOX,gensym("jpatcher"),1, &a);
    
    // Create Objects
    
    char name[256];
    sprintf(name, "unsynced.%s", OBJECT_NAME);

    // FIX - make me better
    
    char *text = NULL;
    long textsize = 0;
    
    atom_gettext(argc, argv, &textsize, &text, 0);
    x->obj_internal = jbox_get_object((t_object *) newobject_sprintf(x->patch_internal, "@maxclass newobj @text \"%s %s\" @patching_rect 0 0 30 10", name, text));
    x->obj_mutator = (t_object *) object_new_typed(CLASS_NOBOX, gensym("__fl.signal.mutator"), 0, NULL);
    
    // Free resources we no longer need
    
    sysmem_freeptr(av);
    freeobject((t_object *)d);
    sysmem_freeptr(text);
    
    // Get the object itself (typed)
    
    t_framelib *internal = (t_framelib *) x->obj_internal;
    
    long num_ins = get_num_ins(internal);
    long num_outs = get_num_outs(internal);
    long num_audio_ins = get_num_audio_ins(internal);
    long num_audio_outs = get_num_audio_outs(internal);
    
    internal->user_object = (t_object *)x;
    
    // Create I/O
    
    x->in_outlets = (void **) malloc(sizeof(void *) * (num_ins + num_audio_ins - 1));
    x->proxy_ins = (void **) malloc(sizeof(void *) * (num_ins + num_audio_ins - 1));
    x->audio_outs = (void **) malloc(sizeof(void *) * (num_audio_outs - 1));
    x->outs = (void **) malloc(sizeof(void *) * num_outs);
    
    // Inlets for messages/signals
    
    x->sync_in = outlet_new(NULL, NULL);
    
    for (long i = num_ins + num_audio_ins - 2; i >= 0 ; i--)
    {
        x->in_outlets[i] = outlet_new(NULL, NULL);
        x->proxy_ins[i] = i ? proxy_new(x, i, &x->unused) : NULL;
    }
    
    // Outlets for messages/signals
    
    for (long i = num_outs - 1; i >= 0 ; i--)
        x->outs[i] = outlet_new((t_object *)x, NULL);
    for (long i = num_audio_outs - 2; i >= 0 ; i--)
        x->audio_outs[i] = outlet_new((t_object *)x, "signal");
    
    // Object pointer types for internal object and mutator
    
    // Create Connections
    
    // Connect the audio sync in to the object and through to the mutator
    
    outlet_add(x->sync_in, inlet_nth(x->obj_internal, 0));
    outlet_add(outlet_nth(x->obj_internal, 0), inlet_nth(x->obj_mutator, 0));
    
    // Connect inlets (all types)
    
    for (long i = 0; i < num_audio_ins + num_ins - 1; i++)
        outlet_add(x->in_outlets[i], inlet_nth(x->obj_internal, i + 1));
    
    // Connect outlets (audio then frame and sync message outlets)
    
    for (long i = 0; i < num_audio_outs - 1; i++)
        outlet_add(outlet_nth(x->obj_internal, i + 1), x->audio_outs[i]);
    
    for (long i = 0; i < num_outs; i++)
    {
        outlet_add(outlet_nth(x->obj_internal, i + num_audio_outs), x->outs[i]);
        outlet_add(outlet_nth(x->obj_mutator, 0), x->outs[i]);
    }
    
    x->sync_check.object = NULL;
    x->sync_check.time = 0;
    
    return x;
}

void wrapper_free(t_wrapper *x)
{
    // Delete ins and proxies
    
    t_framelib *internal = (t_framelib *) x->obj_internal;
    
    long num_ins = get_num_ins(internal);
    long num_audio_ins = get_num_audio_ins(internal);
    
    for (long i = num_ins + num_audio_ins - 2; i >= 0 ; i--)
    {
        if (x->proxy_ins[i])
            freeobject((t_object *)x->proxy_ins[i]);
        freeobject((t_object *)x->in_outlets[i]);
    }
    
    // Free arrays
    
    free(x->in_outlets);
    free(x->proxy_ins);
    free(x->audio_outs);
    free(x->outs);
    
    // N.B. - delete the patch, but not the object within it (which will be freed by deleting the patch)
    
    freeobject((t_object *)x->obj_mutator);
    freeobject((t_object *)x->sync_in);
    freeobject(x->patch_internal);
}

void wrapper_assist(t_wrapper *x, void *b, long m, long a, char *s)
{
    framelib_assist((t_framelib *) x->obj_internal, b, m, a + 1, s);
}

void *wrapper_subpatcher(t_wrapper *x, long index, void *arg)
{
    if ((t_ptr_uint) arg <= 1 || NOGOOD(arg))
        return NULL;
    
    return (index == 0) ? (void *) x->patch_internal : NULL;
}

void wrapper_sync(t_wrapper *x, t_symbol *sym, long ac, t_atom *av)
{
    if (sync_valid(&x->sync_check))
        outlet_anything(x->sync_in, gensym("signal"), ac, av);
}

void wrapper_anything(t_wrapper *x, t_symbol *sym, long ac, t_atom *av)
{
    long inlet = proxy_getinlet((t_object *) x);
    
    outlet_anything(x->in_outlets[inlet], sym, ac, av);
}

void wrapper_main(const char *name)
{
    wrapper_class = class_new(name,
                            (method)wrapper_new,
                            (method)wrapper_free,
                            sizeof(t_wrapper),
                            NULL,
                            A_GIMME,
                            0);
    
    class_addmethod(wrapper_class, (method)wrapper_sync, "sync", A_GIMME, 0);
    class_addmethod(wrapper_class, (method)wrapper_anything, "anything", A_GIMME, 0);
    class_addmethod(wrapper_class, (method)wrapper_subpatcher, "subpatcher", A_CANT, 0);
    class_addmethod(wrapper_class, (method)wrapper_assist, "assist", A_CANT, 0);
    
    // N.B. MUST add signal handling after dspinit to override the builtin responses
    
    class_dspinit(wrapper_class);
    class_addmethod(wrapper_class, (method)wrapper_anything, "signal", A_GIMME, 0);
    
    class_register(CLASS_BOX, wrapper_class);
    
    mutator_main();
}