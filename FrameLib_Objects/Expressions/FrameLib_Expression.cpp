
#include "FrameLib_Expression.h"
#include <cmath>

#include "../Unary/FrameLib_Unary_Template.h"
#include "../Binary/FrameLib_Binary_Template.h"
#include "../Binary/FrameLib_Binary_Objects.h"
#include "../Ternary/FrameLib_Ternary_Template.h"
#include "../Ternary/FrameLib_Ternary_Objects.h"

// Parser Class

// Function/Operator Templates

template <typename Op>
struct UnaryOperation final : public FrameLib_ExprParser::OpBase<double>
{
    UnaryOperation(const char* name, int precedence = 0) : OpBase(name, precedence) {}
    
    size_t numItems() const override                             { return 1; }
    double call(double a, double b, double c) const override     { return Op()(a); }
    
    FrameLib_DSP *create(FrameLib_Context context) const override
    {
        return new FrameLib_UnaryOp<Op>(context, nullptr, nullptr);
    }
};

template <typename Op>
struct BinaryOperation final : public FrameLib_ExprParser::OpBase<double>
{
    BinaryOperation(const char* name, int precedence = 0) : OpBase(name, precedence) {}
    
    size_t numItems() const override                            { return 2; }
    double call(double a, double b, double c) const override    { return Op()(a, b); }
    
    FrameLib_DSP *create(FrameLib_Context context) const override
    {
        FrameLib_Parameters::AutoSerial serialiedParameters;
        serialiedParameters.write("mismatch", "wrap");
        return new FrameLib_BinaryOp<Op>(context, &serialiedParameters, nullptr);
    }
};

template <typename Op>
struct TernaryOperation final : public FrameLib_ExprParser::OpBase<double>
{
    TernaryOperation(const char* name, int precedence = 0) : OpBase(name, precedence) {}
    
    size_t numItems() const override                            { return 3; }
    double call(double a, double b, double c) const override    { return Op()(a, b, c); }
    
    FrameLib_DSP *create(FrameLib_Context context) const override
    {
        FrameLib_Parameters::AutoSerial serialiedParameters;
        serialiedParameters.write("mismatch", "extend");
        return new FrameLib_TernaryOp<Op>(context, &serialiedParameters, nullptr);
    }
};

static double negate(double a) { return -a; }

FrameLib_Expression::Parser::Parser() : FrameLib_ExprParser::Parser<double>(7)
{
    // Default Return Constant
    
    setDefaultConstant(std::numeric_limits<double>::quiet_NaN());
    
    // Constants
    
    addConstant("e", M_E);
    addConstant("pi", M_PI);
    addConstant("epsilon", std::numeric_limits<double>::epsilon());
    addConstant("inf", std::numeric_limits<double>::infinity());
    
    // Operators
    
    addOperator(new UnaryOperation<std::logical_not<double>>("!", 0));
    addOperator(new UnaryOperation<Unary_Functor<negate>>("-", 0));
    
    addOperator(new BinaryOperation<std::divides<double>>("/", 1));
    addOperator(new BinaryOperation<std::multiplies<double>>("*", 1));
    addOperator(new BinaryOperation<Binary_Functor<fmod>>("%", 1));
    
    addOperator(new BinaryOperation<std::plus<double>>("+", 2));
    addOperator(new BinaryOperation<std::minus<double>>("-", 2));
    
    addOperator(new BinaryOperation<std::greater<double>>(">", 3));
    addOperator(new BinaryOperation<std::less<double>>("<", 3));
    addOperator(new BinaryOperation<std::greater_equal<double>>(">=", 3));
    addOperator(new BinaryOperation<std::less_equal<double>>("<=", 3));
    
    addOperator(new BinaryOperation<std::equal_to<double>>("==", 4));
    addOperator(new BinaryOperation<std::not_equal_to<double>>("!=", 4));
    
    addOperator(new BinaryOperation<std::logical_and<double>>("&&", 5));
    
    addOperator(new BinaryOperation<std::logical_or<double>>("||", 6));
    
    // Functions
    
    addFunction(new UnaryOperation<Unary_Functor<sin>>("sin"));
    addFunction(new UnaryOperation<Unary_Functor<cos>>("cos"));
    addFunction(new UnaryOperation<Unary_Functor<tan>>("tan"));
    addFunction(new UnaryOperation<Unary_Functor<sinh>>("sinh"));
    addFunction(new UnaryOperation<Unary_Functor<cosh>>("cosh"));
    addFunction(new UnaryOperation<Unary_Functor<tanh>>("tanh"));
    addFunction(new UnaryOperation<Unary_Functor<asin>>("asin"));
    addFunction(new UnaryOperation<Unary_Functor<acos>>("acos"));
    addFunction(new UnaryOperation<Unary_Functor<atan>>("atan"));
    addFunction(new UnaryOperation<Unary_Functor<asinh>>("asinh"));
    addFunction(new UnaryOperation<Unary_Functor<acosh>>("acosh"));
    addFunction(new UnaryOperation<Unary_Functor<atanh>>("atanh"));
    
    addFunction(new UnaryOperation<Unary_Functor<log>>("log"));
    addFunction(new UnaryOperation<Unary_Functor<log2>>("log2"));
    addFunction(new UnaryOperation<Unary_Functor<log10>>("log10"));
    addFunction(new UnaryOperation<Unary_Functor<exp>>("exp"));
    addFunction(new UnaryOperation<Unary_Functor<exp2>>("exp2"));
    
    addFunction(new UnaryOperation<Unary_Functor<fabs>>("abs"));
    addFunction(new UnaryOperation<Unary_Functor<ceil>>("ceil"));
    addFunction(new UnaryOperation<Unary_Functor<floor>>("floor"));
    addFunction(new UnaryOperation<Unary_Functor<round>>("round"));
    addFunction(new UnaryOperation<Unary_Functor<trunc>>("trunc"));
    
    addFunction(new UnaryOperation<Unary_Functor<sqrt>>("sqrt"));
    addFunction(new UnaryOperation<Unary_Functor<cbrt>>("cbrt"));
    addFunction(new UnaryOperation<Unary_Functor<erf>>("erf"));
    addFunction(new UnaryOperation<Unary_Functor<erfc>>("erfc"));
    
    addFunction(new BinaryOperation<Binary_Functor<pow>>("pow"));
    addFunction(new BinaryOperation<Binary_Functor<hypot>>("hypot"));
    addFunction(new BinaryOperation<Binary_Functor<fmin>>("min"));
    addFunction(new BinaryOperation<Binary_Functor<fmax>>("max"));
    addFunction(new BinaryOperation<FrameLib_Binary_Ops::absDiff>("diff"));
    
    addFunction(new TernaryOperation<Ternary_Functor<FrameLib_Ternary_Ops::clip>>("clip"));
    addFunction(new TernaryOperation<Ternary_Functor<FrameLib_Ternary_Ops::wrap>>("wrap"));
    addFunction(new TernaryOperation<Ternary_Functor<FrameLib_Ternary_Ops::fold>>("fold"));
}

// Input Processor Class

// Constructor

FrameLib_Expression::InputProcessor::InputProcessor(FrameLib_Context context, MismatchModes mode, const double *triggers, unsigned long triggersSize, unsigned long numIns) : FrameLib_Processor(context, nullptr, nullptr, numIns, numIns), mMode(mode)
{
    for (unsigned long i = 0; i < numIns; i++)
        setInputMode(i, false, (i < triggersSize) && triggers[i], false);
}

// Process

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

// Constructor

FrameLib_Expression::ConstantOut::ConstantOut(FrameLib_Context context, const double *triggers, unsigned long triggersSize, unsigned long numIns, double value) : FrameLib_Processor(context, nullptr, nullptr, numIns, 1), mValue(value)
{
    for (unsigned long i = 0; i < numIns; i++)
        setInputMode(i, false, (i < triggersSize) && triggers[i], false);
}

// Process

void FrameLib_Expression::ConstantOut::process()
{
    unsigned long sizeOut;
    
    requestOutputSize(0, 1);
    
    if (allocateOutputs())
        getOutput(0, &sizeOut)[0] = mValue;
}

// Main Class

// Constructor

FrameLib_Expression::FrameLib_Expression(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_Block(kProcessor, context, proxy), mParameters(context, proxy, &sParamInfo)
{
    typedef FrameLib_ExprParser::Graph<double> Graph;
    typedef FrameLib_Block::Connection Connection;
    using namespace FrameLib_ExprParser;

    mParameters.addString(kExpression, "expr", 0);
    mParameters.setInstantiation();
    
    mParameters.addEnum(kMismatchMode, "mismatch");
    mParameters.addEnumItem(kWrap, "wrap");
    mParameters.addEnumItem(kShrink, "shrink");
    mParameters.addEnumItem(kExtend, "extend");
    mParameters.setInstantiation();

    mParameters.addVariableBoolArray(kTriggers, "trigger_ins", true, kMaxIns, kMaxIns);
    mParameters.setInstantiation();

    mParameters.set(serialisedParameters);
    
    MismatchModes mode = static_cast<MismatchModes>(mParameters.getInt(kMismatchMode));
    
    const double *triggers = mParameters.getArray(kTriggers);
    unsigned long triggersSize = mParameters.getArraySize(kTriggers);
    
    Graph graph;
    Parser parser;
    ExprParseError error = parser.parse(graph, mParameters.getString(kExpression), getReporter(), proxy);
    
    if (graph.mNumInputs > kMaxIns)
    {
        getReporter()(kErrorObject, proxy, "expression has more than the maximum number of inputs (#)", kMaxIns);
        graph = Graph();
    }
    
    setIO(graph.mNumInputs, 1);
        
    if (!error && graph.mOperations.size())
    {
        // Create and Input Processor
        
        mInputProcessor.reset(new InputProcessor(context, mode, triggers, triggersSize, graph.mNumInputs));

        // Alias the inputs to the input processor
        
        for (long i = 0 ; i < graph.mNumInputs; i++)
            mInputProcessor->setInputAlias(Connection(this, i), i);
        
        mFixedInputNode = mInputProcessor.get();
        
        // Build the graph if there is one

        for (auto it = graph.mOperations.begin(); it != graph.mOperations.end(); it++)
        {
            FrameLib_DSP* operation = it->mOp->create(context);
            
            for (int i = 0 ; i < 3 && it->mIns[i].mType != kInputUnused; i++)
            {
                if (it->mIns[i].mType == kInputConstant)
                    operation->setFixedInput(i, &it->mIns[i].mValue, 1);
                else if (it->mIns[i].mType == kInputVariable)
                    operation->addConnection(Connection(mInputProcessor.get(), it->mIns[i].mIndex), i);
                else
                    operation->addConnection(Connection(mGraph[it->mIns[i].mIndex].get(), 0), i);
            }
            
            mGraph.add(operation);
        }
        
        mGraph.back()->setOutputAlias(Connection(this, 0), 0);
    }
    else
    {
        // Build the graph if the result is constant (including an invalid expression)

        mGraph.add(new ConstantOut(context, triggers, triggersSize, graph.mNumInputs, graph.mConstant));
        for (long i = 0; i < graph.mNumInputs; i++)
            mGraph.back()->setInputAlias(Connection(this, i), i);
        mGraph.back()->setOutputAlias(Connection(this, 0), 0);
        
        mFixedInputNode = mGraph.back().get();
    }
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
    
    for (auto it = mGraph.begin(); it != mGraph.end(); it++)
        (*it)->reset(samplingRate, maxBlockSize);
}
