#ifndef _MATHSOLVER_EXPRESSION_H_
#define _MATHSOLVER_EXPRESSION_H_

#include <string>
#include <list>
#include "../types/Integer.h"

namespace MathSolver
{

//
//  Expression operations
//

// Stores unique identifiers
struct ExpressionNode
{   
    enum Type
    {
        VARIABLE = 0,
        OPERATOR,
        FUNCTION,
        CONSTANT,
        INTEGER,
        FLOAT
    };

    // Default constructor
    ExpressionNode();

    // Direct copy unsupported
    ExpressionNode(const ExpressionNode& other) = delete;
    ExpressionNode& operator=(const ExpressionNode& other) = delete;

    ExpressionNode* mParent;
    std::list<ExpressionNode*> mChildren;
    Type mType;
    int mPrecedence;

    std::string mStr;
    Integer mExact;
    double mInexact;
};

// Assigns the expression node at source to dest by copying every member except for parent.
void assignExprNode(ExpressionNode* dest, ExpressionNode* src);

// Returns a copy of the given expression tree
ExpressionNode* copyOf(ExpressionNode* expr);

// Returns true if the value of two nodes is the same.
bool equivExpression(ExpressionNode* a, ExpressionNode* b);

// Takes an expression node and recursively simplifes certain operators with interior nodes of the
// same operator into a single operator with many operands. e.g. (+ (+ a (+ b c)) d) ==> (+ a b c d)
void flattenExpr(ExpressionNode* expr);

// Deletes an expression tree.
void freeExpression(ExpressionNode* expr);

// Returns true if the expression only contains numerical operands (Non-symbolic expression).
bool isNumerical(ExpressionNode* expr);

// Returns true if the node is a number.
bool isNumber(ExpressionNode* node);

// Returns true if the node is a number, constant, or variable
bool isValue(ExpressionNode* node);

// Returns the number of nodes in the expression tree.
size_t nodeCount(ExpressionNode* expr);

// Returns an expression tree as a string in infix notation. Assumes the tree is valid.
std::string toInfixString(ExpressionNode* expr);

// Returns an expression tree as a string in prefix notation. Assumes the tree is valid.
std::string toPrefixString(ExpressionNode* expr);

//
// Language constructs
/*
    Operator precedence
    0   numbers, constants  --
    1   functions           RL
    2   -*                  RL
    3   ^                   RL
    4   !                   LR
    5   **                  LR
    6   * / %               LR
    7   +-                  LR
    8   > < >= < == !=      --

    N   , | () [] {}
*/

// Returns true if the string is a predefined or user-defined function.
bool isFunction(const std::string& func);

// Returns true if the character is an operator.
bool isOperatorChar(char c);

// Returns true if the character is a bracket: (), {}, [].
bool isBracket(char c);

// Returns the precedence of the operator. Returns 0 if the string is not
// an operator.
int operatorPrec(const std::string& op);

}

#endif
