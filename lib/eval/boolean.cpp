#include <algorithm>
#include "boolean.h"

namespace MathSolver
{

ExprNode* booleanNot(ExprNode* expr)
{
    if (expr->children().size() != 1)     
    {
        gErrorManager.log("Arity mismatch: " + toInfixString(expr) + " , expected 1 argument", ErrorManager::ERROR, __FILE__, __LINE__); 
        return expr;
    }    

    if (expr->children().front()->type() != ExprNode::BOOLEAN)
    {
        gErrorManager.log("Expected a boolean expression: " + toInfixString(expr),
                           ErrorManager::ERROR, __FILE__, __LINE__);
        return expr;
    }

    ExprNode* res = new BoolNode(!((BoolNode*)expr->children().front())->value(), expr->parent());
    freeExpression(expr);
    return res;
}

ExprNode* booleanBinary(ExprNode* expr, const std::string& op)
{
    if (expr->children().size() < 2)     
    {
        gErrorManager.log("Arity mismatch: " + toInfixString(expr) + " , expected at least 2 arguments", ErrorManager::ERROR, __FILE__, __LINE__); 
        return expr;
    }    

    auto pred = [](ExprNode* node) { return node->type() != ExprNode::BOOLEAN; };
    if (std::any_of(expr->children().begin(), expr->children().end(), pred))
    {
        gErrorManager.log("Expected a boolean expression: " + toInfixString(expr),
                           ErrorManager::ERROR, __FILE__, __LINE__);
        return expr;
    }

    bool accum = new BoolNode(((BoolNode*)expr->children().front())->value(), expr->parent());
    for (auto it = std::next(expr->children().begin()); it != expr->children().end(); ++it)
    {
        if (op == "or")          accum = accum || ((BoolNode*)*it)->value();
        else if (op == "xor")    accum = accum != ((BoolNode*)*it)->value();
        else if (op == "and")    accum = accum && ((BoolNode*)*it)->value();
    }        

    ExprNode* res = new BoolNode(accum, expr->parent());
    freeExpression(expr);
    return res;
}

ExprNode* evaluateBooleanExpr(ExprNode* expr, int data)
{
    if (expr->type() == ExprNode::BOOLEAN)  return expr;

    if (expr->isOperator())
    {
        OpNode* op = (OpNode*)expr;
        if (op->name() == "not")        return booleanNot(expr);
        if (op->name() == "or")         return booleanOr(expr);
        if (op->name() == "xor")        return booleanXor(expr);
        if (op->name() == "and")        return booleanAnd(expr);
    }

    gErrorManager.log("Unimpemented operation: " + toInfixString(expr), ErrorManager::ERROR, __FILE__, __LINE__);
    return expr;
}

bool isBooleanExpr(ExprNode* expr)
{
    if (expr->isOperator() && 
        (((OpNode*)expr)->name() == "not" || ((OpNode*)expr)->name() == "and" ||
         ((OpNode*)expr)->name() == "or" || ((OpNode*)expr)->name() == "xor"))
    {
        return std::all_of(expr->children().begin(), expr->children().end(), isBooleanExpr);
    }

    return expr->type() == ExprNode::BOOLEAN;
}

}