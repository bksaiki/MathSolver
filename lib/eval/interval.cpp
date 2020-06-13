#include "interval.h"

namespace MathSolver
{

ExprNode* rangeOr(ExprNode* op)
{
    if (op->children().size() < 2)     
    {
        gErrorManager.log("Arity mismatch: " + toInfixString(op) + " , expected at least 2 arguments", ErrorManager::ERROR, __FILE__, __LINE__); 
        return op;
    }

    Range accum = ((RangeNode*)op->children().front())->value();
    for (auto itr = std::next(op->children().begin()); itr != op->children().end(); ++itr)
        accum = accum.disjoin(((RangeNode*)*itr)->value());
    
    RangeNode* ret = new RangeNode(accum, op->parent());
    freeExpression(op);
    return ret;
}

ExprNode* rangeAnd(ExprNode* op)
{
    if (op->children().size() < 2)     
    {
        gErrorManager.log("Arity mismatch: " + toInfixString(op) + " , expected at least 2 arguments", ErrorManager::ERROR, __FILE__, __LINE__); 
        return op;
    }

    Range accum = ((RangeNode*)op->children().front())->value();
    for (auto itr = std::next(op->children().begin()); itr != op->children().end(); ++itr)
        accum = accum.conjoin(((RangeNode*)*itr)->value());
    
    RangeNode* ret = new RangeNode(accum, op->parent());
    freeExpression(op);
    return ret;
}

ExprNode* evaluateRange(ExprNode* expr, int data)
{
    if (expr->type() == ExprNode::RANGE)        return expr;

    if (expr->isOperator())
    {
        OpNode* op = (OpNode*)expr;
        if (op->name() == "or")         return rangeOr(expr);
        if (op->name() == "and")        return rangeAnd(expr);
    }

    gErrorManager.log("Unimpemented operation: " + toInfixString(expr), ErrorManager::ERROR, __FILE__, __LINE__);
    return expr;
}

bool isRangeExprNode(ExprNode* node)
{
    if (node->isOperator())
        return ((OpNode*)node)->name() == "or" || ((OpNode*)node)->name() == "and";
    return node->type() == ExprNode::RANGE;
}
    
}