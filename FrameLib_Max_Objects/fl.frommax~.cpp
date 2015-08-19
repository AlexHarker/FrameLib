
#include <ext.h>
#include <ext_obex.h>
#include <ext_obex_util.h>
#include <z_dsp.h>

#include "FrameLib_Multichannel.h"
#include "FrameLib_DSP.h"
#include "FrameLib_Globals.h"

// FIX - THE MAX STUFF NEEDS TO BE EXTENSIBLE SO I DON'T HAVE TO DO THIS
// FIX - SOOOOOOO MANY HACKX - THIS NEEDS A MUCH SAFER APPROACH!
// FIX - memory management is horrible - don't memory manage use st::vector in the audio thread - replace this also

// TO DO

// 1 - allow objects to take a void pointer for their "owner" throughout the system so that you can access whatever the hell you want from inside the object (with no hackz)
// 2 - rewrite the max wrapper to be an extensible class to which max methods can be added etc.
// 3 - redo memory management and structs so they are less horrible and ugly

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

// Specific to this object

struct TaggedMessages
{
    char mTag[129];
    char mString[129];
    bool mStringFlag;
    double mValues[4096];
    unsigned long mCount;
};

struct MaxMessage
{
    MaxMessage() : mCount(0) {}
    
    FrameLib_SpinLock mLock;
    
    double mValues[4096];
    unsigned long mCount;
    std::vector<TaggedMessages> mTagged;
    
    // FIX - this is horrible, but it will work for now and is safe I think (as long as this attribute is always set last by the program... (or is checked for by max)
    // think of other ways to do this later
    
    void addressToString(char *string)
    {
        FL_UIntPtr ptr = (FL_UIntPtr) this;
        
        while (ptr)
        {
            *string++ = ((ptr & 0xF) + 1);
            ptr >>= 0x04;
        }
        
        *string = 0;
    }
    
    static MaxMessage *stringToAddress(const char *string)
    {
        FL_UIntPtr ptr = 0;
        FL_UIntPtr offset = 0;
        while (*string)
        {
            ptr |= ((*string++ & 0x1F) - 1) << offset;
            offset += 0x04;
        }
        
        return (MaxMessage *) ptr;
    }
};

class FrameLib_FromMax : public FrameLib_Processor
{
    enum AttributeList {kMode, kMessages};
    enum Modes {kValues, kParams};
    
public:
    
    FrameLib_FromMax (DSPQueue *queue, FrameLib_Attributes::Serial *serialisedAttributes) : FrameLib_Processor(queue, 1, 1)
    {
        mAttributes.addEnum(kMode, "mode", 0);
        mAttributes.addEnumItem(kValues, "values");
        mAttributes.addEnumItem(kParams, "params");
        
        mAttributes.addString(kMessages, "messages", "");
        mAttributes.set(serialisedAttributes);
        
        mMode = (Modes) mAttributes.getValue(kMode);
        
        outputMode(0, mMode == kValues ? kOutputNormal : kOutputTagged);
        
        mMessages = MaxMessage::stringToAddress(mAttributes.getString(kMessages));
    }
    
protected:
    
    void process()
    {
        unsigned long size = 0;
        
        mMessages->mLock.acquire();
        
        if (mMode == kValues)
        {
            size = mMessages->mCount;
            requestOutputSize(0, size);
            allocateOutputs();
        
            double *output = getOutput(0, &size);
        
            for (unsigned int i = 0; i < size; i++)
                output[i] = mMessages->mValues[i];
        }
        else
        {
            for (unsigned long i = 0; i < mMessages->mTagged.size(); i++)
            {
                if (mMessages->mTagged[i].mStringFlag)
                    size += FrameLib_Attributes::Serial::calcSize(mMessages->mTagged[i].mTag, mMessages->mTagged[i].mString);
                else
                    size += FrameLib_Attributes::Serial::calcSize(mMessages->mTagged[i].mTag, mMessages->mTagged[i].mCount);
            }
            
            requestOutputSize(0, size);
            allocateOutputs();

            FrameLib_Attributes::Serial *serialisedAttributes = getOutput(0);
            
            for (unsigned long i = 0; i < mMessages->mTagged.size(); i++)
            {
                if (mMessages->mTagged[i].mStringFlag)
                    serialisedAttributes->write(mMessages->mTagged[i].mTag, mMessages->mTagged[i].mString);
                else
                    serialisedAttributes->write(mMessages->mTagged[i].mTag, mMessages->mTagged[i].mValues, mMessages->mTagged[i].mCount);
            }
            
            mMessages->mTagged.clear();
        }
        
        mMessages->mLock.release();
    }
    
private:
    
    MaxMessage *mMessages;
    Modes mMode;
};



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

// FIX - sort out formatting style / template class style / naming?
// FIX - improve reporting of extra connections + look into feedback detection...
// FIX - think about adding assist helpers for this later...
// FIX - threadsafety??

//////////////////////////////////////////////////////////////////////////
///////////////////////////// Structures Etc. ////////////////////////////
//////////////////////////////////////////////////////////////////////////

// Connection Mode Enum

enum ConnectMode { kConnect, kConfirm, kDoubleCheck };


// Object class and structures

t_class *this_class;


t_symbol *ps_frame_connection_info;


typedef struct _framelib_connection_info
{
    struct _framelib *object;
    t_object *top_level_patcher;
    unsigned long index;
    ConnectMode mode;
    bool doubled_connection;
    
} t_framelib_connection_info;


typedef struct _framelib_input
{
    void *proxy;
    
    struct _framelib *object;
    unsigned long index;
    bool valid;
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
    
    // FIX - object specific stuff here... - must go last and this is NOT NICE
    MaxMessage *messages;
    
} t_framelib;

// FIX - object specific stuff

void framelib_int(t_framelib *x, t_atom_long in)
{
    x->messages->mLock.acquire();
    x->messages->mValues[0] = in;
    x->messages->mCount = 1;
    x->messages->mLock.release();
}

void framelib_float(t_framelib *x, double in)
{
    x->messages->mLock.acquire();
    x->messages->mValues[0] = in;
    x->messages->mCount = 1;
    x->messages->mLock.release();
}

void framelib_list(t_framelib *x, t_symbol *s, long argc, t_atom *argv)
{
    argc = argc > 4096 ? 4096 : argc;
    
    x->messages->mLock.acquire();
    for (unsigned long i = 0; i < argc; i++)
        x->messages->mValues[i] = atom_getfloat(argv + i);
    x->messages->mCount = argc;
    x->messages->mLock.release();
}

void copyString(char *str, const char *toCopy)
{
    size_t i = 0;
    
    if (toCopy != NULL)
    {
        for (i = 0; i < 128; i++)
            if ((str[i] = toCopy[i]) == 0)
                break;
    }
    
    str[i] = 0;
}

void framelib_anything(t_framelib *x, t_symbol *s, long argc, t_atom *argv)
{
    argc = argc > 4096 ? 4096 : argc;
    
    x->messages->mLock.acquire();
    x->messages->mTagged.push_back(TaggedMessages());
    copyString(x->messages->mTagged.back().mTag, s->s_name);
    
    if (atom_gettype(argv) == A_SYM)
    {
        if (argc > 1)
            object_error((t_object *)x, "too many arguments for string value");
        
        copyString(x->messages->mTagged.back().mString, atom_getsym(argv)->s_name);
        x->messages->mTagged.back().mStringFlag = TRUE;
    }
    else
    {
        for (unsigned long i = 0; i < argc; i++)
            x->messages->mTagged.back().mValues[i] = atom_getfloat(argv + i);
        x->messages->mTagged.back().mStringFlag = FALSE;
        x->messages->mTagged.back().mCount = argc;
    }
    x->messages->mLock.release();
}


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


extern "C" int C74_EXPORT main (void)
{
    // FIX - object specific stuff in here
    
	this_class = class_new ("fl.frommax~",
							(method)framelib_new,
							(method)framelib_free,
							sizeof(t_framelib),
							0L,
							A_GIMME,
							0);
    
    class_addmethod (this_class, (method)framelib_int, "int", A_DEFLONG, 0L);
    class_addmethod (this_class, (method)framelib_float, "float", A_DEFFLOAT, 0L);
    class_addmethod (this_class, (method)framelib_list, "list", A_GIMME, 0L);
    class_addmethod (this_class, (method)framelib_anything, "anything", A_GIMME, 0L);

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
    
    // Init
    
    framelib_common_init(x);

    // Object creation with attributes and arguments
    
    FrameLib_Attributes::Serial *serialisedAttributes = framelib_parse_attributes(x, argc, argv);

    // FIX - object specific stuff here...
    char address[256];
    x->messages = new MaxMessage;
    x->messages->addressToString(address);
    serialisedAttributes->write("messages", address);
    
    x->object = new FrameLib_Expand<FrameLib_FromMax>(getConnectionQueue(x), getDSPQueue(x), serialisedAttributes);
    
    delete serialisedAttributes;
    
    x->inputs = (t_framelib_input *) malloc(sizeof(t_framelib_input) * x->object->getNumIns());
    x->outputs = (void **) malloc(sizeof(void *) * x->object->getNumOuts());
    
    for (unsigned long i = 0; i < x->object->getNumIns(); i++)
    {
        x->inputs[i].proxy = (i != 0 || x->object->getNumAudioIns()) ? proxy_new(x, x->object->getNumIns() - i, &x->proxy_num) : NULL;
        x->inputs[i].object = NULL;
        x->inputs[i].valid = NULL;
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
    // FIX - object specific stuff here...
    delete x->messages;
    
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


void framelib_connect(t_framelib *x, unsigned long index, ConnectMode mode)
{
    // FIX - make this safer when we keep track in both directions of where connections are made
    
    if (NOGOOD(x) || !x->outputs)
        return;
    
    t_framelib_connection_info info;
    
    info.object = x;
    info.index = index;
    info.mode = mode;
    info.top_level_patcher = x->top_level_patcher;
    
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
    
    // Reset DSP
    
    x->object->reset();
}


void framelib_frame(t_framelib *x)
{
    if (ps_frame_connection_info->s_thing == NULL)
        return;
        
    t_framelib_connection_info *info = (t_framelib_connection_info *) ps_frame_connection_info->s_thing;
    
    long confirm_index = x->confirm_index;
    long index = proxy_getinlet((t_object *) x) - x->object->getNumAudioIns();
    
    if (index < 0)
        return;
    
    bool connection_change = FALSE;
    bool valid = (info->top_level_patcher == x->top_level_patcher || info->object == x);

    if (!valid && info->mode == kConnect)
    {
        if (info->object == x)
            object_error((t_object *) x, "direct feedback loop detected");
        else
            object_error((t_object *) x, "cannot connect objects from different top level patchers");
    }
    
    switch (info->mode)
    {
        case kConnect:
            connection_change = (x->inputs[index].object != info->object || x->inputs[index].index != info->index);

            // Check for double connection
            
            if (x->inputs[index].object && x->inputs[index].report_error && connection_change)
            {
                x->confirm_index = index;
                framelib_connect(x->inputs[index].object, x->inputs[index].index, kDoubleCheck);
                x->confirm_index = -1;
                x->inputs[index].report_error = FALSE;
            }
            
            // Just in case...
            
            connection_change = (valid && !x->object->isConnected(index)) ? TRUE : connection_change;
            break;
            
        case kConfirm:
            if (index == confirm_index && x->inputs[index].object == info->object && x->inputs[index].index == info->index)
            {
                x->confirm = TRUE;
                connection_change = (valid != x->inputs[index].valid) || (valid && !x->object->isConnected(index));
            }
            break;
            
        case kDoubleCheck:
            if (index == confirm_index && x->inputs[index].object == info->object && x->inputs[index].index == info->index)
                object_error((t_object *) x, "extra connection to input %ld", index + 1);
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

