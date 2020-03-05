#ifndef _MATHSOLVER_EXPRESSION_H_
#define _MATHSOLVER_EXPRESSION_H_

#include <vector>
#include "../types/Integer.h"

namespace MathSolver
{

// Stores unique identifiers
struct ExpressionNode
{
    ExpressionNode* parent;
    std::vector<ExpressionNode*> children;
    int precedence;

    std::string str;
    Integer natural;
    double inexact;
};

//
// Language constructs
//

/*
    Operator precedence
    1   functions       RL
    2   (), {}, []      RL
    3   ^               RL
    4   !               LR
    5   a(b)            LR
    6   *,/,%           LR
    7   +,-             LR
    8   value           --
*/

// Returns true if the character is an operator.
bool isOperator(char c);

// Returns true if the character is a bracket: (), {}, [].
bool isBracket(char c);

}

#endif
