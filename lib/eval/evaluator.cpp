#include "evaluator.h"

namespace MathSolver
{

bool evaluateExpr(ExpressionNode* expr)
{   
    for (ExpressionNode* child : expr->mChildren)
    {
        if (!isValue(child) && !evaluateExpr(child))
            return false;
    }

    if (isArithmetic(expr)) return evaluateArithmetic(expr);

    return true;
}

}