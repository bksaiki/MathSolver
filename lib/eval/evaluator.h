#ifndef _MATHSOLVER_EVALUATOR_H_
#define _MATHSOLVER_EVALUATOR_H_

#include "../common/base.h"
#include "../expr/expr.h"

namespace MathSolver
{
    
// Templated evaluation layer that takes an expression, an evaluator function, and a data
// structure or type.
template <typename Func, typename Data>
ExprNode* evaluateExprLayer(ExprNode* expr, const Func& func, const Data& data)
{
    size_t childCount = expr->children().size();
    for (size_t i = 0; i < childCount; ++i)
    {
        ExprNode* child = expr->children().front();
        expr->children().pop_front();
        child = evaluateExprLayer(child, func, data);
        expr->children().push_back(child);
    }
  
    return func(expr, data);
}


// Evaluates a mathematical expression and returns the result.
ExprNode* evaluateExpr(ExprNode* expr);

}

#endif