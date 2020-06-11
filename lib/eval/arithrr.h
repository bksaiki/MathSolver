#ifndef _MATHSOLVER_ARITH_RR_H_
#define _MATHSOLVER_ARITH_RR_H_

#include "../common/base.h"
#include "../expr/arithmetic.h"

namespace MathSolver
{

ExprNode* rewriteNeg(ExprNode* op);
ExprNode* rewriteAdd(ExprNode* op);
ExprNode* rewriteSub(ExprNode* op);
ExprNode* rewriteMul(ExprNode* op);
ExprNode* rewriteDiv(ExprNode* op);

ExprNode* rewriteExp(ExprNode* op);
ExprNode* rewriteLog(ExprNode* op);

// Rewrites an expression through rearranging and removing redudant expressions.
ExprNode* rewriteArithmetic(ExprNode* expr);

}

#endif