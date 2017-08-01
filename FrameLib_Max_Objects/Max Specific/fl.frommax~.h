
#include "FrameLib_DSP.h"
#include <FrameLib_MaxClass.h>

// FIX - memory management is horrible - don't memory manage use std::vector in the audio thread - replace this also
// TO DO - redo memory management and structs so they are less horrible and ugly

// Message Structs

struct MaxMessage
{
    struct TaggedMessages
    {
        char mTag[129];
        char mString[129];
        bool mStringFlag;
        double mValues[4096];
        unsigned long mCount;
    };
    
    MaxMessage() : mCount(0) { mValues.resize(4096); }
    
    FrameLib_SpinLock mLock;
    
    std::vector<double> mValues;
    unsigned long mCount;
    std::vector<TaggedMessages> mTagged;
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
    
    // String Helper

    void copyString(char *str, const char *toCopy);
    
    // Data (public so we can take the address)
    
    MaxMessage mMessages;
};
