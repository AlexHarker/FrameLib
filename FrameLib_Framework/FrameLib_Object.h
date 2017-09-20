
#ifndef FRAMELIB_BLOCK_H
#define FRAMELIB_BLOCK_H

#include "FrameLib_Types.h"
#include "FrameLib_Context.h"
#include "FrameLib_Parameters.h"
#include <string>
#include <sstream>
#include <iostream>

// FrameLib_Object

// This abstract template class outlines the basic functionality that objects (blocks / DSP / multichannel must provide)

template <class T>
class FrameLib_Object 
{
    
public:
    
    // Constructor / Destructor
    
    FrameLib_Object(ObjectType type, FrameLib_Context context)
    : mType(type), mContext(context), mFeedback(false), mNumIns(0), mNumOuts(0), mNumAudioChans(0) {}
    virtual ~FrameLib_Object() {}
   
    // Object Type
    
    ObjectType getType() const                  { return mType; }
    
    // Context
    
    const FrameLib_Context getContext() const   { return mContext; }
    
    // Feedback Detection
    
    bool getFeedback() const                    { return mFeedback; }
    void setFeedback(bool feedback)             { mFeedback = feedback; }
    
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
    
    virtual void blockUpdate(double **ins, double **outs, unsigned long blockSize) = 0;
    virtual void reset(double samplingRate, unsigned long maxBlockSize) = 0;
    
    // Return to host to request to be passed audio
    
    static bool handlesAudio()  { return false; }
    
    // Connections
    
    virtual ConnectionResult addConnection(T *object, unsigned long outIdx, unsigned long inIdx) = 0;
    virtual void deleteConnection(unsigned long inIdx) = 0;
    virtual void clearConnections() = 0;
    virtual bool isConnected(unsigned long inIdx) = 0;

    // Info

    virtual std::string objectInfo(bool verbose = false)                        { return "No object info available";  }
    virtual std::string inputInfo(unsigned long idx, bool verbose = false)      { return "No input info available";  }
    virtual std::string outputInfo(unsigned long idx, bool verbose = false)     { return "No output info available";  }
    virtual std::string audioInfo(unsigned long idx, bool verbose = false)      { return "No audio channel info available";  }
   
    virtual FrameType inputType(unsigned long idx) = 0;
    virtual FrameType outputType(unsigned long idx) = 0;
    
    // N.B. Parameter objects can be queried directly for info
    
    virtual const FrameLib_Parameters *getParameters()                          { return NULL;  }
    
protected:
    
    // Info Helpers
    
    const char *getInfo(const char *verboseStr, const char *briefStr, bool verbose)
    {
        return verbose ? verboseStr : briefStr;
    }
    
    std::string getInfo(const char *verboseStr, const char *briefStr, unsigned long idx, bool verbose)
    {
        std::string info = getInfo(verboseStr, briefStr, verbose);
        std::ostringstream idxStr;
        
        idxStr << (idx + 1);
        
        for (size_t pos = info.find("#", 0); pos != std::string::npos;  pos = info.find("#", pos + 1))
            info.replace(pos, 1, idxStr.str());
        
        return info;
    }
    
    std::string getInfo(const char *verboseStr, const char *briefStr, const char *replaceStr, bool verbose)
    {
        std::string info = getInfo(verboseStr, briefStr, verbose);
        
        for (size_t pos = info.find("#", 0); pos != std::string::npos;  pos = info.find("#", pos + 1))
            info.replace(pos, 1, replaceStr);
        
        return info;
    }

private:

    const ObjectType mType;
    FrameLib_Context mContext;
    
    bool mFeedback;
    
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
    
    FrameLib_Block(ObjectType type, FrameLib_Context context) : FrameLib_Object<FrameLib_Block>(type, context) {}
    virtual ~FrameLib_Block() {}

    // Connections
    
    virtual ConnectionResult addConnection(class FrameLib_DSP *object, unsigned long outIdx, unsigned long inIdx) = 0;
    
    virtual ConnectionResult addConnection(FrameLib_Block *object, unsigned long outIdx, unsigned long inIdx)
    {
        return addConnection(object->getOutputObject(outIdx), outIdx, inIdx);
    }
    
protected:
    
    virtual class FrameLib_DSP *getOutputObject(unsigned long outIdx) = 0;
};

#endif
