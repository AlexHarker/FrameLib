
#include "FrameLib_ExpressionGraph.h"
#include <string>
#include "parser.h"

// Constant Out Class

// Constructor

FrameLib_Expression::ConstantOut::ConstantOut(FrameLib_Context context, unsigned long numIns, double value) : FrameLib_Processor(context, NULL, NULL, numIns, 1), mValue(value) {}

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

// Process

// Constructor

FrameLib_Expression::FrameLib_Expression(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_Block(kProcessor, context, proxy), mParameters(&sParamInfo)
{
    typedef FrameLib_Block::Connection Connection;
    
    mParameters.addString(kExpression, "expression", 0);
    mParameters.setInstantiation();
    
    mParameters.set(serialisedParameters);
    
    Graph graph;
    Parser graphParser;
    ParserError error = graphParser.parse(graph, mParameters.getString(kExpression));

    if (graph.mNumInputs > 32)
        graph = Graph();
    
    setIO(graph.mNumInputs, 1);
    
    if (!error && graph.mOperations.size())
    {
        // Build the graph if there is one
        
        for (std::vector<Graph::Operation>::iterator it = graph.mOperations.begin(); it != graph.mOperations.end(); it++)
        {
            FrameLib_DSP* operation = it->mOp->create(context, it->isFixed(0), it->isFixed(1), it->isFixed(2));
            mGraph.push_back(operation);
            
            for (int i = 0 ; i < 3 && it->mIns[i].mType != kInputUnused; i++)
            {
                if (it->mIns[i].mType == kInputConstant)
                    operation->setFixedInput(i, &it->mIns[i].mValue, 1);
                else if (it->mIns[i].mType == kInputVariable)
                    operation->setInputAlias(Connection(this, it->mIns[i].mIndex), i);
                else
                    operation->addConnection(Connection(mGraph[it->mIns[i].mIndex], 0), i);
            }
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
    }
}

FrameLib_Expression::~FrameLib_Expression()
{
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
    for (std::vector<FrameLib_DSP *>::iterator it = mGraph.begin(); it != mGraph.end(); it++)
        (*it)->reset(samplingRate, maxBlockSize);
}
