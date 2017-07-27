
#ifndef FRAMELIB_BLOCK_H
#define FRAMELIB_BLOCK_H

#include "FrameLib_Types.h"
#include "FrameLib_Parameters.h"
#include "FrameLib_Info.h"

// FrameLib_Object

// This abstract template class outlines the basic functionality that objects (blocks / DSP / multichannel must provide)

template <class T>
class FrameLib_Object
{
    
public:
     
    // Constructor / Destructor
    
    FrameLib_Object(ObjectType type) : mType(type), mNumIns(0), mNumOuts(0), mNumAudioChans(0) {}
    virtual ~FrameLib_Object() {}
   
    // Object Type
    
    ObjectType getType()    { return mType; }
    
    // Basic IO Setup / Queries
    
    void setIO(unsigned long nIns, unsigned long nOuts, unsigned long nAudioChans = 0)
    {
        mNumIns = (getType() == kScheduler || nIns) ? nIns : 1;
        mNumOuts = nOuts;
        mNumAudioChans = nAudioChans;
    }
    
    unsigned long getNumIns()               { return mNumIns; }
    unsigned long getNumOuts()              { return mNumOuts; }
    unsigned long getNumAudioIns()          { return getType() != kOutput ? mNumAudioChans : 0; }
    unsigned long getNumAudioOuts()         { return getType() == kOutput ? mNumAudioChans : 0; }
    unsigned long getNumAudioChans()        { return mNumAudioChans; }
    
    // Set Fixed Inputs
    
    virtual void setFixedInput(unsigned long idx, double *input, unsigned long size) = 0;

    // Audio Processing

    // Override to handle audio at the block level (reset called with the audio engine resets)
    
    virtual void blockUpdate(double **ins, double **outs, unsigned long vecSize) = 0;
    virtual void reset(double samplingRate) = 0;
    
    // Return to host to request to be passed audio
    
    static bool handlesAudio()  { return false; }
    
    // Connections
    
    virtual void deleteConnection(unsigned long inIdx) = 0;
    virtual void addConnection(T *object, unsigned long outIdx, unsigned long inIdx) = 0;
    virtual void clearConnections() = 0;
    virtual bool isConnected(unsigned long inIdx) = 0;

    // Info

    virtual const char *objectInfo(bool verbose = false)                        { return "No object info available";  }
    virtual const char *inputInfo(unsigned long idx, bool verbose = false)      { return "No input info available";  }
    virtual const char *outputInfo(unsigned long idx, bool verbose = false)     { return "No output info available";  }
    virtual const char *audioInfo(unsigned long idx, bool verbose = false)      { return "No audio channel info available";  }
   
    virtual FrameType inputType(unsigned long idx) = 0;
    virtual FrameType outputType(unsigned long idx) = 0;
    
    // N.B. Parameter objects can be queried directly for info
    
    virtual const FrameLib_Parameters *getParameters()                          { return NULL;  }
    
private:

    const ObjectType mType;
    
    unsigned long mNumIns;
    unsigned long mNumOuts;
    unsigned long mNumAudioChans;
};

// FrameLib_Block

// This abstract class provides a connectivity interface to FrameLib_DSP objects or blocks (groups of FrameLib_DSP objects).
// Standard objects inherit this in the FrameLib_DSP class.
// Objects that have asynchronous outputs can use this class to host multiple FrameLib_DSP objects and connect them correctly.

class FrameLib_Block : public FrameLib_Object<FrameLib_Block>
{
    
public:
    
    // Constructor / Destructor
    
    FrameLib_Block(ObjectType type) : FrameLib_Object<FrameLib_Block>(type) {}
    virtual ~FrameLib_Block() {}

    // Connections
    
    virtual void addConnection(class FrameLib_DSP *object, unsigned long outIdx, unsigned long inIdx) = 0;
    
    virtual void addConnection(FrameLib_Block *object, unsigned long outIdx, unsigned long inIdx)
    {
        addConnection(object->getOutputObject(outIdx), outIdx, inIdx);
    }
    
protected:
    
    virtual class FrameLib_DSP *getOutputObject(unsigned long outIdx) = 0;
};

#endif
