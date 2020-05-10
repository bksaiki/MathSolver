#include "evaluator.h"

namespace MathSolver
{

ExprNode* evaluateExpr(ExprNode* expr)
{   
    size_t childCount = expr->children().size();
    for (size_t i = 0; i < childCount; ++i)
    {
        ExprNode* child = expr->children().front();
        expr->children().pop_front();
        child = evaluateExpr(child);
        expr->children().push_back(child);
    }

    if (isArithmetic(expr)) return evaluateArithmetic(expr);

    return expr;
}

}