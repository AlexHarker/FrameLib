
#ifndef FRAMELIB_EXPRESSION_H
#define FRAMELIB_EXPRESSION_H

#include "FrameLib_DSP.h"
#include "FrameLib_ExprParser.h"

class FrameLib_Expression : public FrameLib_Block
{
    struct Parser : public FrameLib_ExprParser<double>
    {
        Parser();
    };
    
    class InputProcessor : public FrameLib_Processor
    {
        
    public:
        
        // Constructor
        
        InputProcessor(FrameLib_Context context, unsigned long numIns)
         : FrameLib_Processor(context, NULL, NULL, numIns, numIns) {}
        
    private:
        
        // Update and Process
        
        void process();
    };
    
    class ConstantOut : public FrameLib_Processor
    {

    public:
        
        // Constructor
        
        ConstantOut(FrameLib_Context context, unsigned long numIns, double value)
        : FrameLib_Processor(context, NULL, NULL, numIns, 1), mValue(value) {}
        
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
    
    // Fixed inputs are dealt with either by the input processor or by the constant object
    
    virtual void setFixedInput(unsigned long idx, double *input, unsigned long size)    { mFixedInputNode->setFixedInput(idx, input, size); }
    virtual const double *getFixedInput(unsigned long idx, unsigned long *size)         { return mFixedInputNode->getFixedInput(idx, size); }
    
    // Audio Processing
    
    virtual void blockUpdate(const double * const *ins, double **outs, unsigned long blockSize) {}
    virtual void reset(double samplingRate, unsigned long maxBlockSize);
    
    virtual const FrameLib_Parameters *getParameters() const { return &mParameters; }
    
    // Ordering Connections
    
    virtual void autoOrderingConnections() {};
    virtual void clearAutoOrderingConnections() {};
    
private:
    
    // Data
    
    FrameLib_DSP *mFixedInputNode;
    FrameLib_DSP *mInputProcessor;
    std::vector<FrameLib_DSP *> mGraph;
    
    static ParameterInfo sParamInfo;
    FrameLib_Parameters mParameters;
};


#endif
