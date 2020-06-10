#ifndef _MATHSOLVER_INTERVAL_H_
#define _MATHSOLVER_INTERVAL_H_

#include "../common/base.h"
#include "../expr/expr.h"

namespace MathSolver
{

// Evaluates the given Range expression and returns the result.
ExprNode* evaluateRange(ExprNode* expr);

// Returns true if the node is:
//  (i) a Range type
//  (ii) 'or', 'and'
bool isRangeExprNode(ExprNode* node);

// Returns true if the expression can be simplified into a range.
inline bool isRangeExpr(ExprNode* expr) { return containsAll(expr, isRangeExprNode); }

}

#endif