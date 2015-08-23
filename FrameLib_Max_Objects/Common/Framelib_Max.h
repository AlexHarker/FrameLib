
#include <ext.h>
#include <ext_obex.h>
#include <ext_obex_util.h>
#include <z_dsp.h>

#include "FrameLib_Multichannel.h"
#include "FrameLib_DSP.h"
#include "FrameLib_Globals.h"

// FIX - sort out formatting style / template class style / naming?
// FIX - improve reporting of extra connections + look into feedback detection...
// FIX - think about adding assist helpers for this later...
// FIX - threadsafety??

//////////////////////////////////////////////////////////////////////////
///////////////////////////// Structures Etc. ////////////////////////////
//////////////////////////////////////////////////////////////////////////

// Object class and structures

t_class *this_class;


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
    framelib_set_global(framelib_get_global()->decrement());
    framelib_set_common(x, framelib_get_common(x)->decrement());
}

//////////////////////////////////////////////////////////////////////////
//////////////////////// Object Function Protypes ////////////////////////
//////////////////////////////////////////////////////////////////////////

FrameLib_Attributes::Serial *framelib_parse_attributes(t_framelib *x, long argc, t_atom *argv);

void *framelib_new (t_symbol *s, long argc, t_atom *argv);
void framelib_free (t_framelib *x);
void framelib_assist (t_framelib *x, void *b, long m, long a, char *s);

void framelib_perform(t_framelib *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);
void framelib_dsp (t_framelib *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);

void framelib_connections(t_framelib *x);
void framelib_frame(t_framelib *x);

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
                object_error((t_object *)x, "stray items after attribute %s", sym->s_name);
        }
        
        // Check for lack of values or end of list
        
        if ((++i >= argc) || is_attribute_tag(atom_getsym(argv + i)))
        {
            if (i < (argc + 1))
                object_error((t_object *) x, "no values given for attribute %s", sym->s_name);
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
                object_error((t_object *)x, "string %s in attribute list where value expected", atom_getsym(argv + i)->s_name);
            
            array[j] = atom_getfloat(argv + i);
        }
        
        serialisedAttributes->write(sym->s_name + 1, array, j);
    }

    return serialisedAttributes;
}

//////////////////////////////////////////////////////////////////////////
/////////////////////// Main / New / Free / Assist ///////////////////////
//////////////////////////////////////////////////////////////////////////

// FIX - experimental
/*
t_max_err framelib_notify(t_framelib *x, t_symbol *s, t_symbol *msg, void *sender, void *data)
{
    post ("notify %x, %s, %s, %x %x", x, s->s_name, msg->s_name, sender, data);
}*/

extern "C" int C74_EXPORT main (void)
{
	this_class = class_new (OBJECT_NAME,
							(method)framelib_new,
							(method)framelib_free,
							sizeof(t_framelib),
							0L,
							A_GIMME,
							0);
    
    // FIX - experimental

    //class_addmethod (this_class, (method)framelib_notify, "notify", A_CANT, 0L);
    
	class_addmethod (this_class, (method)framelib_assist, "assist", A_CANT, 0L);
	class_addmethod (this_class, (method)framelib_dsp, "dsp64", A_CANT, 0L);
	class_addmethod (this_class, (method)framelib_frame, "frame", 0L);
    
	class_dspinit(this_class);
	
	class_register(CLASS_BOX, this_class);
		
    ps_frame_connection_info = gensym("__frame__connection__info__");
    
    return 0;
}


void *framelib_new (t_symbol *s, long argc, t_atom *argv)
{
    t_framelib *x = (t_framelib *)object_alloc (this_class);
    
    // FIX - experimental
    
    /*
     t_patcher *p = gensym("#P")->s_thing;
    t_class *c = object_class(p);
    unsigned long numMess = c->c_messcount;
    for (unsigned long i = 0; i < numMess; i++)
    {
        if (c->c_messlist[i].m_sym && c->c_messlist[i].m_sym->s_name)
            post ("message %s", c->c_messlist[i].m_sym->s_name);
    }
    
    object_attach_byptr(x, p);
    */
    
    // Init
    
    framelib_common_init(x);

    // Object creation with attributes and arguments
    
    FrameLib_Attributes::Serial *serialisedAttributes = framelib_parse_attributes(x, argc, argv);

    x->object = new OBJECT_CLASS(getConnectionQueue(x), getDSPQueue(x), serialisedAttributes);
    
    delete serialisedAttributes;
    
    x->inputs = (t_framelib_input *) malloc(sizeof(t_framelib_input) * x->object->getNumIns());
    x->outputs = (void **) malloc(sizeof(void *) * x->object->getNumOuts());
    
    for (unsigned long i = 0; i < x->object->getNumIns(); i++)
    {
        x->inputs[i].proxy = (i != 0 || x->object->getNumAudioIns()) ? proxy_new(x, x->object->getNumIns() - i, &x->proxy_num) : NULL;
        x->inputs[i].object = NULL;
        x->inputs[i].index = 0;
        x->inputs[i].report_error = TRUE;
    }
    
    for (unsigned long i = x->object->getNumOuts(); i > 0; i--)
        x->outputs[i - 1] = outlet_new(x, NULL);
    
    // Setup for audio, even if the object doesn't handle it, so that dsp recompile works correctly
    
    dsp_setup((t_pxobject *)x, (long) x->object->getNumAudioIns());
    
    x->x_obj.z_misc = Z_NO_INPLACE;
    
    for (unsigned long i = 0; i < x->object->getNumAudioOuts(); i++)
        outlet_new(x, "signal");
    
    x->confirm_index = -1;
    x->confirm = FALSE;
    
	return(x);
}


void framelib_free(t_framelib *x)
{
	dsp_free((t_pxobject *)x);

    for (unsigned long i = 0; i < x->object->getNumIns(); i++)
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


void framelib_perform (t_framelib *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{
	x->object->blockProcess(ins, outs, vec_size);
}


void framelib_dsp (t_framelib *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{
    // Check / make connections
    
    framelib_connections(x);

    // Set sampling rate
    
    x->object->setSamplingRate(samplerate);
    
    // Add a perform routine to the chain if the object handles audio
    
	if (x->object->handlesAudio())
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
    
    for (unsigned long i = 0; i < x->object->getNumIns(); i++)
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
    
    for (unsigned long i = x->object->getNumOuts(); i > 0; i--)
        framelib_connect(x, i - 1, kConnect);
    
    // Reset DSP
    
    x->object->reset();
}


void framelib_frame(t_framelib *x)
{
    t_framelib_connection_info *info = (t_framelib_connection_info *) ps_frame_connection_info->s_thing;
    
    long confirm_index = x->confirm_index;
    long index = proxy_getinlet((t_object *) x) - x->object->getNumAudioIns();
    
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
                    object_error((t_object *) x, "direct feedback loop detected");
                else
                    object_error((t_object *) x, "cannot connect objects from different top level patchers");
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
                object_error((t_object *) x, "extra connection to input %ld", index + 1);
            break;
    }
}