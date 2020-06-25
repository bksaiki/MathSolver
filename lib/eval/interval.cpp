#include <algorithm>
#include "../expr/arithmetic.h"
#include "evaluator.h"
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
        gErrorManager.log("Invalid inequality: " + toInfixString(op), ErrorManager::ERROR);
        return op;
    }

    return op;
}

bool cmpNumbers(ExprNode* lhs, ExprNode* rhs, const std::string& op)
{
    if (lhs->type() == ExprNode::INTEGER && rhs->type() == ExprNode::INTEGER)
    {
        if (op == ">")   return ((IntNode*)lhs)->value() > ((IntNode*)rhs)->value();
        if (op == "<")   return ((IntNode*)lhs)->value() < ((IntNode*)rhs)->value();
        if (op == ">=")  return ((IntNode*)lhs)->value() >= ((IntNode*)rhs)->value();
        if (op == "<=")  return ((IntNode*)lhs)->value() <= ((IntNode*)rhs)->value();
        if (op == ">=")  return ((IntNode*)lhs)->value() >= ((IntNode*)rhs)->value();
        else /* != */    return ((IntNode*)lhs)->value() != ((IntNode*)rhs)->value();
    }

    Float f1 = (lhs->type() == ExprNode::INTEGER) ? Float(((IntNode*)lhs)->value().toString()) : ((FloatNode*)lhs)->value();
    Float f2 = (rhs->type() == ExprNode::INTEGER) ? Float(((IntNode*)rhs)->value().toString()) : ((FloatNode*)rhs)->value();
    if (op == ">")      return f1 > f2;
    if (op == "<")      return f1 > f2;
    if (op == ">=")     return f1 > f2;
    if (op == "<=")     return f1 > f2;
    else /* != */       return f1 != f2;
}

ExprNode* rangeCompare(ExprNode* expr, const std::string& op)
{
    if (expr->children().size() < 2)     
    {
        gErrorManager.log("Arity mismatch: " + toInfixString(expr) + " , expected at least 2 arguments", ErrorManager::ERROR, __FILE__, __LINE__); 
        return expr;
    }

    bool validRange = true;
    for (auto it = expr->children().begin(); it != std::prev(expr->children().end()); ++it)
    {
        for (auto it2 = std::next(it); it2 != expr->children().end(); ++it2)
        {
            if ((*it)->isNumber() && (*it2)->isNumber() && cmpNumbers(*it, *it2, op))
                validRange = false;
        }
    }
    
    if (!validRange)
    {
        gErrorManager.log("Invalid inequality: " + toInfixString(expr), ErrorManager::ERROR);
        return expr;
    }

    return expr;
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
        if (op->name() == ">")          return rangeGreater(expr);
        if (op->name() == "<")          return rangeLess(expr);
        if (op->name() == ">=")         return rangeGreaterEq(expr);
        if (op->name() == "<=")         return rangeLessEq(expr);
        if (op->name() == "!=")         return rangeNotEq(expr);
    }
    else if (expr->isSyntax())
    {
        SyntaxNode* syntax = (SyntaxNode*)expr;
        if (syntax->name() == "|")          return rangeSetBuild(expr);
    }

    if (isArithmetic(expr)) return evaluateExpr(expr);

    gErrorManager.log("Unimpemented operation: " + toInfixString(expr), ErrorManager::ERROR, __FILE__, __LINE__);
    return expr;
}

bool isComparatorNode(ExprNode* node)
{
    if (!node->isOperator())    return false;

    OpNode* op = (OpNode*)node;
    return (op->name() == "!=" || op->name() == "==" || 
            op->name() == ">" || op->name() == "<" || op->name() == ">=" || op->name() == "<=");
}

bool isRangeExprNode(ExprNode* node)
{
    if (node->isOperator())
    {
        return ((OpNode*)node)->name() == "or" || ((OpNode*)node)->name() == "and" || isComparatorNode(node);
    }
    if (node->isSyntax())
    {
        return ((SyntaxNode*)node)->name() == "|" && node->children().front()->type() == ExprNode::VARIABLE &&
                (isComparatorNode(node->children().back()) || isRangeExprNode(node->children().back()));
    }

    return node->type() == ExprNode::RANGE;
}

bool isRangeExpr(ExprNode* expr)
{
    if (!isRangeExprNode(expr))   return false;

    auto pred = [](ExprNode* node) { return isRangeExprNode(node) || isArithmetic(node); };
    return containsAll(expr, pred);
}
    
}