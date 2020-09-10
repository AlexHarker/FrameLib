
#ifndef FRAMELIB_COMPLEXEXPRESSION_H
#define FRAMELIB_COMPLEXEXPRESSION_H

#include "FrameLib_DSP.h"
#include "FrameLib_ExprParser.h"

#include <complex>

class FrameLib_ComplexExpression : public FrameLib_Block
{
    const static int kMaxIns = 32;
    
    // Parameter Enums and Info
    
    enum ParameterList { kExpression, kMismatchMode, kTriggers };
    enum MismatchModes { kWrap, kShrink, kExtend};
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

    // Internal Classes
    
    struct Parser : public FrameLib_ExprParser::Parser<std::complex<double>>
    {
        Parser();
    };
    
    class InputProcessor final : public FrameLib_Processor
    {
    public:
        
        // Constructor
        
        InputProcessor(FrameLib_Context context, MismatchModes mode, const double *triggers, unsigned long triggersSize, unsigned long numIns);

        void copyVectorZeroWrap(double* output, const double *input, unsigned long sizeOut, unsigned long sizeIn, unsigned long sizeWrap);
        
    private:
        
        // Update and Process
        
        void process() override;
        
        // Dade
        
        MismatchModes mMode;
    };
    
    class ConstantOut final : public FrameLib_Processor
    {

    public:
        
        // Constructor
        
        ConstantOut(FrameLib_Context context, const double *triggers, unsigned long triggersSize, unsigned long numIns, std::complex<double> value);
        
    private:
        
        // Update and Process
        
        void process() override;
        
        // Data
        
        std::complex<double> mValue;
    };
    
public:
    
    static constexpr ObjectType sType = kProcessor;
    static constexpr bool sHandlesAudio = false;
    
    // Constructor
    
    FrameLib_ComplexExpression(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
 
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;
    
    // Connection Types
    
    FrameType inputType(unsigned long idx) const override   { return kFrameNormal; }
    FrameType outputType(unsigned long idx) const override  { return kFrameNormal; }
    
    // Fixed inputs are dealt with either by the input processor or by the constant object
    
    void setFixedInput(unsigned long idx, const double *input, unsigned long size) override     { mFixedInputNode->setFixedInput(idx, input, size); }
    const double *getFixedInput(unsigned long idx, unsigned long *size) override                { return mFixedInputNode->getFixedInput(idx, size); }
    
    // Audio Processing
    
    uint64_t getBlockTime() const override { return 0; }
    void blockUpdate(const double * const *ins, double **outs, unsigned long blockSize, FrameLib_AudioQueue& notifier) override {}
    void blockUpdate(const double * const *ins, double **outs, unsigned long blockSize) override {}
    void reset(double samplingRate, unsigned long maxBlockSize) override;
    
    const FrameLib_Parameters *getParameters() const override { return &mParameters; }
    
    // Ordering Connections
    
    void autoOrderingConnections() override {};
    void clearAutoOrderingConnections() override {};
    
private:
    
    // Data
    
    FrameLib_DSP *mFixedInputNode;
    std::unique_ptr<FrameLib_DSP> mInputProcessor;
    FrameLib_OwnedList<FrameLib_DSP> mGraph;
    
    static ParameterInfo sParamInfo;
    FrameLib_Parameters mParameters;
};


#endif
