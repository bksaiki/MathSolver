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
        NUMBER
    };

    // Default constructor
    ExpressionNode();

    ExpressionNode* mParent;
    std::list<ExpressionNode*> mChildren;
    Type mType;
    int mPrecedence;

    std::string mStr;
    Integer mExact;
    double mInexact;
};

// Takes an expression node and recursively simplifes certain operators with interior nodes of the
// same operator into a single operator with many operands. e.g. (+ (+ a (+ b c)) d) ==> (+ a b c d)
void flattenExpr(ExpressionNode* node);

// Deletes an expression tree.
void freeExpression(ExpressionNode* node);

// Returns an expression tree as a string in infix notation. Assumes the tree is valid.
std::string toInfixString(ExpressionNode* node);

// Returns an expression tree as a string in prefix notation. Assumes the tree is valid.
std::string toPrefixString(ExpressionNode* node);


//
// Language constructs
/*
    Operator precedence
    0   numbers, constants  --
    1   functions           RL
    2   ^                   RL
    3   !                   LR
    4   a(b) "**"           LR
    5   * / %               LR
    6   +-                  LR
    7   > < >= < == !=      --

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
