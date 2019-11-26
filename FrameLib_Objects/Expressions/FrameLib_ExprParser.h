
#ifndef FRAMELIB_EXPRPARSER_H
#define FRAMELIB_EXPRPARSER_H

#include "FrameLib_Types.h"
#include "FrameLib_Errors.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <limits>
#include <vector>

#include <string.h>

namespace FrameLib_ExprParser
{
    // Parsing Error Types

    enum ExprParseError
    {
        kNoError = 0,
        
        kLexError_UnknownChar,
        kLexError_ZeroLengthToken,
        
        kParseError_EmptySubExpr,
        kParseError_UnmatchedParenthesis,
        kParseError_NotFunction,
        kParseError_UnknownToken,
        kParseError_TooFewCommas,
        kParseError_TooManyCommas,
        kParseError_OpPosition,
        kParseError_FunctionPosition,
        kParseError_StrayItem
    };

    // A base class for functions and operators

    template <class T>
    struct OpBase
    {
        OpBase(const char* name, unsigned int precedence) : mName(name), mPrecedence(precedence) {}
        virtual ~OpBase() {}
        
        virtual size_t numItems() const = 0;
        virtual T call(T a, T b, T c) const = 0;
        virtual FrameLib_DSP *create(FrameLib_Context context) const = 0;

        const char* mName;
        unsigned int mPrecedence;
    };

    // Graph Structure for Output

    enum InputType { kInputUnused, kInputConstant, kInputVariable, kInputResult };
    enum IndexType { kIsInput, kIsOutput };

    template <class T>
    struct Graph
    {
        Graph() : mNumInputs(1), mConstant(0.0) {}
        
        struct Input
        {
            Input() : mType(kInputUnused), mIndex(-1), mValue(0.0) {}
            Input(T value) : mType(kInputConstant), mIndex(-1), mValue(value) {}
            Input(IndexType type, long index) : mType(type == kIsInput ? kInputVariable : kInputResult), mIndex(index), mValue(0.0) {}
            
            InputType mType;
            long mIndex;
            T mValue;
        };
        
        struct Operation
        {
            Operation(const OpBase<T> *op) : mOp(op) {}
            
            const OpBase<T> *mOp;
            Input mIns[3];
        };
        
        std::vector<Operation> mOperations;
        long mNumInputs;
        T mConstant;
    };

    // Parser Class

    template <class T>
    class Parser
    {
        enum TokenType { kParenthesisLHS, kParenthesisRHS, kComma, kOperator, kNumber, kSymbol, kUnknown };
        enum NodeType { kToken, kConstant, kVariable, kResult };

        // An indication of where a node starts and ends in the original string
        
        struct CharSpan
        {
            CharSpan() : mPos(0), mEnd(0) {}
            CharSpan(size_t pos, size_t end) : mPos(pos), mEnd(end) {}
            CharSpan(size_t pos) : CharSpan(pos, pos) {}
            CharSpan(const char* str, const char* pos) : CharSpan(pos - str, pos + 1 - str) {}
            CharSpan(const char* str, const char* pos, const char* end) : mPos(pos - str), mEnd(end - str) {}
            
            size_t mPos, mEnd;
        };
        
        // A token with a type and string

        struct Token
        {
            Token() : mType(kUnknown), mText ("") {}
            Token(TokenType type, const char *string, size_t length) : mType(type), mText(string, length) {}
            
            TokenType mType;
            std::string mText;
        };
        
        // A node that might be a token (yet to be converted), a number, a result or a variable
        
        class Node
        {
            
        public:
            
            Node() : mType(kToken), mValue(0.0), mIndex(-1) {}
            Node(Token token, CharSpan span) : mType(kToken), mToken(token), mValue(0.0), mIndex(-1), mSpan(span) {}
            Node(T constant, CharSpan span) : mType(kConstant), mValue(constant), mIndex(-1), mSpan(span) {}
            Node(IndexType type, long index, CharSpan span) : mType(type == kIsInput ? kVariable : kResult), mValue(0.0), mIndex(index), mSpan(span) {}
            
            // Checks for types of node / tokens
            
            inline bool isLHSParenthesis() const    { return mToken.mType == kParenthesisLHS; }
            inline bool isRHSParenthesis() const    { return mToken.mType == kParenthesisRHS; }
            inline bool isComma() const             { return mToken.mType == kComma; }
            inline bool isOperator() const          { return mToken.mType == kOperator; }
            inline bool isSymbol() const            { return mToken.mType == kSymbol; }
            inline bool isNumber() const            { return mToken.mType == kNumber; }
            inline bool isVariable() const          { return mType == kVariable; }
            inline bool isConstant() const          { return mType == kConstant; }
            inline bool isToken() const             { return mType == kToken; }
            
            // Getters
            
            inline long getIndex() const            { return mIndex; }
            inline T getValue() const               { return mValue; }
            const char *getTokenString() const      { return mToken.mText.c_str(); }
            CharSpan getCharSpan() const            { return mSpan; }
            size_t getCharPos() const               { return mSpan.mPos; }
            size_t getCharEnd() const               { return mSpan.mEnd; }

            // Input Token Conversion
            
            long convertInputTokens()
            {
                if (isSymbol() && !mToken.mText.compare(0, 2, "in") && mToken.mText.length() > 2)
                {
                    for (size_t i = 2; i < mToken.mText.length(); i++)
                        if (!isDigit(mToken.mText[i]))
                            return -1;
                    
                    *this = Node(kIsInput, std::stol(mToken.mText.substr(2)) - 1, mSpan);
                }
                
                return mIndex + 1;
            }
            
        private:
            
            NodeType mType;
            Token mToken;
            T mValue;
            long mIndex;
            CharSpan mSpan;
        };
        
        // Aliases for brevity
        
        using NodeList = std::vector<Node>;
        using NodeIt = typename std::vector<Node>::iterator;
        using ConstOpPtr = const OpBase<T>*;
        
        // Constant Struct
        
        struct Constant
        {
            Constant(const char* name, T value) : mName(name), mValue(value) {}
            
            const char *mName;
            T mValue;
        };
        
        // Operator List Class
        
        class OperatorList
        {
            
        public:
            
            OperatorList() {}
            OperatorList(OperatorList&&) = default;
            OperatorList& operator=(OperatorList&&) = default;
            
            void addItem(OpBase<T> *item)
            {
                mItems.add(item);
            }
            
            ConstOpPtr getItem(const char *name) const
            {
                for (auto it = mItems.begin(); it != mItems.end(); it++)
                    if (!strcmp(name, (*it)->mName))
                        return it->get();
                
                return nullptr;
            }
            
        private:
            
            // Deleted
            
            OperatorList(const OperatorList&) = delete;
            OperatorList& operator=(const OperatorList&) = delete;
            
            FrameLib_OwnedList<OpBase<T>> mItems;
        };
        
    public:
        
        Parser(unsigned int precedenceLevels) : mMaxLengthOp(0)
        {
            mOperators.resize(precedenceLevels);
        }
        
        void addOperator(OpBase<T>* op)
        {
            unsigned int precedence = op->mPrecedence;
            
            assert((precedence < mOperators.size() && "precedence value too large"));
            
            // Add the operator at the right precedence level and keep a list of unique characters
            
            mOperators[precedence].addItem(op);
            for (const char *name = op->mName; *name; name++)
                if (!(isLetter(*name) || isDigit(*name) || isReservedChar(*name)))
                    mOperatorCharList.push_back(*name);
            
            mMaxLengthOp = std::max(mMaxLengthOp, strlen(op->mName));
            
            // Sort so that we can keep the unique characters by using std:unique and std::erase
            
            std::sort(mOperatorCharList.begin(), mOperatorCharList.end());
            auto it = std::unique(mOperatorCharList.begin(), mOperatorCharList.end());
            mOperatorCharList.erase(it, mOperatorCharList.end());
        }
        
        void addFunction(OpBase<T> *item)
        {
            mFunctions.addItem(item);
        }
        
        void addConstant(const char *name, T value)
        {
            mConstants.push_back(Constant(name, value));
        }
        
        void setDefaultConstant(T constant)
        {
            mDefaultConstant = constant;
        }
        
        ExprParseError parse(Graph<T>& graph, const char *expr, FrameLib_ErrorReporter& reporter, FrameLib_Proxy *proxy)
        {
            mReporter = &reporter;
            mProxy = proxy;
            mExpr = expr;
            
            NodeList nodes;
            ExprParseError error;
            T value = 0.0;
            
            graph = Graph<T>();
            
            // Run lexer
            
            if ((error = lex(nodes, expr)))
                return error;
            
            // Replace number tokens and symbols representing constants/variables with relevant node types
            
            for (auto it = nodes.begin(); it != nodes.end(); it++)
            {
                // Convert number tokens to constants
                
                if (it->isNumber())
                    *it = Node(convertTextToNumber(it->getTokenString()), it->getCharSpan());
                
                // Convert constant symbols to constants
                
                if (it->isSymbol() && getConstant(it->getTokenString(), value))
                    *it = Node(value, it->getCharSpan());
                
                // Convert variable tokens to variables
                
                graph.mNumInputs = std::max(it->convertInputTokens(), graph.mNumInputs);
            }
            
            // Parse
            
            if ((error = recursiveParse(graph, nodes, 1, 0)))
                return error;
            
            if (nodes.size() && nodes[0].isConstant())
                graph.mConstant = nodes[0].getValue();
            
            return kNoError;
        }
        
    private:
        
        // Error reporting
        
        template <typename... Args>
        void reportError(const char *error, Args... args)
        {
            (*mReporter)(kErrorObject, mProxy, error, args...);
        }
        
        template <typename... Args>
        void reportError(const char *error, CharSpan span, Args... args)
        {
            std::string expr(mExpr);
            expr.insert(span.mEnd, " << ");
            expr.insert(span.mPos, " >> ");

            reportError(error, args...);
            reportError("expression error here: #", expr.c_str());
        }
        
        template <typename... Args>
        void reportError(const char *error, NodeIt n, Args... args)
        {
            reportError(error, n->getCharSpan(), args...);
        }
        
        // Detect Character Classes
        
        static inline bool isWhitespace(char c)
        {
            return  (c == ' ') ||
            (c == '\n') ||
            (c == '\r') ||
            (c == '\t') ||
            (c == '\b') ||
            (c == '\v') ||
            (c == '\f');
        }
        
        static inline bool isLetter(char c)
        {
            return ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z'));
        }
        
        static inline bool isDigit(char c)                 { return (c >= '0') && (c <= '9'); }
        
        inline bool isOperatorChar(const char c)
        {
            return std::find(mOperatorCharList.begin(), mOperatorCharList.end(), c) != mOperatorCharList.end();
        }

        // Detect Single Characters
        
        static inline bool isUnderscore(char c)        { return (c == '_'); }
        static inline bool isPeriod(char c)            { return (c == '.'); }
        static inline bool isComma(char c)             { return (c == ','); }
        static inline bool isLHSParenthesis(char c)    { return (c == '('); }
        static inline bool isRHSParenthesis(char c)    { return (c == ')'); }
        
        // Detect Sets of Single Characters
        
        static inline bool isParenthesis(char c)       { return isLHSParenthesis(c) || isRHSParenthesis(c); }
        static inline bool isReservedChar(char c)      { return isUnderscore(c) || isPeriod(c) || isComma(c) || isParenthesis(c); }
        
        // Lexing functionality
        
        // Retrieve the type of token from its first character
        
        TokenType getTokenType(const char c)
        {
            if (isLHSParenthesis(c))        return kParenthesisLHS;
            if (isRHSParenthesis(c))        return kParenthesisRHS;
            if (isComma(c))                 return kComma;
            if (isDigit(c))                 return kNumber;
            if (isLetter(c))                return kSymbol;
            if (isOperatorChar(c))          return kOperator;
            
            return kUnknown;
        }
        
        T convertTextToNumber(const char*text) { return std::stod(text); }
        
        // Find the extent of an operator token
        
        const char *extentOperator(const char *expr)
        {
            for (size_t i = std::min(strlen(expr), mMaxLengthOp); i > 0; i--)
            {
                std::string op(expr, i);
                
                if (getOperator(op.c_str()))
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
        
        ExprParseError lex(std::vector<Node>& tokens, const char *expr)
        {
            const char *str = expr;
            
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
                    case kUnknown:
                    {
                        std::string s(1, *expr);
                        reportError("parsing failed - unknown character found: #", CharSpan(str, expr), s.c_str());
                        return kLexError_UnknownChar;
                    }
                }
                
                if (end <= expr)
                {
                    reportError("parsing failed - zero length token");
                    return kLexError_ZeroLengthToken;
                }
                
                tokens.push_back(Node(Token(type, expr, end - expr), CharSpan(str, expr, end)));
                expr = end;
            }
            
            return kNoError;
        }
        
        // Parsing functionality
        
        // Constants
        
        bool getConstant(const char *name, T& value) const
        {
            for (auto it = mConstants.begin(); it != mConstants.end(); it++)
            {
                if (!strcmp(name, it->mName))
                {
                    value = it->mValue;
                    return true;
                }
            }
            
            return false;
        }
        
        // Operators / Functions
        
        ConstOpPtr getOperator(const char *name, unsigned int precedence) const
        {
            return mOperators[precedence].getItem(name);
        }
        
        ConstOpPtr getOperator(const char *name) const
        {
            ConstOpPtr op = nullptr;
            
            for (unsigned int i = 0; i < static_cast<unsigned int>(mOperators.size()); i++)
                if ((op = getOperator(name, i)))
                    return op;
            
            return op;
        }
        
        // Parsing Functions
        
        typename Graph<T>::Input parseInput(NodeIt arg)
        {
            if (arg->isConstant())
                return typename Graph<T>::Input(arg->getValue());
            if (arg->isVariable())
                return typename Graph<T>::Input(kIsInput, arg->getIndex());
            
            return typename Graph<T>::Input(kIsOutput, arg->getIndex());
        }

        Node parseOperation(Graph<T>& graph, ConstOpPtr op, NodeIt a1, NodeIt a2, NodeIt a3, NodeIt n1, NodeIt n2)
        {
            CharSpan span(n1->getCharPos(), n2->getCharEnd());
            
            // Fold constants
            
            if (a1->isConstant() && a2->isConstant() && a3->isConstant())
                return Node(op->call(a1->getValue(), a2->getValue(), a3->getValue()), span);
            
            // Add to the graph
            
            graph.mOperations.push_back(op);
            
            size_t numItems = op->numItems();
            if (numItems)       graph.mOperations.back().mIns[0] = parseInput(a1);
            if (numItems > 1)   graph.mOperations.back().mIns[1] = parseInput(a2);
            if (numItems > 2)   graph.mOperations.back().mIns[2] = parseInput(a3);
            
            return Node(kIsOutput, static_cast<long>(graph.mOperations.size() - 1), span);
        }
        
        ExprParseError parseUnaryOperator(Graph<T>& graph, ConstOpPtr op, NodeList& nodes, const NodeIt& it)
        {
            // Check the operator isn't last, that it's either first, or precededed by an operator and not followed by one
            // N.B. return no error in case this is also an operator later, otherwise it'll be picked up as a stray item
            
            if ((it == nodes.end() - 1) || (it != nodes.begin() && !(it - 1)->isOperator()) || (it + 1)->isOperator())
                return kNoError;

            // Parse operation and erase consumed node (the iterator is still valid so don't alter)
            
            *it = parseOperation(graph, op, it + 1, it + 1, it + 1, it, it + 1);
            nodes.erase(it + 1);
            
            return kNoError;
        }

        ExprParseError parseBinaryOperator(Graph<T>& graph, ConstOpPtr op, NodeList& nodes, NodeIt& it)
        {
            // Check the operator is not at the beginning or end and the nodes either side are not operators
            
            if (it == nodes.begin() || it == nodes.end() - 1 || (it - 1)->isOperator() || (it + 1)->isOperator())
            {
                reportError("parsing failed - operator found in unexpected position", it);
                return kParseError_OpPosition;
            }
            
            // Parse operation and erase consumed nodes (set the iterator back one, as it will be incremented)
            
            *(it - 1) = parseOperation(graph, op, it - 1, it + 1, it + 1, it - 1, it + 1);
            it = nodes.erase(it, it + 2) - 1;
            
            return kNoError;
        }

        void parseFunction(Graph<T>& graph, ConstOpPtr function, NodeList& nodes, NodeIt n1, NodeIt n2)
        {
            assert(function->numItems() && "function takes no arguments");

            NodeIt n = nodes.begin();
            
            nodes[0] = parseOperation(graph, function, n, n + 1, n + 2, n1, n2);
        }

        ExprParseError parseOperators(Graph<T>& graph, Node &result, NodeList& nodes)
        {
            ConstOpPtr op = nullptr;
            ExprParseError error = kNoError;

            // Check for any remaining tokens that are not operators

            for (auto it = nodes.begin(); it != nodes.end(); it++)
            {
                if (it->isToken() && !it->isOperator())
                {
                    if (mFunctions.getItem(it->getTokenString()))
                    {
                        reportError("parsing failed - function without parantheses: #", it, it->getTokenString());
                        return kParseError_FunctionPosition;
                    }
                    
                    reportError("parsing failed - unknown token: #", it, it->getTokenString());
                    return kParseError_UnknownToken;
                }
            }
            
            // Now resolve unary operators by searching right-to-left
            
            for (auto it = nodes.end(); it != nodes.begin(); it--)
                if ((op = getOperator((it - 1)->getTokenString(), 0)))
                    if ((error = parseUnaryOperator(graph, op, nodes, it - 1)))
                        return error;
            
            // Now resolve binary operators in order of precedence
            
            for (unsigned int i = 1; i < static_cast<unsigned int>(mOperators.size()); i++)
            {
                for (auto it = nodes.begin(); it < nodes.end(); it++)
                    if ((op = getOperator(it->getTokenString(), i)))
                        if ((error = parseBinaryOperator(graph, op, nodes, it)))
                            return error;
            }
            
            // Store result and return, checking for stray items
            
            result = nodes[0];
            
            if (nodes.size() != 1 || nodes[0].isToken())
            {
                 CharSpan span = nodes.size() != 1 ? CharSpan(nodes.begin()->getCharEnd()) : CharSpan(nodes.begin()->getCharSpan());
                reportError("parsing failed - unexpected expression structure", span);
                return kParseError_StrayItem;
            }
            
            return kNoError;
        }

        ExprParseError recursiveParse(Graph<T>& graph, NodeList& nodes, size_t resultItems, size_t pos)
        {
            NodeIt n1, n2;
            ExprParseError error = kNoError;
            
            // Check for empty node list
            
            if (nodes.empty())
            {
                reportError("parsing failed - empty sub expression", CharSpan(pos));
                return kParseError_EmptySubExpr;
            }
            
            // Parentheses
            
            for (n2 = nodes.begin(); ; )
            {
                ConstOpPtr function = nullptr;

                // Find first RHS parenthesis and matching LHS parenthesis
                
                for ( ; n2 != nodes.end(); n2++)
                    if (n2->isRHSParenthesis())
                        break;

                // N.B. n1 is offset to stay in range
                
                for (n1 = n2; n1 != nodes.begin(); n1--)
                    if ((n1 - 1)->isLHSParenthesis())
                        break;
            
                // Check that there are parentheses found (if not continue) and for stray parentheses
                
                if ((n1 == nodes.begin()) && (n2 == nodes.end()))
                    break;
                
                if ((n1 == nodes.begin()) != (n2 == nodes.end()))
                {
                    reportError("parsing failed - unmatched parenthesis", n1 == nodes.begin() ? n2 : n1 - 1);
                    return kParseError_UnmatchedParenthesis;
                }
                
                // Copy child nodes, determine if this is a function call and how many items it requires
                // N.B. - here n1 is decremented to indicate the actual LHS parenthesis
                
                NodeList childNodes(n1--, n2);
                size_t numItems = 1;
                size_t pos = n1->getCharEnd();
                
                if ((n1 != nodes.begin()) && (n1 - 1)->isSymbol())
                {
                    // Check that a function is found
                    
                    if (!(function = mFunctions.getItem((n1 - 1)->getTokenString())))
                    {
                        reportError("parsing failed - unknown function: #", n1 - 1, (n1 - 1)->getTokenString());
                        return kParseError_NotFunction;
                    }
                    
                    numItems = function->numItems();
                    n1--;
                }
                
                // Parse the child nodes and then the function (if it is a function)
                
                if ((error = recursiveParse(graph, childNodes, numItems, pos)))
                    return error;
                
                 if (function)
                     parseFunction(graph, function, childNodes, n1, n2);
                
                // Copy the result and then erase the unneeded nodes from this level
                
                *n1 = childNodes[0];
                n2 = nodes.erase(n1 + 1, n2 + 1);
            }
            
            // Commas
            
            size_t numCommas = 0;
            
            for (n1 = nodes.begin(); n1 != nodes.end(); n1++)
            {
                // Find the next comma
                
                for (n2 = n1; n2 != nodes.end(); n2++)
                {
                    if (n2->isComma())
                    {
                        if (++numCommas >= resultItems)
                        {
                            reportError("parsing failed - unexpected comma", n2->getCharSpan());
                            return kParseError_TooManyCommas;
                        }
                        break;
                    }
                }
             
                // Check that the comma is not at the start or end of the nodes
                
                if (n2 == nodes.begin() || n2 == std::prev(nodes.end()))
                {
                    CharSpan span(n2 == nodes.begin() ? n1->getCharPos() : n2->getCharEnd());
                    reportError("parsing failed - empty sub expression", span);
                    return kParseError_EmptySubExpr;
                }
       
                // Copy the child nodes and erase from this level, then parse the remaining operators
                
                NodeList childNodes(n1, n2);
                nodes.erase(n1 + 1, (n2 == nodes.end()) ? n2 : n2 + 1);
                if ((error = parseOperators(graph, *n1, childNodes)))
                    return error;
            }
            
            // Check that we have the required number of nodes
            
            if (nodes.size() < resultItems)
            {
                CharSpan span(nodes.front().getCharPos(), nodes.back().getCharEnd());
                reportError("parsing failed - too few arguments for function", span);
                return kParseError_TooFewCommas;
            }
            
            return kNoError;
        }
        
    private:

        FrameLib_ErrorReporter* mReporter;
        FrameLib_Proxy* mProxy;
        const char *mExpr;
        
        std::vector<OperatorList> mOperators;
        OperatorList mFunctions;
        std::vector<Constant> mConstants;
        
        std::vector<char> mOperatorCharList;
        size_t mMaxLengthOp;
        
        T mDefaultConstant;
    };
}

#endif
