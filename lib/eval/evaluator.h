#ifndef _MATHSOLVER_EVALUATOR_H_
#define _MATHSOLVER_EVALUATOR_H_

#include "../common/base.h"
#include "../expr/expr.h"
#include "arithmetic.h"

namespace MathSolver
{

// Evaluates a mathematical expression. Returns true on success and false otherwise.
ExprNode* evaluateExpr(ExprNode* expr);

}

#endif