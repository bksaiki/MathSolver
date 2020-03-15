#ifndef _MATHSOLVER_EVALUATOR_H_
#define _MATHSOLVER_EVALUATOR_H_

#include "arithmetic.h"
#include "expression.h"

namespace MathSolver
{

// Evaluates a mathematical expression. Returns true on success and false otherwise.
bool evaluateExpr(ExpressionNode* expr);

}

#endif