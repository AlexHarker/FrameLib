
#ifndef FRAMELIB_BLOCK_H
#define FRAMELIB_BLOCK_H

#include "FrameLib_Attributes.h"

// FrameLib_Block

// This abstract class provides a connectivity interface to FrameLib_DSP objects or blocks (groups of FrameLib_DSP objects).
// Standard objects inherit this in the FrameLib_DSP class.
// Objects that have asynchronous outputs can use this class to host multiple FrameLib_DSP objects and connect them correctly.

class FrameLib_Block
{
    
public:
    
    // Constructor / Destructor (virtual)
    
    FrameLib_Block() {}
    virtual ~FrameLib_Block() {}
   
    // Basic Setup / IO Queries
    
    virtual void setSamplingRate(double samplingRate) = 0;
    
    virtual unsigned long getNumIns() = 0;
    virtual unsigned long getNumOuts() = 0;
    virtual unsigned long getNumAudioIns() = 0;
    virtual unsigned long getNumAudioOuts() = 0;
    
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

    FrameLib_Attributes mAttributes;
};

#endif
