#include <algorithm>
#include "../expr/arithmetic.h"
#include "evaluator.h"
#include "inequality.h"
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

ExprNode* rangeSetBuild(ExprNode* op)
{
    if (op->children().size() != 2)     
    {
        gErrorManager.log("Arity mismatch: " + toInfixString(op) + " , expected 2 arguments", ErrorManager::ERROR, __FILE__, __LINE__); 
        return op;
    }

    std::list<std::string> vars = extractVariables(op->children().back());
    ExprNode* lhs = op->children().front();
    bool isValid = true;
    if (lhs->type() == ExprNode::VARIABLE)
    {
        if (std::find(vars.begin(), vars.end(), ((VarNode*)lhs)->name()) == vars.end())
            isValid = false;
    }

    if (!isValid)
    {
        ExprNode* node = new BoolNode(false, op->parent());
        freeExpression(op);
        return node;
    }

    return op;
}

//
//  Range evaluator
//

ExprNode* evaluateRange(ExprNode* expr, int data)
{
    if (expr->type() == ExprNode::RANGE)        return expr;

    if (expr->isOperator())
    {
        OpNode* op = (OpNode*)expr;
        if (op->name() == "or")         return rangeOr(expr);
        if (op->name() == "and")        return rangeAnd(expr);
    }
    else if (expr->isSyntax())
    {
        SyntaxNode* syntax = (SyntaxNode*)expr;
        if (syntax->name() == "|")          return rangeSetBuild(expr);
    }

    if (isInequality(expr))     return evaluateExpr(expr);
    if (isArithmetic(expr))     return evaluateExpr(expr);

    gErrorManager.log("Unimpemented operation: " + toInfixString(expr), ErrorManager::ERROR, __FILE__, __LINE__);
    return expr;
}

bool isRangeExpr(ExprNode* expr)
{
    if (expr->isOperator() &&
        (((OpNode*)expr)->name() == "or" || ((OpNode*)expr)->name() == "and"))
    {
        return std::all_of(expr->children().begin(), expr->children().end(), isRangeExpr);
    }

    if (expr->isSyntax())
    {
        return (((SyntaxNode*)expr)->name() == "|" && expr->children().front()->type() == ExprNode::VARIABLE &&
                isInequality(expr->children().back()));
    }

    return expr->type() == ExprNode::RANGE;
}
    
}