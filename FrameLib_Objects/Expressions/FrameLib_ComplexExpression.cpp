
#include "FrameLib_ComplexExpression.h"
#include <cmath>

#include "../Complex_Unary/FrameLib_Complex_Unary_Template.h"
#include "../Complex_Binary/FrameLib_Complex_Binary_Template.h"

// Specialisations to allow implicit multiples of i for complex numbers

template<>
const char *FrameLib_ExprParser::Parser<std::complex<double>>::extentNumber(const char *expr)
{
    if (isDigit(*expr))
    {
        while (isDigit(*expr)) expr++;
        if (isPeriod(*expr)) for (expr++; isDigit(*expr); expr++);
        if (*expr == 'i' && !isLetter(*(expr + 1)))
            expr++;
    }
    
    return expr;
}

template<>
std::complex<double> FrameLib_ExprParser::Parser<std::complex<double>>::convertTextToNumber(const char* text)
{
    if (text[strlen(text) - 1] == 'i')
    {
        std::string numberText(text, strlen(text) - 1);
        return std::complex<double>(0.0, std::stod(numberText));
    }
        
    return std::stod(text);
}

// Parser Class

// Function/Operator Templates

template <typename Op>
struct UnaryOperation final : public FrameLib_ExprParser::OpBase<std::complex<double>>
{
    typedef std::complex<double> complex;
    
    UnaryOperation(const char* name, int precedence = 0) : OpBase(name, precedence) {}
    
    size_t numItems() const override                                { return 1; }
    complex call(complex a, complex b, complex c) const override    { return Op()(a); }
    
    FrameLib_DSP *create(FrameLib_Context context) const override
    {
        return new FrameLib_Complex_UnaryOp<Op>(context, nullptr, nullptr);
    }
};

template <typename Op>
struct BinaryOperation final : public FrameLib_ExprParser::OpBase<std::complex<double>>
{
    typedef std::complex<double> complex;

    BinaryOperation(const char* name, int precedence = 0) : OpBase(name, precedence) {}
    
    size_t numItems() const override                                { return 2; }
    complex call(complex a, complex b, complex c) const override    { return Op()(a, b); }
    
    FrameLib_DSP *create(FrameLib_Context context) const override
    {
        FrameLib_Parameters::AutoSerial serialiedParameters;
        serialiedParameters.write("mismatch", "wrap");
        return new FrameLib_Complex_BinaryOp<Op>(context, &serialiedParameters, nullptr);
    }
};

static std::complex<double> negate(const std::complex<double>& a) { return -a; }

FrameLib_ComplexExpression::Parser::Parser() : FrameLib_ExprParser::Parser<std::complex<double>>(3)
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
    
    addOperator(new UnaryOperation<Complex_Unary_Functor<negate>>("-", 0));
    
    addOperator(new BinaryOperation<std::divides<std::complex<double>>>("/", 1));
    addOperator(new BinaryOperation<std::multiplies<std::complex<double>>>("*", 1));
    
    addOperator(new BinaryOperation<std::plus<std::complex<double>>>("+", 2));
    addOperator(new BinaryOperation<std::minus<std::complex<double>>>("-", 2));
    
    // Functions
    
    addFunction(new UnaryOperation<Complex_Unary_Functor<std::sin>>("sin"));
    addFunction(new UnaryOperation<Complex_Unary_Functor<std::cos>>("cos"));
    addFunction(new UnaryOperation<Complex_Unary_Functor<std::tan>>("tan"));
    addFunction(new UnaryOperation<Complex_Unary_Functor<std::sinh>>("sinh"));
    addFunction(new UnaryOperation<Complex_Unary_Functor<std::cosh>>("cosh"));
    addFunction(new UnaryOperation<Complex_Unary_Functor<std::tanh>>("tanh"));
    addFunction(new UnaryOperation<Complex_Unary_Functor<std::asin>>("asin"));
    addFunction(new UnaryOperation<Complex_Unary_Functor<std::acos>>("acos"));
    addFunction(new UnaryOperation<Complex_Unary_Functor<std::atan>>("atan"));
    addFunction(new UnaryOperation<Complex_Unary_Functor<std::asinh>>("asinh"));
    addFunction(new UnaryOperation<Complex_Unary_Functor<std::acosh>>("acosh"));
    addFunction(new UnaryOperation<Complex_Unary_Functor<std::atanh>>("atanh"));
    
    addFunction(new UnaryOperation<Complex_Unary_Functor<std::log>>("log"));
    addFunction(new UnaryOperation<Complex_Unary_Functor<std::log10>>("log10"));
    addFunction(new UnaryOperation<Complex_Unary_Functor<std::exp>>("exp"));
    
    addFunction(new UnaryOperation<Complex_Unary_Functor<std::conj>>("conj"));

    addFunction(new UnaryOperation<Complex_Unary_Functor<std::sqrt>>("sqrt"));
    
    addFunction(new BinaryOperation<Complex_Binary_Functor<std::pow>>("pow"));
}

// Input Processor Class

// Constructor

FrameLib_ComplexExpression::InputProcessor::InputProcessor(FrameLib_Context context, MismatchModes mode, const double *triggers, unsigned long triggersSize, unsigned long numIns)
: FrameLib_Processor(context, nullptr, nullptr, numIns * 2, numIns * 2)
, mMode(mode)
{
    for (unsigned long i = 0; i < numIns; i++)
    {
        setInputMode(i + 0, false, (i < triggersSize) && triggers[i], false);
        setInputMode(i + 1, false, (i < triggersSize) && triggers[i], false);
    }
}

// Copying with wrap and zeroing

void FrameLib_ComplexExpression::InputProcessor::copyVectorZeroWrap(double* output, const double *input, unsigned long sizeOut, unsigned long sizeIn, unsigned long sizeWrap)
{
    unsigned long excess = sizeOut % sizeWrap;
    unsigned long excessIn = std::min(excess, sizeIn);
    
    for (unsigned long i = 0; i < (sizeOut - excess); i += sizeWrap)
    {
        copyVector(output + i, input, sizeIn);
        zeroVector(output + i + sizeIn, sizeWrap - sizeIn);
    }
    
    copyVector(output + (sizeOut - excess), input, excessIn);
    zeroVector(output + (sizeOut - (excess - excessIn)), excess - excessIn);
}

// Process

void FrameLib_ComplexExpression::InputProcessor::process()
{
    unsigned long sizeIn, sizeOut, sizeMin, sizeMax, sizeInR, sizeInI;
    
    getInput(0, &sizeInR);
    getInput(1, &sizeInI);
    
    sizeMin = sizeMax = std::max(sizeInR, sizeInI);
    
    for (unsigned long i = 1; i < getNumIns() / 2; i++)
    {
        getInput(i * 2 + 0, &sizeInR);
        getInput(i * 2 + 1, &sizeInI);
        sizeIn = std::max(sizeInR, sizeInI);
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
                for (unsigned long i = 0 ; i < getNumIns() / 2; i++)
                {
                    const double *inputR = getInput(i * 2 + 0, &sizeInR);
                    const double *inputI = getInput(i * 2 + 1, &sizeInI);
                    double *outputR = getOutput(i * 2 + 0, &sizeOut);
                    double *outputI = getOutput(i * 2 + 1, &sizeOut);
                    
                    sizeIn = std::max(sizeInR, sizeInI);
                    copyVectorZeroWrap(outputR, inputR, sizeOut, sizeInR, sizeIn);
                    copyVectorZeroWrap(outputI, inputI, sizeOut, sizeInI, sizeIn);
                }
                break;
                
            case kShrink:
                for (unsigned long i = 0 ; i < getNumIns() / 2; i++)
                {
                    const double *inputR = getInput(i * 2 + 0, &sizeInR);
                    const double *inputI = getInput(i * 2 + 1, &sizeInI);
                    double *outputR = getOutput(i * 2 + 0, &sizeOut);
                    double *outputI = getOutput(i * 2 + 1, &sizeOut);
                    
                    copyVectorZero(outputR, inputR, sizeOut, sizeInR);
                    copyVectorZero(outputI, inputI, sizeOut, sizeInI);
                }
                break;
                
            case kExtend:
                for (unsigned long i = 0 ; i < getNumIns() / 2; i++)
                {
                    const double *inputR = getInput(i * 2 + 0, &sizeInR);
                    const double *inputI = getInput(i * 2 + 1, &sizeInI);
                    double *outputR = getOutput(i * 2 + 0, &sizeOut);
                    double *outputI = getOutput(i * 2 + 1, &sizeOut);
                    
                    if (sizeInR < sizeInI)
                        copyVectorZero(outputR, inputR, sizeOut, sizeInR);
                    else
                        copyVectorExtend(outputR, inputR, sizeOut, sizeInR);
                    
                    if (sizeInI < sizeInR)
                        copyVectorZero(outputI, inputI, sizeOut, sizeInI);
                    else
                        copyVectorExtend(outputI, inputI, sizeOut, sizeInI);
                }
                break;
        }
    }
}

// Constant Out Class

// Constructor

FrameLib_ComplexExpression::ConstantOut::ConstantOut(FrameLib_Context context, const double *triggers, unsigned long triggersSize, unsigned long numIns, std::complex<double> value)
: FrameLib_Processor(context, nullptr, nullptr, numIns * 2, 2)
, mValue(value)
{
    for (unsigned long i = 0; i < numIns; i++)
    {
        setInputMode(i + 0, false, (i < triggersSize) && triggers[i], false);
        setInputMode(i + 1, false, (i < triggersSize) && triggers[i], false);
    }
}

// Process

void FrameLib_ComplexExpression::ConstantOut::process()
{
    unsigned long sizeOut;
    
    requestOutputSize(0, 1);
    requestOutputSize(1, 1);
    
    if (allocateOutputs())
    {
        getOutput(0, &sizeOut)[0] = mValue.real();
        getOutput(1, &sizeOut)[0] = mValue.imag();
    }
}

// Main Class

// Constructor

FrameLib_ComplexExpression::FrameLib_ComplexExpression(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Block(ObjectType::Processor, context, proxy)
, mParameters(context, proxy, &sParamInfo)
{
    typedef FrameLib_ExprParser::Graph<std::complex<double>> Graph;
    typedef FrameLib_Block::Connection Connection;
    using namespace FrameLib_ExprParser;
    
    mParameters.addString(kExpression, "expression", 0);
    mParameters.setInstantiation();
    
    mParameters.setInstantiation();
    
    mParameters.addEnum(kMismatchMode, "mismatch");
    mParameters.addEnumItem(kWrap, "wrap");
    mParameters.addEnumItem(kShrink, "shrink");
    mParameters.addEnumItem(kExtend, "extend");
    mParameters.setInstantiation();
    
    mParameters.addVariableBoolArray(kTriggers, "trigger_ins", true, maxNumIns, maxNumIns);
    mParameters.setInstantiation();

    mParameters.set(serialisedParameters);
  
    const double *triggers = mParameters.getArray(kTriggers);
    unsigned long triggersSize = mParameters.getArraySize(kTriggers);
    
    MismatchModes mode = mParameters.getEnum<MismatchModes>(kMismatchMode);

    Graph graph;
    Parser parser;
    ExprParseError error = parser.parse(graph, mParameters.getString(kExpression), getReporter(), proxy);
    
    if (graph.mNumInputs > maxNumIns)
    {
        getReporter()(ErrorSource::Object, proxy, "expression has more than the maximum number of inputs (#)", maxNumIns);
        graph = Graph();
    }
    
    setIO(graph.mNumInputs * 2, 2);
    
    if (!error && graph.mOperations.size())
    {
        // Create and Input Processor
        
        mInputProcessor.reset(new InputProcessor(context, mode, triggers, triggersSize, graph.mNumInputs));

        // Alias the inputs to the input processor
        
        for (long i = 0 ; i < graph.mNumInputs * 2; i++)
            mInputProcessor->setInputAlias(Connection(this, i), i);
        
        mFixedInputNode = mInputProcessor.get();
        
        // Build the graph if there is one

        for (auto it = graph.mOperations.begin(); it != graph.mOperations.end(); it++)
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
                    operation->addConnection(Connection(mInputProcessor.get(), it->mIns[i].mIndex * 2 + 0), i * 2 + 0);
                    operation->addConnection(Connection(mInputProcessor.get(), it->mIns[i].mIndex * 2 + 1), i * 2 + 1);
                }
                else
                {
                    operation->addConnection(Connection(mGraph[it->mIns[i].mIndex].get(), 0), i * 2 + 0);
                    operation->addConnection(Connection(mGraph[it->mIns[i].mIndex].get(), 1), i * 2 + 1);
                }
            }
            
            mGraph.add(operation);
        }
        
        mGraph.back()->setOutputAlias(Connection(this, 0), 0);
        mGraph.back()->setOutputAlias(Connection(this, 1), 1);
    }
    else
    {
        // Build the graph if the result is constant (including an invalid expression)

        mGraph.add(new ConstantOut(context, triggers, triggersSize, graph.mNumInputs, graph.mConstant));
        for (long i = 0; i < graph.mNumInputs * 2; i++)
            mGraph.back()->setInputAlias(Connection(this, i), i);
        mGraph.back()->setOutputAlias(Connection(this, 0), 0);
        mGraph.back()->setOutputAlias(Connection(this, 1), 1);

        mFixedInputNode = mGraph.back().get();
    }
}

// Info

std::string FrameLib_ComplexExpression::objectInfo(bool verbose)
{
    return formatInfo("Calculates the result of a mathematical expression involving complex numbers: "
                      "Inputs and output are split into pairs of real and imaginary values. "
                      "If input pairs are mismatched then the shorter input is padded with zeros. "
                      "The expression can use input values, operators, functions and constants. "
                      "The output is a frame at least as long as the shortest input pair. "
                      "If input pairs mismatch in length the result depends on the mismatch parameter. "
                      "Pre-computable elements are detected and reduced to constants. "
                      "Thus it is efficient to synthesize constants or represent values precisely.",
                      "Calculates the result of a mathematical expression involving complex numbers", verbose);
}

std::string FrameLib_ComplexExpression::inputInfo(unsigned long idx, bool verbose)
{
    if ((idx % 2) == 0)
        return formatInfo("Real Input #", idx / 2);
    else
        return formatInfo("Imaginary Input #", idx / 2);
}

std::string FrameLib_ComplexExpression::outputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return "Imaginary Output";
    else
        
        return "Real Output";
}

// Parameter Info

FrameLib_ComplexExpression::ParameterInfo FrameLib_ComplexExpression::sParamInfo;

FrameLib_ComplexExpression::ParameterInfo::ParameterInfo()
{
    add("Sets the mathematical expression. "
        "Input pairs are indicated in1 to inx (where x is the number of input pairs created). "
        "Complex numbers can be expressed directly in the form (a + bi). "
        "A range of other constants, operators and functions are available: "
        "Constants { i e pi epsilon inf }. "
        "Unary Operators { - }. "
        "Binary Operators { / * % + }. "
        "Trigonometric Functions { sin cos tan asin acos atan }. "
        "Hyperbolic Functions { sinh cosh tanh asinh acosh atanh }. "
        "Unary Functions { log log10 exp conj sqrt }. "
        "Binary Functions { pow }.");
    add("Sets the mode used when dealing with mismatched input lengths: "
        "wrap - smaller input pairs are read modulo against the longest input pair. "
        "shrink - the output length is set to that of the shortest input pair. "
        "extend - if required input pairs are extended by repeating their final value.");
    add("Sets which input pairs trigger output using an array of bools. "
        "By default all inputs trigger output.");
}

// Reset

void FrameLib_ComplexExpression::reset(double samplingRate, unsigned long maxBlockSize)
{
    if (mInputProcessor)
        mInputProcessor->reset(samplingRate, maxBlockSize);
    
    for (auto it = mGraph.begin(); it != mGraph.end(); it++)
        (*it)->reset(samplingRate, maxBlockSize);
}
