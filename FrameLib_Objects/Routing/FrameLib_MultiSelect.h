
#ifndef FRAMELIB_MULTISELECT_H
#define FRAMELIB_MULTISELECT_H

#include "FrameLib_DSP.h"

class FrameLib_MultiSelect : public FrameLib_Block
{
    // Internal valve class
    
    class Select : public FrameLib_Processor
    {
        // Parameter Enums and Info
        
        enum ParameterList { kActiveIn };
        
    public:
        
        // Constructor
        
        Select(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner, long numIns, long num);

    private:
        
        // Update and Process
        
        void update();
        void process();
        
        // Data
        
        long mNumIns;
        long mActiveIn;
    };
    
    // Parameter Enums and Info
    
    enum ParameterList { kNumIns, kNumOuts, kActiveIn1 };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };
    
public:
    
    // Constructor / Destructor
    
    FrameLib_MultiSelect(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    ~FrameLib_MultiSelect();
    
    // Object Type
    
    static ObjectType getType() { return kProcessor; }
    
    // Info
    
    std::string objectInfo(bool verbose);
    std::string inputInfo(unsigned long idx, bool verbose);
    std::string outputInfo(unsigned long idx, bool verbose);
    
    // Connection Types
    
    virtual FrameType inputType(unsigned long idx) const    { return kFrameAny; }
    virtual FrameType outputType(unsigned long idx) const   { return kFrameAny; }
    
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
    
    // Data

    long mNumIns;
    long mNumOuts;
    
    std::vector<Select *> mSelects;
    FrameLib_Parameters mParameters;
    static ParameterInfo sParamInfo;
};

#endif
