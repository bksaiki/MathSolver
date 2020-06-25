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

// Comparator operators
ExprNode* rangeCompare(ExprNode* expr, const std::string& op);
inline ExprNode* rangeGreater(ExprNode* expr) { return rangeCompare(expr, ">"); }
inline ExprNode* rangeLess(ExprNode* expr) { return rangeCompare(expr, ">="); }
inline ExprNode* rangeGreaterEq(ExprNode* expr) { return rangeCompare(expr, ">"); }
inline ExprNode* rangeLessEq(ExprNode* expr) { return rangeCompare(expr, "<="); }
inline ExprNode* rangeNotEq(ExprNode* expr) { return rangeCompare(expr, "!="); }

// Evaluates the given Range expression and returns the result.
ExprNode* evaluateRange(ExprNode* expr, int data = 0);

// Returns true if the node is a comparator.
bool isComparatorNode(ExprNode* node);

// Returns true if the node is:
//  (i)     a Range type
//  (ii)    'or', 'and'
//  (iii)   comparison (e.g. x > 5)
bool isRangeExprNode(ExprNode* node);

// Returns true if the expression can be simplified into a range.
bool isRangeExpr(ExprNode* expr);

}

#endif