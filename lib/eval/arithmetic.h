#ifndef _MATHSOLVER_ARITHMETIC_H_
#define _MATHSOLVER_ARITHMETIC_H_

#include "../common/base.h"
#include "../expr/expr.h"

namespace MathSolver
{

// Evaluates an arithmetic expression. Returns true upon success.
ExprNode* evaluateArithmetic(ExprNode* expr);

//
// Numeric evaluators
//

ExprNode* numericNeg(ExprNode* op);
ExprNode* numericAdd(ExprNode* op);
ExprNode* numericSub(ExprNode* op);
ExprNode* numericMul(ExprNode* op);
ExprNode* numericDiv(ExprNode* op);

// ExprNode* numericExp(ExprNode* op);
// ExprNode* numericLog(ExprNode* op);
// ExprNode* numericSin(ExprNode* op);
// ExprNode* numericCos(ExprNode* op);
// ExprNode* numericTan(ExprNode* op);

//
// Symbolic evaluators
//

ExprNode* symbolicNeg(ExprNode* op);
ExprNode* symbolicAdd(ExprNode* op);
ExprNode* symbolicSub(ExprNode* op);
ExprNode* symbolicMul(ExprNode* op);
ExprNode* symbolicDiv(ExprNode* op);

ExprNode* symbolicExp(ExprNode* op);
ExprNode* symbolicLog(ExprNode* op);
ExprNode* symbolicSin(ExprNode* op);
ExprNode* symbolicCos(ExprNode* op);
ExprNode* symbolicTan(ExprNode* op);

}

#endif