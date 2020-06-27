#ifndef _MATHSOLVER_BOOLEAN_H_
#define _MATHSOLVER_BOOLEAN_H_

#include "../common/base.h"
#include "../expr/expr.h"

namespace MathSolver
{

ExprNode* booleanBinary(ExprNode* expr, const std::string& op);

// Evaluators
ExprNode* booleanNot(ExprNode* expr);
inline ExprNode* booleanOr(ExprNode* expr) { return booleanBinary(expr, "or"); }
inline ExprNode* booleanXor(ExprNode* expr) { return booleanBinary(expr, "xor"); }
inline ExprNode* booleanAnd(ExprNode* expr) { return booleanBinary(expr, "and"); }

// Evaluates a boolean expression and returns the result.
ExprNode* evaluateBooleanExpr(ExprNode* expr, int data);

// Returns true if the expression contains only boolean logic.
bool isBooleanExpr(ExprNode* node);

}

#endif