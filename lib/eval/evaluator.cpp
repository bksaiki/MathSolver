#include "../expr/arithmetic.h"
#include "arithmetic.h"
#include "arithrr.h"
#include "evaluator.h"
#include "interval.h"

namespace MathSolver
{

ExprNode* evaluateExprR(ExprNode* expr, bool firstPass)
{   
    size_t childCount = expr->children().size();
    for (size_t i = 0; i < childCount; ++i)
    {
        ExprNode* child = expr->children().front();
        expr->children().pop_front();
        child = evaluateExprR(child, firstPass);
        expr->children().push_back(child);
    }
  
    if (isArithmetic(expr))     return evaluateArithmetic(expr, firstPass);
    if (isRangeExpr(expr))      return evaluateRange(expr);

    gErrorManager.log("Unrecognized expression: " + toInfixString(expr), ErrorManager::ERROR, __FILE__, __LINE__);
    return expr;
}

ExprNode* evaluateExpr(ExprNode* expr)
{ 
    if (expr == nullptr)
        return expr;

    ExprNode* eval = evaluateExprR(expr, true);
    return evaluateExprR(eval, false);
}

ExprNode* rewriteExprR(ExprNode* expr)
{   
    size_t childCount = expr->children().size();
    for (size_t i = 0; i < childCount; ++i)
    {
        ExprNode* child = expr->children().front();
        expr->children().pop_front();
        child = rewriteExprR(child);
        expr->children().push_back(child);
    }
  
    if (isArithmetic(expr))     return rewriteArithmetic(expr);
    if (isRangeExpr(expr))      return expr;

    gErrorManager.log("Unrecognized expression: " + toInfixString(expr), ErrorManager::ERROR, __FILE__, __LINE__);
    return expr;
}

ExprNode* rewriteExpr(ExprNode* expr)
{ 
    if (expr == nullptr)
        return expr;

    return rewriteExprR(expr);
}

}