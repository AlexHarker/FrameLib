
#ifndef FRAMELIB_COMPLEXEXPRESSION_H
#define FRAMELIB_COMPLEXEXPRESSION_H

#include  <complex>

#include "FrameLib_DSP.h"
#include "FrameLib_ExprParser.h"

class FrameLib_ComplexExpression : public FrameLib_Block
{
    const static int kMaxIns = 32;
    
    // Parameter Enums and Info
    
    enum ParameterList { kExpression, kMismatchMode, kTriggers };
    enum MismatchModes { kWrap, kShrink, kExtend};
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

    // Internal Classes
    
    struct Parser : public FrameLib_ExprParser<std::complex<double> >
    {
        Parser();
    };
    
    class InputProcessor : public FrameLib_Processor
    {
        
    public:
        
        // Constructor
        
        InputProcessor(FrameLib_Context context, MismatchModes mode, const double *triggers, size_t triggersSize, unsigned long numIns);

        void copyVectorZeroWrap(double* output, const double *input, unsigned long sizeOut, unsigned long sizeIn, unsigned long sizeWrap);
        
    private:
        
        // Update and Process
        
        void process();
        
        // Dade
        
        MismatchModes mMode;
    };
    
    class ConstantOut : public FrameLib_Processor
    {

    public:
        
        // Constructor
        
        ConstantOut(FrameLib_Context context, MismatchModes mode, const double *triggers, size_t triggersSize, unsigned long numIns, std::complex<double> value);
        
    private:
        
        // Update and Process
        
        void process();
        
        // Data
        
        MismatchModes mMode;
        std::complex<double> mValue;
    };
    
public:
    
    // Constructor / Destructor
    
    FrameLib_ComplexExpression(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    ~FrameLib_ComplexExpression();

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
