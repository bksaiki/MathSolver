#ifndef _MATHSOLVER_INEQUALITY_
#define _MATHSOLVER_INEQUALITY_

#include "../common/base.h"
#include "../expr/expr.h"

namespace MathSolver
{

// Extracts a Range from an expression of the form (?= x a), (?= a x), or (?= a x b) where 'a', 'b'
// are numbers and '?=' is a comparator operator. Returns the null set otherwise.
Range extractRange(ExprNode* expr);

// Converts a univariate interval to an expression.
ExprNode* toExpression(const interval_t& ival, const std::string& var);

// Converts a univariate Range to an expression.
ExprNode* toExpression(const Range& range, const std::string& var);

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
ExprNode* evaluateInequality(ExprNode* expr, int data);

// Returns true if the expression is an inequality.
bool isInequality(ExprNode* expr);

}

#endif