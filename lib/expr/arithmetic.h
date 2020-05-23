#ifndef _MATHSOLVER_ARITHMETIC_EXPR_H_
#define _MATHSOLVER_ARITHMETIC_EXPR_H_

#include "../common/base.h"
#include "../expr/expr.h"

namespace MathSolver
{

// Returns true if the expression only contains the following:
//      numbers, constants, variables,
//      operators: +, -, *, /, % (mod), !, ^
//      functions: exp, log, sin, cos, tan
// Assumes the expression is valid.
bool isArithmetic(ExprNode* expr);

// Finds the common term between two monomial expressions.
std::list<ExprNode*> commonTerm(ExprNode* expr1, ExprNode* expr2);

// Finds the coefficient of an expression given a base term.
std::list<ExprNode*> coeffTerm(ExprNode* expr, ExprNode* term);

// Assumes the expression is in the form x^n or x and returns x, the base that is being
// raised to the power of n or 1, respectively.
ExprNode* getPowBase(ExprNode* op);

// Assumes the expression is in the form x^n or x and returns n or 1, respectively. If it is the
// latter case, a new expression node is created.
ExprNode* getPowExp(ExprNode* op);

}

#endif