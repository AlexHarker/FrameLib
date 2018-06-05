
#ifndef FRAMELIB_EXPRESSION_H
#define FRAMELIB_EXPRESSION_H

#include "FrameLib_DSP.h"

class FrameLib_Expression : public FrameLib_Block
{
    class ConstantOut : public FrameLib_Processor
    {

    public:
        
        // Constructor
        
        ConstantOut(FrameLib_Context context, unsigned long numIns, double value);
        
    private:
        
        // Update and Process
        
        void process();
        
        // Data
        
        double mValue;
    };
    
    // Parameter Enums and Info
    
    enum ParameterList { kExpression };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };
    
public:
    
    // Constructor / Destructor
    
    FrameLib_Expression(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    ~FrameLib_Expression();

    // Object Type
    
    static ObjectType getType() { return kProcessor; }
    
    // Info
    
    std::string objectInfo(bool verbose);
    std::string inputInfo(unsigned long idx, bool verbose);
    std::string outputInfo(unsigned long idx, bool verbose);
    
    // Connection Types
    
    virtual FrameType inputType(unsigned long idx) const    { return kFrameNormal; }
    virtual FrameType outputType(unsigned long idx) const   { return kFrameNormal; }
    
    // N.B. - Nothing can be acheived by setting a fixed input, so ignore this
    
    virtual void setFixedInput(unsigned long idx, double *input, unsigned long size) {}
    virtual const double *getFixedInput(unsigned long idx, unsigned long *size) { return getEmptyFixedInput(idx, size); }
    
    // Audio Processing
    
    virtual void blockUpdate(const double * const *ins, double **outs, unsigned long blockSize) {}
    virtual void reset(double samplingRate, unsigned long maxBlockSize);
    
    virtual const FrameLib_Parameters *getParameters() const { return &mParameters; }
    
    // Ordering Connections
    
    virtual void autoOrderingConnections() {};
    virtual void clearAutoOrderingConnections() {};
    
private:
    
    // Data
    
    std::vector<FrameLib_DSP *> mGraph;
    
    static ParameterInfo sParamInfo;
    FrameLib_Parameters mParameters;
};


#endif
