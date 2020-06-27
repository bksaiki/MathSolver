#include <algorithm>
#include "evaluator.h"
#include "inequality.h"
#include "../expr/arithmetic.h"

namespace MathSolver
{

bool compareNumbers(ExprNode* lhs, ExprNode* rhs, const std::string& op)
{
    if (lhs->type() == ExprNode::INTEGER && rhs->type() == ExprNode::INTEGER)
    {
        if (op == ">")   return ((IntNode*)lhs)->value() > ((IntNode*)rhs)->value();
        if (op == "<")   return ((IntNode*)lhs)->value() < ((IntNode*)rhs)->value();
        if (op == ">=")  return ((IntNode*)lhs)->value() >= ((IntNode*)rhs)->value();
        if (op == "<=")  return ((IntNode*)lhs)->value() <= ((IntNode*)rhs)->value();
        if (op == ">=")  return ((IntNode*)lhs)->value() >= ((IntNode*)rhs)->value();
        if (op == ">=")  return ((IntNode*)lhs)->value() != ((IntNode*)rhs)->value();
    }
    else
    {
        Float f1 = (lhs->type() == ExprNode::INTEGER) ? Float(((IntNode*)lhs)->value().toString()) : ((FloatNode*)lhs)->value();
        Float f2 = (rhs->type() == ExprNode::INTEGER) ? Float(((IntNode*)rhs)->value().toString()) : ((FloatNode*)rhs)->value();
        if (op == ">")      return f1 > f2;
        if (op == "<")      return f1 < f2;
        if (op == ">=")     return f1 >= f2;
        if (op == "<=")     return f1 <= f2;
        if (op == "!=")     return f1 != f2;
    }

    gErrorManager.log("Expected a comparator " + toInfixString(lhs) + op + toInfixString(rhs), ErrorManager::ERROR, __FILE__, __LINE__);
    return false;
}

ExprNode* inequalityCompare(ExprNode* expr, const std::string& op)
{
    if (expr->children().size() < 2)     
    {
        gErrorManager.log("Arity mismatch: " + toInfixString(expr) + " , expected at least 2 arguments", ErrorManager::ERROR, __FILE__, __LINE__); 
        return expr;
    }

    bool isValid = true;
    for (auto it = expr->children().begin(); it != std::prev(expr->children().end()); ++it)
    {
        auto it2 = std::next(it);
        if ((*it)->isNumber() && (*it2)->isNumber() && !compareNumbers(*it, *it2, op))
            isValid = false;
    }
    
    auto pred = [](ExprNode* node) { return node->isNumber(); };
    if (std::all_of(expr->children().begin(), expr->children().end(), pred))
    {
        ExprNode* node = new BoolNode(isValid, expr->parent());
        freeExpression(expr);
        return node;
    }

    if (!isValid)
    {
        ExprNode* node = new BoolNode(false, expr->parent());
        freeExpression(expr);
        return node;
    }

    return expr;
}

ExprNode* inequalityAnd(ExprNode* expr)
{
    if (expr->children().size() < 2)     
    {
        gErrorManager.log("Arity mismatch: " + toInfixString(expr) + " , expected at least 2 arguments", ErrorManager::ERROR, __FILE__, __LINE__); 
        return expr;
    }

    return expr;
}

ExprNode* inequalityOr(ExprNode* expr)
{
    if (expr->children().size() < 2)     
    {
        gErrorManager.log("Arity mismatch: " + toInfixString(expr) + " , expected at least 2 arguments", ErrorManager::ERROR, __FILE__, __LINE__); 
        return expr;
    }

    return expr;
}

// Evalutator

ExprNode* evaluateInequality(ExprNode* expr, int data)
{
    if (expr->isOperator())
    {
        OpNode* op = (OpNode*)expr;
        if (op->name() == ">")          return inequalityGreater(expr);
        if (op->name() == "<")          return inequalityLess(expr);
        if (op->name() == ">=")         return inequalityGreaterEq(expr);
        if (op->name() == "<=")         return inequalityLessEq(expr);
        if (op->name() == "!=")         return inequalityNotEq(expr);

        if (op->name() == "and")        return inequalityAnd(expr);
        if (op->name() == "or")         return inequalityOr(expr);
    }

    if (isArithmetic(expr))     return evaluateExpr(expr);

    gErrorManager.log("Unimpemented operation: " + toInfixString(expr), ErrorManager::ERROR, __FILE__, __LINE__);
    return expr;
}

// Checking

bool isInequality(ExprNode* expr)
{
    if (expr->isOperator())
    {
        OpNode* op = (OpNode*)expr;
        if (op->name() == "and" || op->name() == "or")
            return std::all_of(op->children().begin(), op->children().end(), isInequality);

        if (op->name() == ">" || op->name() == "<" || op->name() == ">=" || op->name() == "<=" || op->name() == "!=")
            return std::all_of(op->children().begin(), op->children().end(), isArithmetic);
    }

    return false;
}

}