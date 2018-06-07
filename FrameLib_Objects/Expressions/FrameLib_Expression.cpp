
#include "FrameLib_Expression.h"

#include "../Unary/FrameLib_Unary_Template.h"
#include "../Binary/FrameLib_Binary_Template.h"
#include "../Ternary/FrameLib_Ternary_Template.h"
#include "../Ternary/FrameLib_Ternary_Objects.h"

// Parser Class

// Function/Operator Templates

template <typename Op>
struct UnaryOperation : public OpBase<double>
{
    UnaryOperation(const char* name, int precedence = 0) : OpBase(name, precedence) {}
    
    int numItems() const                                { return 1; }
    double call(double a, double b, double c) const     { return Op()(a); }
    
    FrameLib_DSP *create(FrameLib_Context context) const
    {
        return new FrameLib_UnaryOp<Op>(context, NULL, NULL);
    }
};

template <typename Op>
struct BinaryOperation : public OpBase<double>
{
    BinaryOperation(const char* name, int precedence = 0) : OpBase(name, precedence) {}
    
    int numItems() const                                { return 2; }
    double call(double a, double b, double c) const     { return Op()(a, b); }
    
    FrameLib_DSP *create(FrameLib_Context context) const
    {
        FrameLib_Parameters::AutoSerial serialiedParameters;
        serialiedParameters.write("mismatch", "wrap");
        return new FrameLib_BinaryOp<Op>(context, &serialiedParameters, NULL);
    }
};

template <typename Op>
struct TernaryOperation : public OpBase<double>
{
    TernaryOperation(const char* name, int precedence = 0) : OpBase(name, precedence) {}
    
    virtual int numItems() const                        { return 3; }
    double call(double a, double b, double c) const     { return Op()(a, b, c); }
    
    FrameLib_DSP *create(FrameLib_Context context) const
    {
        FrameLib_Parameters::AutoSerial serialiedParameters;
        serialiedParameters.write("mismatch", "extend");
        return new FrameLib_TernaryOp<Op>(context, &serialiedParameters, NULL);
    }
};

static double negate(double a) { return -a; }

FrameLib_Expression::Parser::Parser() : FrameLib_ExprParser(7)
{
    // Default Return Constant
    
    setDefaultConstant(std::numeric_limits<double>::quiet_NaN());
    
    // Constants
    
    addConstant("e", M_E);
    addConstant("pi", M_PI);
    addConstant("epsilon", std::numeric_limits<double>::epsilon());
    addConstant("inf", std::numeric_limits<double>::infinity());
    
    // Operators
    
    addOperator(new UnaryOperation<std::logical_not<double>  >("!", 0));
    addOperator(new UnaryOperation<Unary_Functor<negate> >("-", 0));
    
    addOperator(new BinaryOperation<std::divides<double> >("/", 1));
    addOperator(new BinaryOperation<std::multiplies<double> >("*", 1));
    addOperator(new BinaryOperation<Binary_Functor<fmod> >("%", 1));
    
    addOperator(new BinaryOperation<std::plus<double> >("+", 2));
    addOperator(new BinaryOperation<std::minus<double> >("-", 2));
    
    addOperator(new BinaryOperation<std::greater<double> >(">", 3));
    addOperator(new BinaryOperation<std::less<double> >("<", 3));
    addOperator(new BinaryOperation<std::greater_equal<double> >(">=", 3));
    addOperator(new BinaryOperation<std::less_equal<double> >("<=", 3));
    
    addOperator(new BinaryOperation<std::equal_to<double> >("==", 4));
    addOperator(new BinaryOperation<std::not_equal_to<double> >("!=", 4));
    
    addOperator(new BinaryOperation<std::logical_and<double> >("&&", 5));
    
    addOperator(new BinaryOperation<std::logical_or<double> >("||", 6));
    
    // Functions
    
    addFunction(new UnaryOperation<Unary_Functor<sin> >("sin"));
    addFunction(new UnaryOperation<Unary_Functor<cos> >("cos"));
    addFunction(new UnaryOperation<Unary_Functor<tan> >("tan"));
    addFunction(new UnaryOperation<Unary_Functor<sinh> >("sinh"));
    addFunction(new UnaryOperation<Unary_Functor<cosh> >("cosh"));
    addFunction(new UnaryOperation<Unary_Functor<tanh> >("tanh"));
    addFunction(new UnaryOperation<Unary_Functor<asin> >("asin"));
    addFunction(new UnaryOperation<Unary_Functor<acos> >("acos"));
    addFunction(new UnaryOperation<Unary_Functor<atan> >("atan"));
    addFunction(new UnaryOperation<Unary_Functor<asinh> >("asinh"));
    addFunction(new UnaryOperation<Unary_Functor<acosh> >("acosh"));
    addFunction(new UnaryOperation<Unary_Functor<atanh> >("atanh"));
    
    addFunction(new UnaryOperation<Unary_Functor<log> >("log"));
    addFunction(new UnaryOperation<Unary_Functor<log2> >("log2"));
    addFunction(new UnaryOperation<Unary_Functor<log10> >("log10"));
    addFunction(new UnaryOperation<Unary_Functor<exp> >("exp"));
    addFunction(new UnaryOperation<Unary_Functor<exp2> >("exp2"));
    
    addFunction(new UnaryOperation<Unary_Functor<fabs> >("abs"));
    addFunction(new UnaryOperation<Unary_Functor<ceil> >("ceil"));
    addFunction(new UnaryOperation<Unary_Functor<floor> >("floor"));
    addFunction(new UnaryOperation<Unary_Functor<round> >("round"));
    addFunction(new UnaryOperation<Unary_Functor<trunc> >("trunc"));
    
    addFunction(new UnaryOperation<Unary_Functor<sqrt> >("sqrt"));
    addFunction(new UnaryOperation<Unary_Functor<cbrt> >("cbrt"));
    addFunction(new UnaryOperation<Unary_Functor<erf> >("erf"));
    addFunction(new UnaryOperation<Unary_Functor<erfc> >("erfc"));
    
    addFunction(new BinaryOperation<Binary_Functor<pow> >("pow"));
    addFunction(new BinaryOperation<Binary_Functor<hypot> >("hypot"));
    addFunction(new BinaryOperation<Binary_Functor<fmin> >("min"));
    addFunction(new BinaryOperation<Binary_Functor<fmax> >("max"));
    addFunction(new BinaryOperation<Binary_Functor<fdim> >("diff"));
    
    addFunction(new TernaryOperation<Ternary_Functor<Ternary::clip> >("clip"));
    addFunction(new TernaryOperation<Ternary_Functor<Ternary::wrap> >("wrap"));
    addFunction(new TernaryOperation<Ternary_Functor<Ternary::fold> >("fold"));
}

// Input Processor Class

// Constructor

void FrameLib_Expression::InputProcessor::process()
{
    unsigned long sizeMin, sizeMax, sizeIn, sizeOut;
    
    getInput(0, &sizeMin);
    getInput(0, &sizeMax);

    for (unsigned long i = 1; i < getNumIns(); i++)
    {
        getInput(i, &sizeIn);
        sizeMin = std::min(sizeIn, sizeMin);
        sizeMax = std::max(sizeIn, sizeMax);
    }
    
    sizeOut = sizeMin ? (mMode == kShrink ? sizeMin : sizeMax) : 0;
    
    for (unsigned long i = 0 ; i < getNumIns(); i++)
        requestOutputSize(i, sizeOut);
    
    if (allocateOutputs())
    {
        switch (mMode)
        {
            // N.B. - these can call the same routine as the size is already shrunk for KShrink so no wrapping will take place
                
            case kWrap:
            case kShrink:
                for (unsigned long i = 0 ; i < getNumIns(); i++)
                {
                    const double *input = getInput(i, &sizeIn);
                    double *output = getOutput(i, &sizeOut);
                    copyVectorWrap(output, input, sizeOut, sizeIn);
                }
                break;
                
            case kExtend:
                for (unsigned long i = 0 ; i < getNumIns(); i++)
                {
                    const double *input = getInput(i, &sizeIn);
                    double *output = getOutput(i, &sizeOut);
                    copyVectorExtend(output, input, sizeOut, sizeIn);
                }
                break;
        }
    }
}

// Constant Out Class

void FrameLib_Expression::ConstantOut::process()
{
    unsigned long sizeMin, sizeMax, sizeIn, sizeOut;
    
    getInput(0, &sizeMin);
    getInput(0, &sizeMax);
    
    for (unsigned long i = 1; i < getNumIns(); i++)
    {
        getInput(i, &sizeIn);
        sizeMin = std::min(sizeIn, sizeMin);
        sizeMax = std::max(sizeIn, sizeMax);
    }
    
    sizeOut = sizeMin ? (mMode == kShrink ? sizeMin : sizeMax) : 0;
    
    if (allocateOutputs())
        std::fill_n(getOutput(0, &sizeOut), sizeOut, mValue);
}

// Main Class

// Constructor

FrameLib_Expression::FrameLib_Expression(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_Block(kProcessor, context, proxy), mInputProcessor(NULL), mParameters(&sParamInfo)
{
    typedef Graph<double> Graph;
    typedef FrameLib_Block::Connection Connection;
    
    mParameters.addString(kExpression, "expr", 0);
    mParameters.setInstantiation();
    
    mParameters.addEnum(kMismatchMode, "mismatch");
    mParameters.addEnumItem(kWrap, "wrap");
    mParameters.addEnumItem(kShrink, "shrink");
    mParameters.addEnumItem(kExtend, "extend");
    mParameters.setInstantiation();

    mParameters.set(serialisedParameters);
    
    MismatchModes mode = static_cast<MismatchModes>(mParameters.getInt(kMismatchMode));
    
    Graph graph;
    Parser parser;
    ExprParseError error = parser.parse(graph, mParameters.getString(kExpression));

    if (graph.mNumInputs > 32)
        graph = Graph();
    
    setIO(graph.mNumInputs, 1);
    
    if (!error && graph.mOperations.size())
    {
        // Create and Input Processor
        
        mInputProcessor = new InputProcessor(context, mode, graph.mNumInputs);

        // Alias the inputs to the input processor
        
        for (unsigned long i = 0 ; i < graph.mNumInputs; i++)
            mInputProcessor->setInputAlias(Connection(this, i), i);
        
        mFixedInputNode = mInputProcessor;
        
        // Build the graph if there is one

        for (std::vector<Graph::Operation>::iterator it = graph.mOperations.begin(); it != graph.mOperations.end(); it++)
        {
            FrameLib_DSP* operation = it->mOp->create(context);
            
            for (int i = 0 ; i < 3 && it->mIns[i].mType != kInputUnused; i++)
            {
                if (it->mIns[i].mType == kInputConstant)
                    operation->setFixedInput(i, &it->mIns[i].mValue, 1);
                else if (it->mIns[i].mType == kInputVariable)
                    operation->addConnection(Connection(mInputProcessor, it->mIns[i].mIndex), i);
                else
                    operation->addConnection(Connection(mGraph[it->mIns[i].mIndex], 0), i);
            }
            
            mGraph.push_back(operation);
        }
        
        mGraph.back()->setOutputAlias(Connection(this, 0), 0);
    }
    else
    {
        // Build the graph if the result is constant (including an invalid expression)

        mGraph.push_back(new ConstantOut(context, mode, graph.mNumInputs, graph.mConstant));
        for (long i = 0; i < graph.mNumInputs; i++)
            mGraph.back()->setInputAlias(Connection(this, i), i);
        mGraph.back()->setOutputAlias(Connection(this, 0), 0);
        
        mFixedInputNode = mGraph.back();
    }
}

FrameLib_Expression::~FrameLib_Expression()
{
    if (mInputProcessor)
        delete mInputProcessor;
    
    for (std::vector<FrameLib_DSP *>::iterator it = mGraph.begin(); it != mGraph.end(); it++)
        delete (*it);
    
    mGraph.clear();
}

// Info

std::string FrameLib_Expression::objectInfo(bool verbose)
{
    return formatInfo("Applies a mathematical expression to the inputs: ",
                      "Applies a mathematical expression to the inputs", verbose);
}

std::string FrameLib_Expression::inputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Input Frame #", "Input Frame #", idx, verbose);
}

std::string FrameLib_Expression::outputInfo(unsigned long idx, bool verbose)
{
    return "Output Frame";
}

// Parameter Info

FrameLib_Expression::ParameterInfo FrameLib_Expression::sParamInfo;

FrameLib_Expression::ParameterInfo::ParameterInfo()
{
    add("Sets the mathematical expression.");
    add("Sets the number of inputs.");
}

// Reset

void FrameLib_Expression::reset(double samplingRate, unsigned long maxBlockSize)
{
    if (mInputProcessor)
        mInputProcessor->reset(samplingRate, maxBlockSize);
    
    for (std::vector<FrameLib_DSP *>::iterator it = mGraph.begin(); it != mGraph.end(); it++)
        (*it)->reset(samplingRate, maxBlockSize);
}
