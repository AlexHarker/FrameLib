
#include "fl.frommax~.h"

// Underlying FrameLib Class

// Constructor

FrameLib_FromMax::FrameLib_FromMax(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, 1, 1)
{
    mParameters.addEnum(kMode, "mode", 0);
    mParameters.addEnumItem(kValues, "values");
    mParameters.addEnumItem(kParams, "params");
    
    mParameters.setInfo(&sParamInfo);
    
    mParameters.set(serialisedParameters);
    
    mMode = (Modes) mParameters.getValue(kMode);
    
    outputMode(0, mMode == kValues ? kFrameNormal : kFrameTagged);
    
    mMessages = ((FrameLib_MaxClass_FromMax *) owner)->getMessages();
}

// Info

const char *FrameLib_FromMax::objectInfo(bool verbose)
{
    return getInfo("Turn max messages into frames: In values mode the output is the last receive value(s) as a vector. "
                   "In params mode all messages are collected and output as a single tagged frame suitable for setting parameters.",
                   "Turn max messages into frames.", verbose);
}

const char *FrameLib_FromMax::inputInfo(unsigned long idx, bool verbose)
{
    return getInfo("Trigger Frame - triggers output", "Trigger Frame", verbose);
}

const char *FrameLib_FromMax::outputInfo(unsigned long idx, bool verbose)
{
    return "Output Frames";
}

// Parameter Info

FrameLib_FromMax::ParameterInfo FrameLib_FromMax::sParamInfo;

FrameLib_FromMax::ParameterInfo::ParameterInfo()
{
    add("Sets the object mode."
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
        size = mMessages->mCount;
        requestOutputSize(0, size);
        allocateOutputs();
        
        double *output = getOutput(0, &size);
        
        for (unsigned long i = 0; i < size; i++)
            output[i] = mMessages->mValues[i];
    }
    else
    {
        for (unsigned long i = 0; i < mMessages->mTagged.size(); i++)
        {
            if (mMessages->mTagged[i].mStringFlag)
                size += FrameLib_Parameters::Serial::calcSize(mMessages->mTagged[i].mTag, mMessages->mTagged[i].mString);
            else
                size += FrameLib_Parameters::Serial::calcSize(mMessages->mTagged[i].mTag, mMessages->mTagged[i].mCount);
        }
        
        requestOutputSize(0, size);
        allocateOutputs();
        
        FrameLib_Parameters::Serial *serialisedParameters = getOutput(0);
        
        for (unsigned long i = 0; i < mMessages->mTagged.size(); i++)
        {
            if (mMessages->mTagged[i].mStringFlag)
                serialisedParameters->write(mMessages->mTagged[i].mTag, mMessages->mTagged[i].mString);
            else
                serialisedParameters->write(mMessages->mTagged[i].mTag, mMessages->mTagged[i].mValues, mMessages->mTagged[i].mCount);
        }
        
        mMessages->mTagged.clear();
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
    mMessages.mLock.acquire();
    mMessages.mValues[0] = in;
    mMessages.mCount = 1;
    mMessages.mLock.release();
}

void FrameLib_MaxClass_FromMax::floatHandler(double in)
{
    mMessages.mLock.acquire();
    mMessages.mValues[0] = in;
    mMessages.mCount = 1;
    mMessages.mLock.release();
}

void FrameLib_MaxClass_FromMax::list(t_symbol *s, long argc, t_atom *argv)
{
    argc = argc > 4096 ? 4096 : argc;
    
    mMessages.mLock.acquire();
    for (unsigned long i = 0; i < argc; i++)
        mMessages.mValues[i] = atom_getfloat(argv + i);
    mMessages.mCount = argc;
    mMessages.mLock.release();
}

void FrameLib_MaxClass_FromMax::anything(t_symbol *s, long argc, t_atom *argv)
{
    argc = argc > 4096 ? 4096 : argc;
    
    mMessages.mLock.acquire();
    mMessages.mTagged.push_back(MaxMessage::TaggedMessages());
    copyString(mMessages.mTagged.back().mTag, s->s_name);
    
    if (atom_gettype(argv) == A_SYM)
    {
        if (argc > 1)
            object_error(mUserObject, "too many arguments for string value");
        
        copyString(mMessages.mTagged.back().mString, atom_getsym(argv)->s_name);
        mMessages.mTagged.back().mStringFlag = true;
    }
    else
    {
        for (unsigned long i = 0; i < argc; i++)
            mMessages.mTagged.back().mValues[i] = atom_getfloat(argv + i);
        mMessages.mTagged.back().mStringFlag = false;
        mMessages.mTagged.back().mCount = argc;
    }
    mMessages.mLock.release();
}

// String Helper

void FrameLib_MaxClass_FromMax::copyString(char *str, const char *toCopy)
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

// Main

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass_FromMax::makeClass<FrameLib_MaxClass_FromMax>(CLASS_BOX, "fl.frommax~");
}
