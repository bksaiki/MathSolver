#ifndef _MATHSOLVER_INEQUALITYRR_H_
#define _MATHSOLVER_INEQUALITYRR_H_

#include "../expr/expr.h"

namespace MathSolver
{

// Rewrite rule for >, <, >=, <=.
ExprNode* rewriteInequalityCommon(ExprNode* op);

// Rewrite rule for !=.
ExprNode* rewriteInequalityNotEq(ExprNode* op);

// Rewrite an inequality expression by expanding chained notation
// into multiple binary expressions
ExprNode* rewriteInequality(ExprNode* expr, int data);

}

#endif