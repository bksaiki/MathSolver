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
bool isArithmeticNode(ExprNode* node);

// Returns true if every node in the expression is arithmetic:
inline bool isArithmetic(ExprNode* expr) { return containsAll(expr, isArithmeticNode); }

// Returns true if the expression contains inexact (Float) subexpressions.
inline bool isInexact(ExprNode* expr) { return containsAll(expr, [](ExprNode* node) { return node->type() != ExprNode::FLOAT; }); }

// Returns true if the expression is undefined
inline bool isUndef(ExprNode* expr) { return expr->type() == ExprNode::CONSTANT && ((ConstNode*)expr)->name() == "undef"; }

// Finds the common term between two monomial expressions.
std::list<ExprNode*> commonTerm(ExprNode* expr1, ExprNode* expr2);

// Finds the coefficient of an expression given a base term.
std::list<ExprNode*> coeffTerm(ExprNode* expr, ExprNode* term);

// Removes a list of term from an expression
void removeTerm(ExprNode* expr, const std::list<ExprNode*>& terms);

// Assumes the expression is in the form x^n or x and returns a copy of x,
ExprNode* extractPowBase(ExprNode* op);

// Assumes the expression is in the form x^n or x and returns a copy of n or 1, respectively.
ExprNode* extractPowExp(ExprNode* op);

// Assumes the expression is in the form x^n or x and returns x directly.
ExprNode* peekPowBase(ExprNode* op);

// Assumes the expression is in the form x^n or x and returns n or 1 directly.
ExprNode* peekPowExp(ExprNode* op);

}

#endif