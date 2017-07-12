
#include "FrameLib_DSP.h"

// FIX - memory management is horrible - don't memory manage use std::vector in the audio thread - replace this also
// TO DO - redo memory management and structs so they are less horrible and ugly

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
    MaxMessage() : mCount(0) { mValues.resize(4096); }
    
    FrameLib_SpinLock mLock;
    
    std::vector<double> mValues;
    unsigned long mCount;
    std::vector<TaggedMessages> mTagged;
};

// FIX - slightly nasty forward declarations...

class FrameLib_MaxClass_From;
MaxMessage *getMessages(FrameLib_MaxClass_From *object);

class FrameLib_FromMax : public FrameLib_Processor
{
    enum ParameterList {kMode};
    enum Modes {kValues, kParams};
    
public:
    
    FrameLib_FromMax(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, 1, 1)
    {
        mParameters.addEnum(kMode, "mode", 0);
        mParameters.addEnumItem(kValues, "values");
        mParameters.addEnumItem(kParams, "params");
        
        mParameters.set(serialisedParameters);
        
        mMode = (Modes) mParameters.getValue(kMode);
        
        outputMode(0, mMode == kValues ? kOutputNormal : kOutputTagged);
        
        mMessages = getMessages((FrameLib_MaxClass_From *)owner);
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
    
private:
    
    MaxMessage *mMessages;
    Modes mMode;
};

#include <FrameLib_MaxClass.h>

class FrameLib_MaxClass_From : public FrameLib_MaxClass<FrameLib_Expand<FrameLib_FromMax> >
{

public:
    
    FrameLib_MaxClass_From(t_symbol *s, long argc, t_atom *argv) : FrameLib_MaxClass(s, argc, argv) {}
    
    // Additional handlers
    
    void intHandler(t_atom_long in)
    {
        messages.mLock.acquire();
        messages.mValues[0] = in;
        messages.mCount = 1;
        messages.mLock.release();
    }
    
    void floatHandler(double in)
    {
        messages.mLock.acquire();
        messages.mValues[0] = in;
        messages.mCount = 1;
        messages.mLock.release();
    }
    
    void list(t_symbol *s, long argc, t_atom *argv)
    {
        argc = argc > 4096 ? 4096 : argc;
        
        messages.mLock.acquire();
        for (unsigned long i = 0; i < argc; i++)
            messages.mValues[i] = atom_getfloat(argv + i);
        messages.mCount = argc;
        messages.mLock.release();
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
    
    void anything(t_symbol *s, long argc, t_atom *argv)
    {
        argc = argc > 4096 ? 4096 : argc;
        
        messages.mLock.acquire();
        messages.mTagged.push_back(TaggedMessages());
        copyString(messages.mTagged.back().mTag, s->s_name);
        
        if (atom_gettype(argv) == A_SYM)
        {
            if (argc > 1)
                object_error(mUserObject, "too many arguments for string value");
            
            copyString(messages.mTagged.back().mString, atom_getsym(argv)->s_name);
            messages.mTagged.back().mStringFlag = TRUE;
        }
        else
        {
            for (unsigned long i = 0; i < argc; i++)
                messages.mTagged.back().mValues[i] = atom_getfloat(argv + i);
            messages.mTagged.back().mStringFlag = FALSE;
            messages.mTagged.back().mCount = argc;
        }
        messages.mLock.release();
    }
    
    // Class Initisation
    
    static void classInit(t_class *c, t_symbol *nameSpace, const char *classname)
    {
        FrameLib_MaxClass::classInit(c, nameSpace, classname);
        
        addMethod<FrameLib_MaxClass_From, &FrameLib_MaxClass_From::intHandler>(c, "int");
        addMethod<FrameLib_MaxClass_From, &FrameLib_MaxClass_From::floatHandler>(c, "float");
        addMethod<FrameLib_MaxClass_From, &FrameLib_MaxClass_From::list>(c, "list");
        addMethod<FrameLib_MaxClass_From, &FrameLib_MaxClass_From::anything>(c, "anything");
    }
    
    // Data (public so we can take the address)
    
    MaxMessage messages;
};

MaxMessage *getMessages(FrameLib_MaxClass_From *object)
{
    return &object->messages;
}

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass_From::makeClass<FrameLib_MaxClass_From>(CLASS_BOX, "fl.frommax~");
}
