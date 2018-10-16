
#ifndef FRAMELIB_EXPRPARSER_H
#define FRAMELIB_EXPRPARSER_H

#include "FrameLib_Types.h"

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
        kParseError_StrayParenthesis,
        kParseError_NotFunction,
        kParseError_UnknownToken,
        kParseError_TooFewCommas,
        kParseError_TooManyCommas,
        kParseError_OpPosition,
        kParseError_StrayItem
    };

    // A base class for functions and operators

    template <class T>
    struct OpBase
    {
        OpBase(const char* name, int precedence) : mName(name), mPrecedence(precedence) {}
        virtual ~OpBase() {}
        
        virtual int numItems() const = 0;
        virtual T call(T a, T b, T c) const = 0;
        virtual FrameLib_DSP *create(FrameLib_Context context) const = 0;

        const char* mName;
        int mPrecedence;
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
            Node(Token token) : mType(kToken), mToken(token), mValue(0.0), mIndex(-1) {}
            Node(T constant) : mType(kConstant), mValue(constant), mIndex(-1) {}
            Node(IndexType type, long index) : mType(type == kIsInput ? kVariable : kResult), mValue(0.0), mIndex(index) {}
            
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
            
            // Input Token Conversion
            
            long convertInputTokens()
            {
                if (isSymbol() && !mToken.mText.compare(0, 2, "in") && mToken.mText.length() > 2)
                {
                    for (size_t i = 2; i < mToken.mText.length(); i++)
                        if (!isDigit(mToken.mText[i]))
                            return -1;
                    
                    *this = Node(kIsInput, std::stol(mToken.mText.substr(2)) - 1);
                }
                
                return mIndex + 1;
            }
            
        private:
            
            NodeType mType;
            Token mToken;
            T mValue;
            long mIndex;
        };
        
        // Typedefs for brevity
        
        typedef std::vector<Node> NodeList;
        typedef typename std::vector<Node>::iterator NodeListIterator;
        
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
            
            const OpBase<T> *getItem(const char *name) const
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
        
        Parser(int precedenceLevels) : mMaxLengthOp(0)
        {
            mOperators.resize(precedenceLevels);
        }
        
        void addOperator(OpBase<T>* op)
        {
            int precedence = op->mPrecedence;
            
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
        
        ExprParseError parse(Graph<T>& graph, const char *expr)
        {
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
                    *it = Node(convertTextToNumber(it->getTokenString()));
                
                // Convert constant symbols to constants
                
                if (it->isSymbol() && getConstant(it->getTokenString(), value))
                    *it = Node(value);
                
                // Convert variable tokens to variables
                
                graph.mNumInputs = std::max(it->convertInputTokens(), graph.mNumInputs);
            }
            
            // Parse
            
            if ((error = recursiveParse(graph, nodes, 1)))
                return error;
            
            if (nodes.size() && nodes[0].isConstant())
                graph.mConstant = nodes[0].getValue();
            
            return kNoError;
        }
        
    private:
        
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
        
        const OpBase<T> *getOperator(const char *name, int precedence) const
        {
            return mOperators[precedence].getItem(name);
        }
        
        const OpBase<T> *getOperator(const char *name) const
        {
            const OpBase<T> *op = nullptr;
            
            for (int i = 0; i < mOperators.size(); i++)
                if ((op = getOperator(name, i)))
                    return op;
            
            return op;
        }
        
        // Parsing Functions
        
        typename Graph<T>::Input parseInput(const Node& arg)
        {
            if (arg.isConstant())
                return typename Graph<T>::Input(arg.getValue());
            if (arg.isVariable())
                return typename Graph<T>::Input(kIsInput, arg.getIndex());
            
            return typename Graph<T>::Input(kIsOutput, arg.getIndex());
        }

        Node parseOperation(Graph<T>& graph, const OpBase<T> *op, const Node& arg1, const Node& arg2, const Node& arg3)
        {
            // Fold constants
            
            if (arg1.isConstant() && arg2.isConstant() && arg3.isConstant())
                return Node(op->call(arg1.getValue(), arg2.getValue(), arg3.getValue()));
            
            // Add to the graph
            
            graph.mOperations.push_back(op);
            
            int numItems = op->numItems();
            if (numItems)       graph.mOperations.back().mIns[0] = parseInput(arg1);
            if (numItems > 1)   graph.mOperations.back().mIns[1] = parseInput(arg2);
            if (numItems > 2)   graph.mOperations.back().mIns[2] = parseInput(arg3);
            
            return Node(kIsOutput, graph.mOperations.size() - 1);
        }
        
        ExprParseError parseUnaryOperator(Graph<T>& graph, const OpBase<T> *op, NodeList& nodes, NodeListIterator& it)
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

        ExprParseError parseBinaryOperator(Graph<T>& graph, const OpBase<T> *op, NodeList& nodes, NodeListIterator& it)
        {
            // Check the operator is not at the beginning or end and the nodes either side are not operators
            
            if (it == nodes.begin() || it == nodes.end() - 1 || (it - 1)->isOperator() || (it + 1)->isOperator())
                return kParseError_OpPosition;
            
            // Parse operation and erase consumed nodes (set the iterator back one, as it will be incremented)
            
            *(it - 1) = parseOperation(graph, op, *(it - 1), *(it + 1), *(it + 1));
            it = nodes.erase(it, it + 2) - 1;
            
            return kNoError;
        }

        void parseFunction(Graph<T>& graph, const OpBase<T> *function, NodeList& nodes)
        {
            int numItems = function->numItems();
            
            if (!numItems) nodes.push_back(Node(0.0));
            
            nodes[0] = parseOperation(graph, function, nodes[0], nodes[numItems > 1 ? 1 : 0], nodes[numItems > 2 ? 2 : 0]);
        }

        ExprParseError parseOperators(Graph<T>& graph, Node &result, NodeList& nodes)
        {
            const OpBase<T> *op = nullptr;
            ExprParseError error = kNoError;

            // Check for any remaining tokens that are not operators

            for (auto it = nodes.begin(); it != nodes.end(); it++)
                if (it->isToken() && !it->isOperator())
                    return kParseError_UnknownToken;
            
            // Now resolve unary operators by searching right-to-left
            
             for (auto it = nodes.end() - 1; it >= nodes.begin(); it--)
                 if ((op = getOperator(it->getTokenString(), 0)))
                    if ((error = parseUnaryOperator(graph, op, nodes, it)))
                        return error;

            // Now resolve binary operators in order of precedence
            
            for (int i = 1; i < mOperators.size(); i++)
            {
                for (auto it = nodes.begin(); it < nodes.end(); it++)
                    if ((op = getOperator(it->getTokenString(), i)))
                        if ((error = parseBinaryOperator(graph, op, nodes, it)))
                            return error;
            }
            
            // Store result and return, checking for stray items
            
            result = nodes[0];
            
            return (nodes.size() != 1 || nodes[0].isToken()) ? kParseError_StrayItem : kNoError;;
        }

        ExprParseError recursiveParse(Graph<T>& graph, NodeList& nodes, int resultItems)
        {
            NodeListIterator n1, n2;
            ExprParseError error = kNoError;
            
            // Check for empty node list
            
            if (nodes.empty())
                return kParseError_EmptySubExpr;
            
            // Parentheses
            
            for (n2 = nodes.begin(); ; )
            {
                const OpBase<T> *function = nullptr;

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
                    
                    if (!(function = mFunctions.getItem((n1 - 1)->getTokenString())))
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
        
    private:

        std::vector<OperatorList> mOperators;
        OperatorList mFunctions;
        std::vector<Constant> mConstants;
        
        std::vector<char> mOperatorCharList;
        size_t mMaxLengthOp;
        
        T mDefaultConstant;
    };
}

#endif
