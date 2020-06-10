#include "../expr/arithmetic.h"
#include "arithmetic.h"
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

    ExprNode* simplified = evaluateExprR(expr, true);
    if (containsType(simplified, ExprNode::FLOAT))
        return evaluateExprR(simplified, false);
    return simplified;
}

}