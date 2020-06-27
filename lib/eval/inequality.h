#ifndef _MATHSOLVER_INEQUALITY_
#define _MATHSOLVER_INEQUALITY_

#include "../common/base.h"
#include "../expr/expr.h"

namespace MathSolver
{

// Returns true if the two numbers satisfy the given comparison.
bool compareNumbers(ExprNode* lhs, ExprNode* rhs, const std::string& op);

// Comparator operators
ExprNode* inequalityCompare(ExprNode* expr, const std::string& op);
inline ExprNode* inequalityGreater(ExprNode* expr) { return inequalityCompare(expr, ">"); }
inline ExprNode* inequalityLess(ExprNode* expr) { return inequalityCompare(expr, "<"); }
inline ExprNode* inequalityGreaterEq(ExprNode* expr) { return inequalityCompare(expr, ">="); }
inline ExprNode* inequalityLessEq(ExprNode* expr) { return inequalityCompare(expr, "<="); }
inline ExprNode* inequalityNotEq(ExprNode* expr) { return inequalityCompare(expr, "!="); }

// Combine equalities
ExprNode* inequalityAnd(ExprNode* expr);
ExprNode* inequalityOr(ExprNode* expr);

// Evaluates an inequality
ExprNode* evaluateInequality(ExprNode* expr, int data = 0);

// Returns true if the expression is an inequality.
bool isInequality(ExprNode* expr);

}

#endif