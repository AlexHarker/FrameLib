
#include <ext.h>
#include <ext_obex.h>
#include <ext_obex_util.h>
#include <z_dsp.h>

#include "FrameLib_Multichannel.h"
#include "FrameLib_DSP.h"

// FIX - sort out formatting style
// FIX - reporting of invalid connections (doubles and wrong dsp_object) - also deletion with double connections
// FIX - naming
// FIX - object creation
// FIX - arguments / attributes etc.
// FIX - patcher local queues please
// FIX - better dsp validity fix (parent patcher?)
// FIX - think about adding name helpers for this later...

//////////////////////////////////////////////////////////////////////////
////////////////////////// Max Framelib Globals //////////////////////////
//////////////////////////////////////////////////////////////////////////

void framelib_init();
void framelib_global_free();

FrameLib_MultiChannel::ConnectionQueue *getConnectionQueue();
FrameLib_DSP::DSPQueue *getDSPQueue();


typedef struct _framelib_global
{
    FrameLib_MultiChannel::ConnectionQueue *connectionQueue;
    FrameLib_DSP::DSPQueue *dspQueue;
    FrameLib_Memory *allocator;
    
} t_framelib_global;


t_framelib_global *global;
t_symbol *ps_framelib_global;


void framelib_init()
{
	ps_framelib_global = gensym ("__FrameLib__Global__");
    global = (t_framelib_global *) ps_framelib_global->s_thing;
	
	if (!global)
	{
		global  = new t_framelib_global;
        global->connectionQueue = new FrameLib_MultiChannel::ConnectionQueue;
        global->dspQueue = new FrameLib_DSP::DSPQueue;
        global->allocator = new FrameLib_Memory;
        
        ps_framelib_global->s_thing = (t_object *) global;
        
        quittask_install((method)framelib_global_free, 0);
	}
}

void framelib_global_free()
{
    ps_framelib_global->s_thing = NULL;

    delete global->connectionQueue;
    delete global->dspQueue;
    delete global;
    
    global = NULL;
}

FrameLib_MultiChannel::ConnectionQueue *getConnectionQueue()
{
    return global->connectionQueue;
}

FrameLib_DSP::DSPQueue *getDSPQueue()
{
    return global->dspQueue;
}

FrameLib_Memory *getAllocator()
{
    return global->allocator;
}

//////////////////////////////////////////////////////////////////////////
///////////////////////////// Structures Etc. ////////////////////////////
//////////////////////////////////////////////////////////////////////////

// Connection Mode Enum

enum ConnectMode { kConnect, kConfirm };


// Object class and structures

t_class *this_class;


t_symbol *ps_frame_connection_info;


typedef struct _framelib_connection_info
{
    struct _framelib *object;
    t_object *dsp_object;
    unsigned long index;
    ConnectMode mode;
    
} t_framelib_connection_info;


typedef struct _framelib_input
{
    void *proxy;
    
    struct _framelib *object;
    unsigned long index;
    bool valid;
    
} t_framelib_input;


typedef struct _framelib
{
    t_pxobject x_obj;
    
    FrameLib_MultiChannel *object;
		
    t_object *dsp_object;
    
    void **outputs;
    t_framelib_input *inputs;
    
    long proxy_num;
    long confirm_index;
    
    bool confirm;
    
} t_framelib;


// Function prototypes

t_max_err framelib_fixed_size_set(t_framelib *x, t_object *attr, long argc, t_atom *argv);
t_atom_long framelib_arg_check(t_framelib *x, char *name, t_atom_long val, t_atom_long min, t_atom_long max);

void *framelib_new (t_symbol *s, short argc, t_atom *argv);
void framelib_free (t_framelib *x);
void framelib_assist (t_framelib *x, void *b, long m, long a, char *s);

void framelib_perform(t_framelib *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);
void framelib_dsp (t_framelib *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);

void framelib_connections(t_framelib *x);
void framelib_frame(t_framelib *x);

//////////////////////////////////////////////////////////////////////////
/////////////////////// Main / New / Free / Assist ///////////////////////
//////////////////////////////////////////////////////////////////////////


extern "C" int C74_EXPORT main (void)
{
	this_class = class_new (OBJECT_NAME,
							(method)framelib_new,
							(method)framelib_free,
							sizeof(t_framelib),
							0L,
							A_GIMME,
							0);
    
	class_addmethod (this_class, (method)framelib_assist, "assist", A_CANT, 0L);
	class_addmethod (this_class, (method)framelib_dsp, "dsp64", A_CANT, 0L);
	class_addmethod (this_class, (method)framelib_frame, "frame", 0L);
	
	class_dspinit(this_class);
	
	class_register(CLASS_BOX, this_class);
		
    ps_frame_connection_info = gensym("__frame__connection__info__");
    
    framelib_init();
    return 0;
}


void *framelib_new (t_symbol *s, short argc, t_atom *argv)
{
    t_framelib *x = (t_framelib *)object_alloc (this_class);
    
    double schedspeed = 16.0;
    long numIO = 1;
    
    if (argc)
        numIO = atom_getlong(argv);
    if (argc)
        schedspeed = atom_getfloat(argv);
    
    x->object = OBJECT_CREATE;
    
    // Setup for audio, even if the object doesn't handle it, so that dsp recompile works correctly
    
    dsp_setup((t_pxobject *)x, (long) x->object->getNumAudioIns());
        
    x->x_obj.z_misc = Z_NO_INPLACE;
        
    for (unsigned long i = 0; i < x->object->getNumAudioOuts(); i++)
        outlet_new(x, "signal");
    
    x->inputs = (t_framelib_input *) malloc(sizeof(t_framelib_input) * x->object->getNumIns());
    x->outputs = (void **) malloc(sizeof(void *) * x->object->getNumOuts());
    
    for (unsigned long i = 0; i < x->object->getNumIns(); i++)
    {
        x->inputs[i].proxy = (i != 0 || x->object->getNumAudioIns()) ? proxy_new(x, i, &x->proxy_num) : NULL;
        x->inputs[i].object = NULL;
        x->inputs[i].valid = NULL;
        x->inputs[i].index = 0;
    }
    
    for (unsigned long i = 0; i < x->object->getNumOuts(); i++)
        x->outputs[i] = outlet_new(x, NULL);
    
    x->dsp_object = NULL;
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
    // Make / check connections
    
    x->dsp_object = dsp64;
    
    framelib_connections(x);
    
    // Reset DSP
    
    x->object->reset();
    
    // Add a perform routine to the chain if the object handles audio
    
	if (x->object->handlesAudio())
		object_method(dsp64, gensym("dsp_add64"), x, framelib_perform, 0, NULL);
}


//////////////////////////////////////////////////////////////////////////
////////////////////////// Connection Routines ///////////////////////////
//////////////////////////////////////////////////////////////////////////


void framelib_connect(t_framelib *x, unsigned long index, ConnectMode mode)
{
    // FIX - make this safer when we keep track in both directions of where connections are made
    
    if (NOGOOD(x))
        return;
    
    t_framelib_connection_info info;
    
    info.object = x;
    info.index = index;
    info.mode = mode;
    info.dsp_object = x->dsp_object;
    
    ps_frame_connection_info->s_thing = (t_object *) &info;
    outlet_anything(x->outputs[index], gensym("frame"), 0, NULL);
    ps_frame_connection_info->s_thing = NULL;
}


void framelib_connections(t_framelib *x)
{
    // Check input connections
    
    for (unsigned long i = 0; i < x->object->getNumIns(); i++)
    {
        if (x->inputs[i].object != NULL)
        {
            x->confirm = FALSE;
            x->confirm_index = i;

            framelib_connect(x->inputs[i].object, x->inputs[i].index, kConfirm);
            
            if (!x->confirm)
            {
                // Object is no longer connected as before
                
                x->inputs[i].object = NULL;
                x->inputs[i].index = 0;
                x->inputs[i].valid = FALSE;
                
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
}


void framelib_frame(t_framelib *x)
{
    if (ps_frame_connection_info->s_thing == NULL)
        return;
        
    t_framelib_connection_info *info = (t_framelib_connection_info *) ps_frame_connection_info->s_thing;
    
    long confirm_index = x->confirm_index;
    long index = proxy_getinlet((t_object *) x);
   
    bool connection_change = FALSE;
    bool valid = (info->dsp_object == x->dsp_object) && x->dsp_object && info->dsp_object;

    //if (!valid && x->dsp_object && info->dsp_object)
    //object_error((t_object *) x, "cannot connect objects from different patches");
    
    switch (info->mode)
    {
        case kConnect:
            connection_change = (x->inputs[index].object != info->object || x->inputs[index].index != info->index || x->inputs[index].valid != valid);
            connection_change = (valid && !x->object->isConnected(index)) ? TRUE : connection_change;
            break;
            
        case kConfirm:
            if (index == confirm_index && x->inputs[index].object == info->object && x->inputs[index].index == info->index)
            {
                x->confirm = TRUE;
                connection_change = (valid != x->inputs[index].valid) || (valid && !x->object->isConnected(index));
            }
            break;
    }

    if (connection_change)
    {
        x->inputs[index].object = info->object;
        x->inputs[index].index = info->index;
        x->inputs[index].valid = valid;

        if (valid)
            x->object->addConnection(info->object->object, info->index, index);
        else
        {
            if(x->object->isConnected(index))
                x->object->deleteConnection(index);
        }
    }
}

