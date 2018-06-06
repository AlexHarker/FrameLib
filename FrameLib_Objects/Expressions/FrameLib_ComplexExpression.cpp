
#include "FrameLib_ComplexExpression.h"

#include "../Complex_Unary/FrameLib_Complex_Unary_Template.h"
#include "../Complex_Binary/FrameLib_Complex_Binary_Template.h"

// Parser Class

// Function/Operator Templates

template <typename Op>
struct UnaryOperation : public OpBase<std::complex<double> >
{
    typedef std::complex<double> complex;
    
    UnaryOperation(const char* name, int precedence = 0) : OpBase(name, precedence) {}
    
    int numItems() const                                    { return 1; }
    complex call(complex a, complex b, complex c) const     { return Op()(a); }
    
    FrameLib_DSP *create(FrameLib_Context context) const
    {
        return new FrameLib_Complex_UnaryOp<Op>(context, NULL, NULL);
    }
};

template <typename Op>
struct BinaryOperation : public OpBase<std::complex<double> >
{
    typedef std::complex<double> complex;

    BinaryOperation(const char* name, int precedence = 0) : OpBase(name, precedence) {}
    
    int numItems() const                                    { return 2; }
    complex call(complex a, complex b, complex c) const     { return Op()(a, b); }
    
    FrameLib_DSP *create(FrameLib_Context context) const
    {
        FrameLib_Parameters::AutoSerial serialiedParameters;
        serialiedParameters.write("mismatch", "wrap");
        return new FrameLib_Complex_BinaryOp<Op>(context, &serialiedParameters, NULL);
    }
};

static std::complex<double> negate(const std::complex<double>& a) { return -a; }

FrameLib_ComplexExpression::Parser::Parser() : FrameLib_ExprParser(3)
{
    // Default Return Constant

    setDefaultConstant(std::complex<double>(std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN()));
    
    // Constants
    
    addConstant("i", std::complex<double>(0.0, 1.0));
    addConstant("e", M_E);
    addConstant("pi", M_PI);
    addConstant("epsilon", std::numeric_limits<double>::epsilon());
    addConstant("inf", std::numeric_limits<double>::infinity());
    
    // Operators
    
    addOperator(new UnaryOperation<Complex_Unary_Functor<negate> >("-", 0));
    
    addOperator(new BinaryOperation<std::divides<std::complex<double> > >("/", 1));
    addOperator(new BinaryOperation<std::multiplies<std::complex<double> > >("*", 1));
    
    addOperator(new BinaryOperation<std::plus<std::complex<double> > >("+", 2));
    addOperator(new BinaryOperation<std::minus<std::complex<double> > >("-", 2));
    
    // Functions
    
    addFunction(new UnaryOperation<Complex_Unary_Functor<std::sin> >("sin"));
    addFunction(new UnaryOperation<Complex_Unary_Functor<std::cos> >("cos"));
    addFunction(new UnaryOperation<Complex_Unary_Functor<std::tan> >("tan"));
    addFunction(new UnaryOperation<Complex_Unary_Functor<std::sinh> >("sinh"));
    addFunction(new UnaryOperation<Complex_Unary_Functor<std::cosh> >("cosh"));
    addFunction(new UnaryOperation<Complex_Unary_Functor<std::tanh> >("tanh"));
    addFunction(new UnaryOperation<Complex_Unary_Functor<std::asin> >("asin"));
    addFunction(new UnaryOperation<Complex_Unary_Functor<std::acos> >("acos"));
    addFunction(new UnaryOperation<Complex_Unary_Functor<std::atan> >("atan"));
    addFunction(new UnaryOperation<Complex_Unary_Functor<std::asinh> >("asinh"));
    addFunction(new UnaryOperation<Complex_Unary_Functor<std::acosh> >("acosh"));
    addFunction(new UnaryOperation<Complex_Unary_Functor<std::atanh> >("atanh"));
    
    addFunction(new UnaryOperation<Complex_Unary_Functor<std::log> >("log"));
    addFunction(new UnaryOperation<Complex_Unary_Functor<std::log10> >("log10"));
    addFunction(new UnaryOperation<Complex_Unary_Functor<std::exp> >("exp"));
    
    addFunction(new UnaryOperation<Complex_Unary_Functor<std::conj> >("conj"));

    addFunction(new UnaryOperation<Complex_Unary_Functor<std::sqrt> >("sqrt"));
    
    addFunction(new BinaryOperation<Complex_Binary_Functor<std::pow> >("pow"));
}

// Input Processor Class

// Constructor

void FrameLib_ComplexExpression::InputProcessor::process()
{
    unsigned long sizeInR = 0;
    unsigned long sizeInI = 0;
    unsigned long sizeOut = 0;
    unsigned long sizeIn;
    
    getInput(0, &sizeInR);
    getInput(1, &sizeInI);
    
    sizeOut = std::max(sizeInR, sizeInI);
    
    for (unsigned long i = 1; i < getNumIns() / 2; i++)
    {
        getInput(i * 2 + 0, &sizeInR);
        getInput(i * 2 + 1, &sizeInI);
        
        sizeOut = std::min(std::max(sizeInR, sizeInI), sizeOut);
    }
    
    for (unsigned long i = 0 ; i < getNumIns(); i++)
        requestOutputSize(i, sizeOut);
    
    if (allocateOutputs())
    {
        for (unsigned long i = 0 ; i < getNumIns(); i++)
        {
            const double *input = getInput(i, &sizeIn);
            double *output = getOutput(i, &sizeOut);
            unsigned long copySize = std::min(sizeIn, sizeOut);
            copyVector(output, input, copySize);
            zeroVector(output + copySize, sizeOut - copySize);
        }
    }
}

// Constant Out Class

void FrameLib_ComplexExpression::ConstantOut::process()
{
    unsigned long sizeInR = 0;
    unsigned long sizeInI = 0;
    unsigned long sizeOut = 0;
    
    getInput(0, &sizeInR);
    getInput(1, &sizeInI);
    
    sizeOut = std::max(sizeInR, sizeInI);
    
    for (unsigned long i = 1; i < getNumIns() / 2; i++)
    {
        getInput(i * 2 + 0, &sizeInR);
        getInput(i * 2 + 1, &sizeInI);
        
        sizeOut = std::min(std::max(sizeInR, sizeInI), sizeOut);
    }
    
    requestOutputSize(0, sizeOut);
    requestOutputSize(1, sizeOut);
    
    if (allocateOutputs())
    {
        std::fill_n(getOutput(0, &sizeOut), sizeOut, mValue.real());
        std::fill_n(getOutput(1, &sizeOut), sizeOut, mValue.imag());
    }
}

// Main Class

// Constructor

FrameLib_ComplexExpression::FrameLib_ComplexExpression(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_Block(kProcessor, context, proxy), mInputProcessor(NULL), mParameters(&sParamInfo)
{
    typedef Graph<std::complex<double> > Graph;
    typedef FrameLib_Block::Connection Connection;
    
    mParameters.addString(kExpression, "expr", 0);
    mParameters.setInstantiation();
    
    mParameters.set(serialisedParameters);
    
    Graph graph;
    Parser parser;
    ExprParseError error = parser.parse(graph, mParameters.getString(kExpression));

    if (graph.mNumInputs > 32)
        graph = Graph();
    
    setIO(graph.mNumInputs * 2, 2);
    
    if (!error && graph.mOperations.size())
    {
        // Create and Input Processor
        
        mInputProcessor = new InputProcessor(context, graph.mNumInputs);

        // Alias the inputs to the input processor
        
        for (unsigned long i = 0 ; i < graph.mNumInputs * 2; i++)
            mInputProcessor->setInputAlias(Connection(this, i), i);
        
        mFixedInputNode = mInputProcessor;
        
        // Build the graph if there is one

        for (std::vector<Graph::Operation>::iterator it = graph.mOperations.begin(); it != graph.mOperations.end(); it++)
        {
            FrameLib_DSP* operation = it->mOp->create(context);
            
            for (int i = 0 ; i < 3 && it->mIns[i].mType != kInputUnused; i++)
            {
                if (it->mIns[i].mType == kInputConstant)
                {
                    double real = it->mIns[i].mValue.real();
                    double imag = it->mIns[i].mValue.imag();

                    operation->setFixedInput(i * 2 + 0, &real, 1);
                    operation->setFixedInput(i * 2 + 1, &imag, 1);
                }
                else if (it->mIns[i].mType == kInputVariable)
                {
                    operation->addConnection(Connection(mInputProcessor, it->mIns[i].mIndex * 2 + 0), i * 2 + 0);
                    operation->addConnection(Connection(mInputProcessor, it->mIns[i].mIndex * 2 + 1), i * 2 + 1);
                }
                else
                {
                    operation->addConnection(Connection(mGraph[it->mIns[i].mIndex], 0), i * 2 + 0);
                    operation->addConnection(Connection(mGraph[it->mIns[i].mIndex], 1), i * 2 + 1);
                }
            }
            
            mGraph.push_back(operation);
        }
        
        mGraph.back()->setOutputAlias(Connection(this, 0), 0);
        mGraph.back()->setOutputAlias(Connection(this, 1), 1);
    }
    else
    {
        // Build the graph if the result is constant (including an invalid expression)

        mGraph.push_back(new ConstantOut(context, graph.mNumInputs, graph.mConstant));
        for (long i = 0; i < graph.mNumInputs * 2; i++)
            mGraph.back()->setInputAlias(Connection(this, i), i);
        mGraph.back()->setOutputAlias(Connection(this, 0), 0);
        mGraph.back()->setOutputAlias(Connection(this, 1), 1);

        mFixedInputNode = mGraph.back();
    }
}

FrameLib_ComplexExpression::~FrameLib_ComplexExpression()
{
    if (mInputProcessor)
        delete mInputProcessor;
    
    for (std::vector<FrameLib_DSP *>::iterator it = mGraph.begin(); it != mGraph.end(); it++)
        delete (*it);
    
    mGraph.clear();
}

// Info

std::string FrameLib_ComplexExpression::objectInfo(bool verbose)
{
    return formatInfo("Applies a mathematical expression to complex pairs of inputs: ",
                      "Applies a mathematical expression to complex pairs of inputs", verbose);
}

std::string FrameLib_ComplexExpression::inputInfo(unsigned long idx, bool verbose)
{
    if ((idx % 2) == 0)
        return formatInfo("Real Input Frame #", "Real Input Frame #", idx / 2, verbose);
    else
        return formatInfo("Imag Input Frame #", "Imag Input Frame #", idx / 2, verbose);
}

std::string FrameLib_ComplexExpression::outputInfo(unsigned long idx, bool verbose)
{
    switch (idx)
    {
        case 0: return "Real Output Frame";
        case 1: return "Imag Output Frame";
    }
    
    return "Unknown Output";
}

// Parameter Info

FrameLib_ComplexExpression::ParameterInfo FrameLib_ComplexExpression::sParamInfo;

FrameLib_ComplexExpression::ParameterInfo::ParameterInfo()
{
    add("Sets the mathematical expression.");
    add("Sets the number of inputs.");
}

// Reset

void FrameLib_ComplexExpression::reset(double samplingRate, unsigned long maxBlockSize)
{
    if (mInputProcessor)
        mInputProcessor->reset(samplingRate, maxBlockSize);
    
    for (std::vector<FrameLib_DSP *>::iterator it = mGraph.begin(); it != mGraph.end(); it++)
        (*it)->reset(samplingRate, maxBlockSize);
}
