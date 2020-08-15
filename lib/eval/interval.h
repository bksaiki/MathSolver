#ifndef _MATHSOLVER_INTERVAL_H_
#define _MATHSOLVER_INTERVAL_H_

#include "../common/base.h"
#include "../expr/expr.h"

namespace MathSolver
{

// Range operators
ExprNode* rangeOr(ExprNode* op);
ExprNode* rangeAnd(ExprNode* op);
ExprNode* rangeSetBuild(ExprNode* op);

// Evaluates the given Range expression and returns the result.
ExprNode* evaluateRange(ExprNode* expr, int data = 0);

// Returns true if the node is a comparator.
bool isComparatorNode(ExprNode* node);

// Returns true if the expression can be simplified into a range.
//  (i)     Range type, e.g. [0, 1]
//  (ii)    'or', 'and'
//  (iii)   Set builder, e.g. { x | x > 5 }  
bool isRangeExpr(ExprNode* expr);

}

#endif