
#ifndef FRAMELIB_ROUTE_H
#define FRAMELIB_ROUTE_H

#include "FrameLib_DSP.h"

class FrameLib_Route : public FrameLib_Block
{
    // Internal valve class
    
    class Valve : public FrameLib_Processor
    {
        // Parameter Enums and Info
        
        enum ParameterList { kActiveValve };
        
    public:
        
        // Constructor
        
        Valve(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner, long num);

    private:
        
        // Update and Process
        
        void update();
        void process();
        
        // Data
        
        long mValveNumber;
        long mActiveValve;
    };
    
    // Parameter Enums and Info
    
    enum ParameterList { kNumOuts, kActiveOut };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };
    
public:
    
    // Constructor / Destructor
    
    FrameLib_Route(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    ~FrameLib_Route();
    
    // Object Type
    
    static ObjectType getType() { return kProcessor; }
    
    // Info
    
    std::string objectInfo(bool verbose);
    std::string inputInfo(unsigned long idx, bool verbose);
    std::string outputInfo(unsigned long idx, bool verbose);
    
    // Connection Types
    
    virtual FrameType inputType(unsigned long idx) const    { return kFrameAny; }
    virtual FrameType outputType(unsigned long idx) const   { return kFrameAny; }
    
    // Connection Queries
    
    virtual unsigned long getNumInputObjects(unsigned long blockIdx)                        { return mNumOuts; }
    virtual DSPConnection getInputConnection(unsigned long blockIdx, unsigned long idx)     { return DSPConnection(mValves[idx], blockIdx); }
    
    virtual DSPConnection getOutputConnection(unsigned long blockIdx)                       { return DSPConnection(mValves[blockIdx], 0); }
    
    virtual unsigned long getNumOrderingConnectionObjects()                                 { return 0; }
    virtual FrameLib_DSP *getOrderingConnectionObject(unsigned long idx)                    { return NULL; }
    
    // N.B. - Nothing can be acheived by setting a fixed input, so ignore this
    
    virtual void setFixedInput(unsigned long idx, double *input, unsigned long size) {}
    
    // Audio Processing
    
    virtual void blockUpdate(double **ins, double **outs, unsigned long blockSize) {}
    virtual void reset(double samplingRate, unsigned long maxBlockSize);
    
    virtual const FrameLib_Parameters *getParameters() const                                { return &mParameters; }

    // Ordering Connections
    
    virtual void autoOrderingConnections() {};
    virtual void clearAutoOrderingConnections() {};
    
private:
    
    virtual void connectionUpdate(Queue *queue) {}

    // Data
    
    long mNumOuts;
    
    std::vector<Valve *> mValves;
    FrameLib_Parameters mParameters;
    static ParameterInfo sParamInfo;
};

#endif
