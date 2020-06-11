#ifndef _MATHSOLVER_EVALUATOR_H_
#define _MATHSOLVER_EVALUATOR_H_

#include "../common/base.h"
#include "../expr/expr.h"

namespace MathSolver
{

// Evaluates a mathematical expression and returns the result.
ExprNode* evaluateExpr(ExprNode* expr);

// Simplifies an expression through rewrite rules.
ExprNode* rewriteExpr(ExprNode* expr);

// Rewrites and then evaluates. This should be called unless you want the rewrite result.
inline ExprNode* rrAndEvalExpr(ExprNode* expr) { return evaluateExpr(rewriteExpr(expr)); }

}

#endif