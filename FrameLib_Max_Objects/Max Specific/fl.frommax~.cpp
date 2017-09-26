
#include "fl.frommax~.h"

// Underlying FrameLib Class

// Constructor

FrameLib_FromMax::FrameLib_FromMax(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, owner, &sParamInfo, 1, 1)
{
    mParameters.addEnum(kMode, "mode", 0);
    mParameters.addEnumItem(kValues, "values");
    mParameters.addEnumItem(kParams, "params");
        
    mParameters.set(serialisedParameters);
    
    mMode = (Modes) mParameters.getValue(kMode);
    
    outputMode(0, mMode == kValues ? kFrameNormal : kFrameTagged);
    
    mMessages = ((FrameLib_MaxClass_FromMax *) owner)->getMessages();
}

// Info

std::string FrameLib_FromMax::objectInfo(bool verbose)
{
    return formatInfo("Turn max messages into frames: In values mode the output is the last receive value(s) as a vector. "
                   "In params mode messages are collected and output as a single tagged frame for setting parameters.",
                   "Turn max messages into frames.", verbose);
}

std::string FrameLib_FromMax::inputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Trigger Frame - triggers output", "Trigger Frame", verbose);
}

std::string FrameLib_FromMax::outputInfo(unsigned long idx, bool verbose)
{
    return "Output Frames";
}

// Parameter Info

FrameLib_FromMax::ParameterInfo FrameLib_FromMax::sParamInfo;

FrameLib_FromMax::ParameterInfo::ParameterInfo()
{
    add("Sets the object mode. "
        "values - translate values from max into vectors. "
        "params - translate messages into concatenated tagged frames to set parameters");
}

// Process

void FrameLib_FromMax::process()
{
    unsigned long size = 0;
    
    mMessages->mLock.acquire();
    
    if (mMode == kValues)
    {
        requestOutputSize(0, mMessages->mList.size());
        allocateOutputs();
        
        double *output = getOutput(0, &size);
        
        mMessages->mList.get(output, size);
    }
    else
    {
        requestOutputSize(0, mMessages->calcTaggedSize());
        allocateOutputs();
        
        FrameLib_Parameters::Serial *serialisedParameters = getOutput(0);
        
        mMessages->writeTagged(serialisedParameters);
        mMessages->mTagsNeedClear = true;
    }
    
    mMessages->mLock.release();
}

// Max Class

// Class Initisation

void FrameLib_MaxClass_FromMax::classInit(t_class *c, t_symbol *nameSpace, const char *classname)
{
    FrameLib_MaxClass::classInit(c, nameSpace, classname);
    
    addMethod<FrameLib_MaxClass_FromMax, &FrameLib_MaxClass_FromMax::intHandler>(c, "int");
    addMethod<FrameLib_MaxClass_FromMax, &FrameLib_MaxClass_FromMax::floatHandler>(c, "float");
    addMethod<FrameLib_MaxClass_FromMax, &FrameLib_MaxClass_FromMax::list>(c, "list");
    addMethod<FrameLib_MaxClass_FromMax, &FrameLib_MaxClass_FromMax::anything>(c, "anything");
}

// Additional handlers

void FrameLib_MaxClass_FromMax::intHandler(t_atom_long in)
{
    floatHandler(in);
}

void FrameLib_MaxClass_FromMax::floatHandler(double in)
{
    atom a;
    atom_setfloat(&a, in);
    
    list(NULL, 1, &a);
}

void FrameLib_MaxClass_FromMax::list(t_symbol *s, long argc, t_atom *argv)
{
    mMessages.mLock.acquire();
    if (mMessages.mTagsNeedClear)
        mMessages.mTagged.clear();
    mMessages.mList.set(argv, argc);
    mMessages.mTagsNeedClear = false;
    mMessages.mLock.release();
}

void FrameLib_MaxClass_FromMax::anything(t_symbol *s, long argc, t_atom *argv)
{
    if (argc > 1 && atom_gettype(argv) == A_SYM)
        object_error(mUserObject, "too many arguments for string value");
    
    mMessages.mLock.acquire();
    if (mMessages.mTagsNeedClear)
        mMessages.mTagged.clear();
    
    mMessages.mTagged.push_back(MaxMessage::TaggedMessages());
    mMessages.mTagged.back().setTag(s->s_name);
    
    if (atom_gettype(argv) == A_SYM)
        mMessages.mTagged.back().set(atom_getsym(argv)->s_name);
    else
        mMessages.mTagged.back().set(argv, argc);
    
    mMessages.mTagsNeedClear = false;
    mMessages.mLock.release();
}

// Main

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass_FromMax::makeClass<FrameLib_MaxClass_FromMax>(CLASS_BOX, "fl.frommax~");
}
