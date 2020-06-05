#ifndef _MATHSOLVER_ARITHMETIC_EXPR_H_
#define _MATHSOLVER_ARITHMETIC_EXPR_H_

#include "../common/base.h"
#include "../expr/expr.h"

namespace MathSolver
{

// Returns true if the node only contains the following:
//      numbers, constants, variables,
//      operators: +, -, *, /, % (mod), !, ^
//      functions: exp, log, sin, cos, tan
// Assumes the expression is valid.
bool nodeIsArithmetic(ExprNode* node);

// Returns true if every node in the expression is arithmetic:
inline bool isArithmetic(ExprNode* expr) { return containsAll(expr, nodeIsArithmetic); }

// Returns true if the expression contains inexact (Float) subexpressions.
inline bool isInexact(ExprNode* expr) { return containsAll(expr, [](ExprNode* node) { return node->type() != ExprNode::FLOAT; }); }

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

// Applies arithmetic rewrite rules to make evaluation easier
//   (- a b c ...) ==> (+ a (- b) (- c) ...)
void arithmeticRewrite(ExprNode* op);

}

#endif