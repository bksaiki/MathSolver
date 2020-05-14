#ifndef _MATHSOLVER_ARITHMETIC_EXPR_H_
#define _MATHSOLVER_ARITHMETIC_EXPR_H_

#include "../common/base.h"
#include "../expr/expr.h"

namespace MathSolver
{

// Returns true if the expression only contains the following:
//      numbers, constants, variables,
//      operators: +, -, *, /, %, !, ^
//      functions: exp, log, sin, cos, tan
// Assumes the expression is valid.
bool isArithmetic(ExprNode* expr);

// Finds the common term between two monomial expressions.
std::list<ExprNode*> commonTerm(ExprNode* expr1, ExprNode* expr2);

// Finds the coefficient of an expression given a base term.
std::list<ExprNode*> coeffTerm(ExprNode* expr, ExprNode* term);

}

#endif