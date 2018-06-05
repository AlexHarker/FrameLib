
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>
#include <functional>

#include <string.h>

#include "../Unary/FrameLib_Unary_Template.h"
#include "../Binary/FrameLib_Binary_Template.h"
#include "../Ternary/FrameLib_Ternary_Template.h"
#include "../Ternary/FrameLib_Ternary_Objects.h"

// ******************************************** //

// Detect Character Classes

inline bool isWhitespace(char c)
{
    return  (c == ' ') ||
    (c == '\n') ||
    (c == '\r') ||
    (c == '\t') ||
    (c == '\b') ||
    (c == '\v') ||
    (c == '\f');
}

inline bool isLetter(char c)
{
    return ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z'));
}

inline bool isDigit(char c)            { return (c >= '0') && (c <= '9'); }

// Detect Single Characters

inline bool isUnderscore(char c)        { return (c == '_'); }
inline bool isPeriod(char c)            { return (c == '.'); }
inline bool isComma(char c)             { return (c == ','); }
inline bool isLHSParenthesis(char c)    { return (c == '('); }
inline bool isRHSParenthesis(char c)    { return (c == ')'); }

// Detect Sets of Single Characters

inline bool isParenthesis(char c)       { return isLHSParenthesis(c) || isRHSParenthesis(c); }
inline bool isReservedChar(char c)      { return isUnderscore(c) || isPeriod(c) || isComma(c) || isParenthesis(c); }

// ******************************************** //

// A base class for functions and operators

struct OpBase
{
    OpBase(const char* name, int precedence) : mName(name), mPrecedence(precedence) {}
    virtual ~OpBase() {}
    
    virtual int numItems() const = 0;
    virtual double call(double a, double b, double c) const = 0;
    virtual FrameLib_DSP *create(FrameLib_Context context, bool const1, bool const2, bool const3) const = 0;

    const char* mName;
    int mPrecedence;
};

// Function/Operator Templates

template <typename Op>
struct UnaryOperation : public OpBase
{
    UnaryOperation(const char* name, int precedence = 0) : OpBase(name, precedence) {}
    
    virtual int numItems() const                        { return 1; }
    double call(double a, double b, double c) const     { return Op()(a); }
    
    FrameLib_DSP *create(FrameLib_Context context, bool const1, bool const2, bool const3) const
    {
        return new FrameLib_UnaryOp<Op>(context, NULL, NULL);
    }
};

template <typename Op>
struct BinaryOperation : public OpBase
{
    BinaryOperation(const char* name, int precedence = 0) : OpBase(name, precedence) {}
    
    virtual int numItems() const                        { return 2; }
    double call(double a, double b, double c) const     { return Op()(a, b); }
    
    FrameLib_DSP *create(FrameLib_Context context, bool const1, bool const2, bool const3) const
    {
        FrameLib_Parameters::AutoSerial serialiedParameters;
        serialiedParameters.write("mismatch", (const1 || const2) ? "wrap" : "shrink");
        return new FrameLib_BinaryOp<Op>(context, &serialiedParameters, NULL);
    }
};

template <typename Op>
struct TernaryOperation : public OpBase
{
    TernaryOperation(const char* name, int precedence = 0) : OpBase(name, precedence) {}
    
    virtual int numItems() const                        { return 3; }
    double call(double a, double b, double c) const     { return Op()(a, b, c); }
    
    FrameLib_DSP *create(FrameLib_Context context, bool const1, bool const2, bool const3) const
    {
        // FIX - not sure what to do here...
        FrameLib_Parameters::AutoSerial serialiedParameters;
        serialiedParameters.write("mismatch", "shrink");
        return new FrameLib_TernaryOp<Op>(context, &serialiedParameters, NULL);
    }
};



class OpList
{
    
public:
    
    OpList() {}
    
    ~OpList()
    {
        for (std::vector<OpBase *>::iterator it = mItems.begin(); it != mItems.end(); it++)
        {
            delete (*it);
            *it = NULL;
        }
        
        mItems.clear();
    }
    
    void addItem(OpBase *item)
    {
        mItems.push_back(item);
    }
    
    const OpBase *getItem(const char *name) const
    {
        for (std::vector<OpBase *>::const_iterator it = mItems.begin(); it != mItems.end(); it++)
            if (!strcmp(name, (*it)->mName))
                return (*it);
        
        return NULL;
    }
    
private:
    
    std::vector<OpBase *> mItems;
};

struct FunctionList : private OpList
{
    FunctionList()
    {
        addItem(new UnaryOperation<Unary_Functor<sin> >("sin"));
        addItem(new UnaryOperation<Unary_Functor<cos> >("cos"));
        addItem(new UnaryOperation<Unary_Functor<tan> >("tan"));
        addItem(new UnaryOperation<Unary_Functor<sinh> >("sinh"));
        addItem(new UnaryOperation<Unary_Functor<cosh> >("cosh"));
        addItem(new UnaryOperation<Unary_Functor<tanh> >("tanh"));
        addItem(new UnaryOperation<Unary_Functor<asin> >("asin"));
        addItem(new UnaryOperation<Unary_Functor<acos> >("acos"));
        addItem(new UnaryOperation<Unary_Functor<atan> >("atan"));
        addItem(new UnaryOperation<Unary_Functor<asinh> >("asinh"));
        addItem(new UnaryOperation<Unary_Functor<acosh> >("acosh"));
        addItem(new UnaryOperation<Unary_Functor<atanh> >("atanh"));
        
        addItem(new UnaryOperation<Unary_Functor<log> >("log"));
        addItem(new UnaryOperation<Unary_Functor<log2> >("log2"));
        addItem(new UnaryOperation<Unary_Functor<log10> >("log10"));
        addItem(new UnaryOperation<Unary_Functor<exp> >("exp"));
        addItem(new UnaryOperation<Unary_Functor<exp2> >("exp2"));
                                                                     
        addItem(new UnaryOperation<Unary_Functor<fabs> >("abs"));
        addItem(new UnaryOperation<Unary_Functor<ceil> >("ceil"));
        addItem(new UnaryOperation<Unary_Functor<floor> >("floor"));
        addItem(new UnaryOperation<Unary_Functor<round> >("round"));
        addItem(new UnaryOperation<Unary_Functor<trunc> >("trunc"));

        addItem(new UnaryOperation<Unary_Functor<sqrt> >("sqrt"));
        addItem(new UnaryOperation<Unary_Functor<cbrt> >("cbrt"));
        addItem(new UnaryOperation<Unary_Functor<erf> >("erf"));
        addItem(new UnaryOperation<Unary_Functor<erfc> >("erfc"));
        
        addItem(new BinaryOperation<Binary_Functor<pow> >("pow"));
        addItem(new BinaryOperation<Binary_Functor<hypot> >("hypot"));
        addItem(new BinaryOperation<Binary_Functor<fmin> >("min"));
        addItem(new BinaryOperation<Binary_Functor<fmax> >("max"));
        addItem(new BinaryOperation<Binary_Functor<fdim> >("diff"));
        
        addItem(new TernaryOperation<Ternary_Functor<Ternary::clip> >("clip"));
        addItem(new TernaryOperation<Ternary_Functor<Ternary::wrap> >("wrap"));
        addItem(new TernaryOperation<Ternary_Functor<Ternary::fold> >("fold"));
    }
    
    const OpBase *getFunction(const char *name) const
    {
        return getItem(name);
    }
};

class OperatorList
{
    static double negate(double a) { return -a; }

    void addOperator(OpBase* op)
    {
        int precedence = op->mPrecedence;
        
        assert((precedence < precedenceLimit() && "precedence value too large"));
        
        // Add the operator at the right precedence level and keep a list of unique characters
        
        mOperators[precedence].addItem(op);
        for (const char *name = op->mName; *name; name++)
            if (!(isLetter(*name) || isDigit(*name) || isReservedChar(*name)))
                mCharList.push_back(*name);
        
        mMaxLengthOp = std::max(mMaxLengthOp, strlen(op->mName));
        
        // Sort so that we can keep the unique characters by using std:unique and std::erase
        
        std::sort(mCharList.begin(), mCharList.end());
        std::vector<char>::iterator it = std::unique(mCharList.begin(), mCharList.end());
        mCharList.erase(it, mCharList.end());
    }
    
public:
    
    OperatorList() : mMaxLengthOp(0)
    {
        mOperators.resize(7);
        
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
    }
    
    const OpBase *getOperator(const char *name, int precedence) const
    {
        return mOperators[precedence].getItem(name);
    }
    
    const OpBase *getOperator(const char *name) const
    {
        const OpBase *op = NULL;
        
        for (int i = 0; i < precedenceLimit(); i++)
            if ((op = getOperator(name, i)))
                return op;
        
        return op;
    }

    size_t maxLengthOp() const          { return mMaxLengthOp; }
    int precedenceLimit() const         { return static_cast<int>(mOperators.size()); }
    bool isOperatorChar(const char c)   { return std::find(mCharList.begin(), mCharList.end(), c) != mCharList.end(); }
    
private:
    
    size_t mMaxLengthOp;
    std::vector<char> mCharList;
    std::vector<OpList> mOperators;
};

class ConstantList
{
    struct Constant
    {
        Constant(const char* name, double value) : mName(name), mValue(value) {}
        
        const char *mName;
        double mValue;
    };
    
    void addConstant(const char *name, double value) { mConstants.push_back(Constant(name, value)); }
    
public:
    
    ConstantList()
    {
        addConstant("e", M_E);
        addConstant("pi", M_PI);
        addConstant("epsilon", std::numeric_limits<double>::epsilon());
        addConstant("inf", std::numeric_limits<double>::infinity());
    }
    
    bool getConstant(const char *name, double& value) const
    {
        for (std::vector<Constant>::const_iterator it = mConstants.begin(); it != mConstants.end(); it++)
        {
            if (!strcmp(name, it->mName))
            {
                value = it->mValue;
                return true;
            }
        }
        
        return false;
    }
    
private:
    
    std::vector<Constant> mConstants;
};

// FIX - globals

ConstantList constantList;
FunctionList functionList;
OperatorList operatorList;

// Types

enum ParserError
{
    kNoError = 0,
    
    kLexError_UnknownChar,
    kLexError_ZeroLengthToken,
    
    kParseError_EmptySubExpr,
    kParseError_StrayParenthesis,
    kParseError_NotFunction,
    kParseError_UnknownToken,
    kParseError_TooFewCommas,
    kParseError_TooManyCommas,
    kParseError_OpPosition,
    kParseError_StrayItem
};

// Lexing functionality

namespace Lexer
{
    enum TokenType { kParenthesisLHS, kParenthesisRHS, kComma, kOperator, kNumber, kSymbol, kUnknown };

    // A token with a type and string
    
    struct Token
    {
        Token() : mType(kUnknown), mText ("") {}
        Token(TokenType type, const char *string, size_t length) : mType(type), mText(string, length) {}
        
        TokenType mType;
        std::string mText;
    };
    
    // Retrieve the type of token from its first character
    
    TokenType getTokenType(const char c)
    {
        if (isLHSParenthesis(c))                return kParenthesisLHS;
        if (isRHSParenthesis(c))                return kParenthesisRHS;
        if (isComma(c))                         return kComma;
        if (isDigit(c))                         return kNumber;
        if (isLetter(c))                        return kSymbol;
        if (operatorList.isOperatorChar(c))     return kOperator;

        return kUnknown;
    }
    
    // Find the extent of an operator token
    
    const char *extentOperator(const char *expr)
    {
        for (size_t i = std::min(strlen(expr), operatorList.maxLengthOp()); i > 0; i--)
        {
            std::string op(expr, i);
            
            if (operatorList.getOperator(op.c_str()))
                return expr + i;
        }
        
        return expr;
    }
    
    // Find the extent of a number token

    const char *extentNumber(const char *expr)
    {
        if (isDigit(*expr))
        {
            while (isDigit(*expr)) expr++;
            if (isPeriod(*expr)) for (expr++; isDigit(*expr); expr++);
        }
        
        return expr;
    }
    
    // Find the extent of a symbol token

    const char *extentSymbol(const char *expr)
    {
        if (isLetter(*expr))
            while (isLetter(*expr) || isUnderscore(*expr) || isDigit(*expr)) expr++;
        return expr;
    }
    
    // Lex the input into tokens or nodes
    
    template <class T>
    ParserError lex(std::vector<T>& tokens, const char *expr)
    {
        while (*expr)
        {
            TokenType type;
            const char *end = expr;
            
            // Ignore whitespace
            
            while (isWhitespace(*expr)) expr++;
            
            if (!(*expr))
                break;
            
            // Lex the tokens
            
            switch((type = getTokenType(*expr)))
            {
                case kParenthesisLHS:       end = expr + 1;                 break;
                case kParenthesisRHS:       end = expr + 1;                 break;
                case kComma:                end = expr + 1;                 break;
                case kOperator:             end = extentOperator(expr);     break;
                case kNumber:               end = extentNumber(expr);       break;
                case kSymbol:               end = extentSymbol(expr);       break;
                case kUnknown:              return kLexError_UnknownChar;
            }
            
            if (end <= expr)
                return kLexError_ZeroLengthToken;
            
            tokens.push_back(Token(type, expr, end - expr));
            expr = end;
        }
        
        return kNoError;
    }
};

// Graph Structure

enum InputType { kInputUnused, kInputConstant, kInputVariable, kInputResult };
enum IndexType { kIsInput, kIsOutput };

struct Graph
{
    Graph() : mNumInputs(1), mConstant(std::numeric_limits<double>::quiet_NaN()) {}
    
    struct Input
    {
        Input() : mType(kInputUnused), mIndex(-1), mValue(0.0) {}
        Input(double value) : mType(kInputConstant), mIndex(-1), mValue(value) {}
        Input(IndexType type, long index) : mType(type == kIsInput ? kInputVariable : kInputResult), mIndex(index), mValue(0.0) {}
        
        bool isFixed() { return mType == kInputUnused || mType == kInputConstant; }

        InputType mType;
        long mIndex;
        double mValue;
    };
    
    struct Operation
    {
        Operation(const OpBase *op) : mOp(op) {}
        
        bool isFixed(int i) { return mIns[i].isFixed(); }

        const OpBase *mOp;
        Input mIns[3];
    };
    
    std::vector<Operation> mOperations;
    long mNumInputs;
    double mConstant;
};

// Parsing functionality

class Parser
{
    // A node that might be a token (yet to be converted), a number, a result or a variable

    enum NodeType { kToken, kConstant, kVariable, kResult };

    class Node
    {
        
    public:
        
        Node() : mType(kToken), mValue(0.0), mIndex(-1) {}
        Node(Lexer::Token token) : mType(kToken), mToken(token), mValue(0.0), mIndex(-1) {}
        Node(double constant) : mType(kConstant), mValue(constant), mIndex(-1) {}
        Node(IndexType type, long index) : mType(type == kIsInput ? kVariable : kResult), mValue(0.0), mIndex(index) {}
        
        // Checks for types of node / tokens
        
        inline bool isLHSParenthesis() const    { return mToken.mType == Lexer::kParenthesisLHS; }
        inline bool isRHSParenthesis() const    { return mToken.mType == Lexer::kParenthesisRHS; }
        inline bool isComma() const             { return mToken.mType == Lexer::kComma; }
        inline bool isOperator() const          { return mToken.mType == Lexer::kOperator; }
        inline bool isSymbol() const            { return mToken.mType == Lexer::kSymbol; }
        inline bool isNumber() const            { return mToken.mType == Lexer::kNumber; }
        inline bool isVariable() const          { return mType == kVariable; }
        inline bool isConstant() const          { return mType == kConstant; }
        inline bool isToken() const             { return mType == kToken; }
        
        // Getters
        
        inline long getIndex() const            { return mIndex; }
        inline double getValue() const          { return mValue; }
        const char *getTokenString() const      { return mToken.mText.c_str(); }
        
        // Conversion
        
        void convertTokens(long& maxInput)
        {
            double value = 0.0;
            
            // Convert number tokens to constants
            
            if (isNumber())
                *this = Node(std::stod(mToken.mText));
            
            // Convert constant symbols to constants
            
            if (isSymbol() && constantList.getConstant(getTokenString(), value))
                *this = Node(value);
            
            // Convert variable tokens to variables
        
            if (isSymbol() && !mToken.mText.compare(0, 2, "in") && mToken.mText.length() > 2)
            {
                for (size_t i = 2; i < mToken.mText.length(); i++)
                    if (!isDigit(mToken.mText[i]))
                        return;
                
                long index = std::stol(mToken.mText.substr(2)) - 1;
                maxInput = std::max(maxInput, index + 1);
                *this = Node(kIsInput, index);
            }
        }
        
    private:
        
        NodeType mType;
        Lexer::Token mToken;
        double mValue;
        long mIndex;
    };

    typedef std::vector<Node> NodeList;
    typedef std::vector<Node>::iterator NodeListIterator;
    
    Graph::Input parseInput(const Node& arg)
    {
        if (arg.isConstant())
            return Graph::Input(arg.getValue());
        if (arg.isVariable())
            return Graph::Input(kIsInput, arg.getIndex());
        
        return Graph::Input(kIsOutput, arg.getIndex());
    }

    Node parseOperation(Graph& graph, const OpBase *op, const Node& arg1, const Node& arg2, const Node& arg3)
    {
        // Fold constants
        
        if ( arg1.isConstant() && arg2.isConstant() && arg3.isConstant())
            return Node(op->call(arg1.getValue(), arg2.getValue(), arg3.getValue()));
        
        // Add to the graph
        
        graph.mOperations.push_back(op);
        
        int numItems = op->numItems();
        if (numItems)       graph.mOperations.back().mIns[0] = parseInput(arg1);
        if (numItems > 1)   graph.mOperations.back().mIns[1] = parseInput(arg2);
        if (numItems > 2)   graph.mOperations.back().mIns[2] = parseInput(arg3);
        
        return Node(kIsOutput, graph.mOperations.size() - 1);
    }
    
    ParserError parseUnaryOperator(Graph& graph, const OpBase *op, NodeList& nodes, NodeListIterator& it)
    {
        // Check the operator isn't last, that its either first, or precededed by an operator and not followed by one
        // N.B. return no error in case this is also an operator later, otherwise it'll be picked up as a stray item
        
        if ((it == nodes.end() - 1) || (it != nodes.begin() && !(it - 1)->isOperator()) || (it + 1)->isOperator())
            return kNoError;

        // Parse operation amd erase consumed node (the iterator is still valid so don't alter)
        
        *it = parseOperation(graph, op, *(it + 1), *(it + 1), *(it + 1));
        nodes.erase(it + 1);
        
        return kNoError;
    }

    ParserError parseBinaryOperator(Graph& graph, const OpBase *op, NodeList& nodes, NodeListIterator& it)
    {
        // Check the operator is not at the beginning or end and the nodes either side are not operators
        
        if (it == nodes.begin() || it == nodes.end() - 1 || (it - 1)->isOperator() || (it + 1)->isOperator())
            return kParseError_OpPosition;
        
        // Parse operation and erase consumed nodes (set the iterator back one, as it will be incremented)
        
        *(it - 1) = parseOperation(graph, op, *(it - 1), *(it + 1), *(it + 1));
        it = nodes.erase(it, it + 2) - 1;
        
        return kNoError;
    }

    void parseFunction(Graph& graph, const OpBase *function, NodeList& nodes)
    {
        int numItems = function->numItems();
        
        if (!numItems) nodes.push_back(Node(0.0));
        
        nodes[0] = parseOperation(graph, function, nodes[0], nodes[numItems > 1 ? 1 : 0], nodes[numItems > 2 ? 2 : 0]);
    }

    ParserError parseOperators(Graph& graph, Node &result, NodeList& nodes)
    {
        const OpBase *op = NULL;
        ParserError error = kNoError;

        // Check for any remaining tokens that are not operators

        for (NodeListIterator it = nodes.begin(); it != nodes.end(); it++)
            if (it->isToken() && !it->isOperator())
                return kParseError_UnknownToken;
        
        // Now resolve unary operators by searching right-to-left
        
         for (NodeListIterator it = nodes.end() - 1; it >= nodes.begin(); it--)
             if ((op = operatorList.getOperator(it->getTokenString(), 0)))
                if ((error = parseUnaryOperator(graph, op, nodes, it)))
                    return error;

        // Now resolve binary operators in order of precedence
        
        for (int i = 1; i < operatorList.precedenceLimit(); i++)
        {
            for (NodeListIterator it = nodes.begin(); it < nodes.end(); it++)
                if ((op = operatorList.getOperator(it->getTokenString(), i)))
                    if ((error = parseBinaryOperator(graph, op, nodes, it)))
                        return error;
        }
        
        // Store result and return, checking for stray items
        
        result = nodes[0];
        
        return (nodes.size() != 1 || nodes[0].isToken()) ? kParseError_StrayItem : kNoError;;
    }

    ParserError recursiveParse(Graph& graph, NodeList& nodes, int resultItems)
    {
        ParserError error = kNoError;
        NodeListIterator n1, n2;
        
        // Check for empty node list
        
        if (nodes.empty())
            return kParseError_EmptySubExpr;
        
        // Parentheses
        
        for (n2 = nodes.begin(); ; )
        {
            const OpBase *function = NULL;

            // Find first RHS parenthesis and matching LHS parenthesis
            
            for ( ; n2 != nodes.end(); n2++)
                if (n2->isRHSParenthesis())
                    break;

            for (n1 = n2 == nodes.end() ? n2 - 1 : n2; n1 >= nodes.begin(); n1--)
                if (n1->isLHSParenthesis())
                    break;
        
            // Check that there are parentheses found (if not continue) and for stray parentheses
            
            if ((n1 < nodes.begin()) && (n2 == nodes.end()))
                break;
            
            if ((n1 < nodes.begin()) != (n2 == nodes.end()))
                return kParseError_StrayParenthesis;
            
            // Copy child nodes, determine if this is a function call and how many items it requires
            
            NodeList childNodes(n1 + 1, n2);
            int numItems = 1;
            
            if ((n1 != nodes.begin()) && (n1 - 1)->isSymbol())
            {
                // Check that a function is found
                
                if (!(function = functionList.getFunction((n1 - 1)->getTokenString())))
                    return kParseError_NotFunction;
                
                numItems = function->numItems();
                n1--;
            }
            
            // Parse the child nodes and then the function (if it is a function)
            
            if ((error = recursiveParse(graph, childNodes, numItems)))
                return error;
            
             if (function)
                 parseFunction(graph, function, childNodes);
            
            // Copy the result and then erase the unneeded nodes from this level
            
            *n1 = childNodes[0];
            n2 = nodes.erase(n1 + 1, n2 + 1);
        }
        
        // Commas
        
        for (n1 = nodes.begin(); n1 != nodes.end(); n1++)
        {
            // Find the next comma
            
            for (n2 = n1; n2 != nodes.end(); n2++)
                if (n2->isComma())
                    break;
            
            // Check that there are some items to parse
            
            if (n1 == n2)
                return kParseError_EmptySubExpr;
            
            // Copy the child nodes and erase from this level, then parse the remaining operators
            
            NodeList childNodes(n1, n2);
            nodes.erase(n1 + 1, (n2 == nodes.end()) ? n2 : n2 + 1);
            if ((error = parseOperators(graph, *n1, childNodes)))
                return error;
        }
        
        // Check that we have the required number of nodes
        
        if (nodes.size() == resultItems)
            return kNoError;
        
        return (nodes.size() < resultItems) ? kParseError_TooFewCommas : kParseError_TooManyCommas;
    }

public:
        
    ParserError parse(Graph& graph, const char *expr)
    {
        NodeList nodes;
        ParserError error;

        graph = Graph();
        
        // Run lexer
        
        if ((error = Lexer::lex(nodes, expr)))
            return error;
        
        // Replace number tokens and symbols representing constants/variables with relevant node types
        
        for (NodeListIterator it = nodes.begin(); it != nodes.end(); it++)
            it->convertTokens(graph.mNumInputs);
        
        // Parse
        
        if ((error = recursiveParse(graph, nodes, 1)))
            return error;
       
        if (nodes.size() && nodes[0].isConstant())
            graph.mConstant = nodes[0].getValue();
        
        return kNoError;
    }
};
