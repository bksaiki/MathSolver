#ifndef _MATHSOLVER_EXPRESSION_H_
#define _MATHSOLVER_EXPRESSION_H_

#include <string>
#include <vector>
#include "../types/Integer.h"

namespace MathSolver
{

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
    std::vector<ExpressionNode*> mChildren;
    Type mType;
    int mPrecedence;

    std::string mStr;
    Integer mExact;
    double mInexact;
};

//
// Language constructs
//

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
