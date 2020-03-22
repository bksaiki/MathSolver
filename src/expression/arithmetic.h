#ifndef _MATHSOLVER_ARITHMETIC_H_
#define _MATHSOLVER_ARITHMETIC_H_

#include "expression.h"

namespace MathSolver
{

// Returns true if the expression only contains the following:
//      numbers, constants, variables,
//      operators: +, -, *, /, %, !, ^
//      functions: exp, log, sin, cos, tan
// Assumes the expression is valid.
bool isArithmetic(ExpressionNode* expr);

// Finds the common term between two monomial expressions.
std::list<ExpressionNode*> commonTerm(ExpressionNode* expr1, ExpressionNode* expr2);

// Finds the coefficient of an expression given a base term.
std::list<ExpressionNode*> coeffTerm(ExpressionNode* expr, ExpressionNode* term);

// Evaluates an arithmetic expression. Returns true upon success.
bool evaluateArithmetic(ExpressionNode* expr);

}

#endif