
#include "FrameLib_DSP.h"
#include <FrameLib_MaxClass.h>
#include <algorithm>

// FIX - memory management is horrible - don't memory manage use std::vector in the audio thread - replace this also
// TO DO - redo memory management and structs so they are less horrible and ugly

// Message Structs

struct MaxMessage
{
    class List
    {
        
    public:
        
        List() : mSize(0) { mValues.resize(4096); }
        
        void set(t_atom *values, unsigned long size)
        {
            mSize = size > 4096 ? 4096 : size;
            for (unsigned long i = 0; i < mSize; i++)
                mValues[i] = atom_getfloat(values++);
        }
        
        void get(double *output, unsigned long size) const      { std::copy(mValues.begin(), mValues.begin() + (size > mSize ? mSize : size), output); }
        const double *get() const                               { return &mValues[0]; }
        unsigned long size() const                              { return mSize; }

    private:
        
        std::vector<double> mValues;
        unsigned long mSize;
    };
    
    // FIX - tags with multichannel expansion
    
    class TaggedMessages
    {
        
    public:
        
        void setTag(const char *str)    { copyString(mTag, str); }
        
        void set(const char *str)
        {
            mIsString = true;
            copyString(mString, str);
        }
        
        void set(t_atom *values, unsigned long size)
        {
            mIsString = false;
            mList.set(values, size);
        }
        
        size_t calcTaggedSize()
        {
            if (mIsString)
                return FrameLib_Parameters::Serial::calcSize(mTag, mString);
            else
                return FrameLib_Parameters::Serial::calcSize(mTag, mList.size());
        }
        
        void writeTagged(FrameLib_Parameters::Serial *serialisedParameters)
        {
            if (mIsString)
                serialisedParameters->write(mTag, mString);
            else
                serialisedParameters->write(mTag, mList.get(), mList.size());
        }
        
    private:
        
        static void copyString(char *str, const char *toCopy)
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
        
        
        char mTag[129];
        bool mIsString;
        char mString[129];
        List mList;
    };
    
    MaxMessage() : mTagsNeedClear(false) {}
    
    size_t calcTaggedSize()
    {
        size_t size = 0;
        
        for (unsigned long i = 0; i < mTagged.size() && !mTagsNeedClear; i++)
            size += mTagged[i].calcTaggedSize();
       
        return size;
    }
    
    void writeTagged(FrameLib_Parameters::Serial *serialisedParameters)
    {
        for (unsigned long i = 0; i < mTagged.size() && !mTagsNeedClear; i++)
            mTagged[i].writeTagged(serialisedParameters);
    }
    
    SpinLock mLock;
    
    List mList;
    std::vector<TaggedMessages> mTagged;
    bool mTagsNeedClear;
};

// Underlying FrameLib Class

class FrameLib_FromMax : public FrameLib_Processor
{
    // Parameter Info and Enums
    
    enum ParameterList { kMode };
    enum Modes { kValues, kParams };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_FromMax(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
        
    // Info
    
    std::string objectInfo(bool verbose);
    std::string inputInfo(unsigned long idx, bool verbose);
    std::string outputInfo(unsigned long idx, bool verbose);

private:
    
    void process();
    
// Data
    
    MaxMessage *mMessages;
    Modes mMode;
    
    static ParameterInfo sParamInfo;
};

// Max Class

class FrameLib_MaxClass_FromMax : public FrameLib_MaxClass<FrameLib_Expand<FrameLib_FromMax> >
{

public:
    
    // Class Initialisation
    
    static void classInit(t_class *c, t_symbol *nameSpace, const char *classname);
    
    // Constructor
    
    FrameLib_MaxClass_FromMax(t_symbol *s, long argc, t_atom *argv) : FrameLib_MaxClass(s, argc, argv) {}
    
    // Additional handlers
    
    void intHandler(t_atom_long in);
    void floatHandler(double in);
    void list(t_symbol *s, long argc, t_atom *argv);
    void anything(t_symbol *s, long argc, t_atom *argv);
    
    MaxMessage *getMessages() { return &mMessages; }
        
private:
    
    // Data
    
    MaxMessage mMessages;
};
