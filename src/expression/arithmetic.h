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

// Evaluates an arithmetic expression. Returns true upon success.
bool evaluateArithmetic(ExpressionNode* expr);

}

#endif