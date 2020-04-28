#include "evaluator.h"

namespace MathSolver
{

bool evaluateExpr(ExprNode* expr)
{   
    for (ExprNode* child : expr->children)
    {
        if (!isValue(child) && !evaluateExpr(child))
            return false;
    }

    if (isArithmetic(expr)) return evaluateArithmetic(expr);

    return true;
}

}