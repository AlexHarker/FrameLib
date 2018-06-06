
#include "FrameLib_ExpressionGraph.h"

// Parser Class

static double negate(double a) { return -a; }

FrameLib_Expression::Parser::Parser() : FrameLib_ExprParser(7)
{
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
    unsigned long sizeIn = 0;
    unsigned long sizeOut = 0;
    
    getInput(0, &sizeOut);
    
    for (unsigned long i = 1; i < getNumIns(); i++)
    {
        getInput(i, &sizeIn);
        sizeOut = std::min(sizeIn, sizeOut);
    }
    
    for (unsigned long i = 0 ; i < getNumIns(); i++)
        requestOutputSize(i, sizeOut);
    
    if (allocateOutputs())
    {
        for (unsigned long i = 0 ; i < getNumIns(); i++)
        {
            const double *input = getInput(i, &sizeIn);
            double *output = getOutput(i, &sizeOut);
            copyVector(output, input, sizeOut);
        }
    }
}

// Constant Out Class

void FrameLib_Expression::ConstantOut::process()
{
    unsigned long sizeIn = 0;
    unsigned long sizeOut = 0;
    
    getInput(0, &sizeOut);
    
    for (unsigned long i = 1; i < getNumIns(); i++)
    {
        getInput(i, &sizeIn);
        sizeOut = std::min(sizeIn, sizeOut);
    }
    
    requestOutputSize(0, sizeOut);
    
    if (allocateOutputs())
        std::fill_n(getOutput(0, &sizeOut), sizeOut, mValue);
}

// Main Class

// Constructor

FrameLib_Expression::FrameLib_Expression(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_Block(kProcessor, context, proxy), mInputProcessor(NULL), mParameters(&sParamInfo)
{
    typedef FrameLib_Block::Connection Connection;
    
    mParameters.addString(kExpression, "expression", 0);
    mParameters.setInstantiation();
    
    mParameters.set(serialisedParameters);
    
    Graph graph;
    Parser parser;
    ExprParseError error = parser.parse(graph, mParameters.getString(kExpression));

    if (graph.mNumInputs > 32)
        graph = Graph();
    
    setIO(graph.mNumInputs, 1);
    
    if (!error && graph.mOperations.size())
    {
        // Create and Input Processor
        
        mInputProcessor = new InputProcessor(context, graph.mNumInputs);

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

        mGraph.push_back(new ConstantOut(context, graph.mNumInputs, graph.mConstant));
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
