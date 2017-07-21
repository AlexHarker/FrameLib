
#ifndef FRAMELIB_BLOCK_H
#define FRAMELIB_BLOCK_H

#include "FrameLib_Parameters.h"

// FrameLib_Block

// This abstract class provides a connectivity interface to FrameLib_DSP objects or blocks (groups of FrameLib_DSP objects).
// Standard objects inherit this in the FrameLib_DSP class.
// Objects that have asynchronous outputs can use this class to host multiple FrameLib_DSP objects and connect them correctly.

class FrameLib_Block
{
    
public:
 
    // IO Count Structure
    
    struct IOCount
    {
        IOCount() : mNumIns(0), mNumOuts(0), mNumAudioIns(0), mNumAudioOuts(0) {}
        
        IOCount(unsigned long nIns, unsigned long nOuts, unsigned long nAudioIns, unsigned long nAudioOuts)
        : mNumIns(nIns), mNumOuts(nOuts), mNumAudioIns(nAudioIns), mNumAudioOuts(nAudioOuts) {}
        
        unsigned long mNumIns, mNumOuts, mNumAudioIns, mNumAudioOuts;
    };
    
    // Constructor / Destructor (virtual)
    
    FrameLib_Block() {}
    virtual ~FrameLib_Block() {}
   
    // Basic Setup / IO Queries
    
    virtual void setSamplingRate(double samplingRate) = 0;
    
    void setIO(const IOCount& count)        { mIOCount = count; }
    
    unsigned long getNumIns()               { return mIOCount.mNumIns; }
    unsigned long getNumOuts()              { return mIOCount.mNumOuts; }
    unsigned long getNumAudioIns()          { return mIOCount.mNumAudioIns; }
    unsigned long getNumAudioOuts()         { return mIOCount.mNumAudioOuts; }
    
    // Set Fixed Inputs
    
    virtual void setFixedInput(unsigned long idx, double *input, unsigned long size) = 0;

    // Audio Processing

    // Override to handle audio at the block level (reset called with the audio engine resets)
    
    virtual void blockUpdate(double **ins, double **outs, unsigned long vecSize) = 0;
    virtual void reset() = 0;
    
    // Return to host to request to be passed audio
    
    static bool handlesAudio()  { return false; }
    
    // Connections
    
    virtual void deleteConnection(unsigned long inIdx) = 0;
    virtual void addConnection(class FrameLib_DSP *object, unsigned long outIdx, unsigned long inIdx) = 0;
    virtual void clearConnections() = 0;
    virtual bool isConnected(unsigned long inIdx) = 0;
    
    void addConnection(FrameLib_Block *object, unsigned long outIdx, unsigned long inIdx)
    {
        addConnection(object->getOutputObject(outIdx), outIdx, inIdx);
    }
    
protected:

    virtual class FrameLib_DSP *getOutputObject(unsigned long outIdx) = 0;
    
private:

    IOCount mIOCount;
};

#endif
