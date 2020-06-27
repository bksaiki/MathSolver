#include "../expr/arithmetic.h"
#include "arithmetic.h"
#include "arithrr.h"
#include "evaluator.h"
#include "inequality.h"
#include "interval.h"

namespace MathSolver
{

ExprNode* evaluateExpr(ExprNode* expr)
{ 
    if (expr == nullptr)        return expr;
    if (isUndef(expr))          return expr;

    if (isArithmetic(expr))
    {
        expr = evaluateExprLayer(expr, rewriteArithmetic, 0);
        expr = evaluateExprLayer(expr, evaluateArithmetic, true);
        return evaluateExprLayer(expr, evaluateArithmetic, false);
    }

    if (isInequality(expr))     return evaluateExprLayer(expr, evaluateInequality, 0);
    if (isRangeExpr(expr))      return evaluateExprLayer(expr, evaluateRange, 0);
    
    gErrorManager.log("Unrecognized expression: " + toInfixString(expr), ErrorManager::ERROR, __FILE__, __LINE__);
    return expr;
}

}