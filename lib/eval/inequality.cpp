#include <algorithm>
#include "boolean.h"
#include "evaluator.h"
#include "inequality.h"
#include "../types/range.h"
#include "../expr/arithmetic.h"

namespace MathSolver
{

Range extractRange(ExprNode* expr)
{
    if (expr->children().size() == 2)
    {   
        OpNode* op = (OpNode*)expr;
        ExprNode* lhs = expr->children().front();
        ExprNode* rhs = expr->children().back();
        if (lhs->type() == ExprNode::VARIABLE && rhs->isNumber())
        {
            Float bound = toFloat(rhs);
            if (op->name() == ">")      return Range(bound, "inf", false, false);
            if (op->name() == "<")      return Range("-inf", bound, false, false);
            if (op->name() == ">=")     return Range(bound, "inf", true, false);
            if (op->name() == "<=")     return Range("-inf", bound, false, true);
            if (op->name() == "!=")     return Range({{ "-inf", bound, false, false }, { bound, "inf", false, false }});
        }
        else if (lhs->isNumber() && rhs->type() == ExprNode::VARIABLE)
        {
            Float bound = toFloat(lhs);
            if (op->name() == ">")      return Range("-inf", bound, false, false);
            if (op->name() == "<")      return Range(bound, "inf", false, false);
            if (op->name() == ">=")     return Range("-inf", bound, false, true);
            if (op->name() == "<=")     return Range(bound, "inf", true, false);
            if (op->name() == "!=")     return Range({{ "-inf", bound, false, false }, { bound, "inf", false, false }});
        }
    }   
    else if (expr->children().size() == 3)
    {
        OpNode* op = (OpNode*)expr;
        ExprNode* lhs = expr->children().front();
        ExprNode* mid = *(std::next(expr->children().begin()));
        ExprNode* rhs = expr->children().back();
        if (lhs->isNumber() && mid->type() == ExprNode::VARIABLE && rhs->isNumber())
        {
            Float lbound = toFloat(lhs);
            Float ubound = toFloat(rhs);
            if (op->name() == ">")      return Range(ubound, lbound, false, false);
            if (op->name() == "<")      return Range(lbound, ubound, false, false);
            if (op->name() == ">=")     return Range(ubound, lbound, true, true);
            if (op->name() == "<=")     return Range(lbound, ubound, true, true);
        }
    }
    
    return Range();
}

ExprNode* toExpression(const interval_t& ival, const std::string& var)
{
    VarNode* vnode = new VarNode(var);
    if (ival.lower == NEG_INFINITY && ival.upper == POS_INFINITY)
    {
        ExprNode* op = new OpNode("<");
        ExprNode* lbound = new FloatNode("-inf", op);
        ExprNode* ubound = new FloatNode("inf", op);
        op->children().push_back(lbound);
        op->children().push_back(vnode);
        op->children().push_back(ubound);
        vnode->setParent(op);
        return op;
    }
    else if (ival.lower == NEG_INFINITY)
    {
        ExprNode* op = new OpNode(((ival.upperClosed) ? "<=" : "<"));
        ExprNode* bound = new FloatNode(ival.upper, op);
        op->children().push_back(vnode);
        op->children().push_back(bound);
        vnode->setParent(op);
        return op;
    }
    else if (ival.upper == POS_INFINITY)
    {
        ExprNode* op = new OpNode(((ival.lowerClosed) ? ">=" : ">"));
        ExprNode* bound = new FloatNode(ival.lower, op);
        op->children().push_back(vnode);
        op->children().push_back(bound);
        vnode->setParent(op);
        return op;
    }
    else
    {
        if (ival.lowerClosed == ival.upperClosed)
        {
            ExprNode* op = new OpNode(((ival.lowerClosed) ? "<=" : "<"));
            ExprNode* lbound = new FloatNode(ival.lower, op);
            ExprNode* ubound = new FloatNode(ival.upper, op);
            op->children().push_back(lbound);
            op->children().push_back(vnode);
            op->children().push_back(ubound);
            vnode->setParent(op);
            return op;
        }
        else
        {
            ExprNode* op = new OpNode("and");

            ExprNode* lower = new OpNode(((ival.lowerClosed) ? ">=" : ">"), op);
            ExprNode* lbound = new FloatNode(ival.lower, op);
            lower->children().push_back(vnode);
            lower->children().push_back(lbound);
            vnode->setParent(lower);

            ExprNode* upper = new OpNode(((ival.upperClosed) ? "<=" : "<"), op);
            ExprNode* ubound = new FloatNode(ival.upper, op);
            upper->children().push_back(vnode);
            upper->children().push_back(ubound);
            vnode->setParent(upper);

            op->children().push_back(lower);
            op->children().push_back(upper);
            return op;
        }    
    }
}

ExprNode* toExpression(const Range& range, const std::string& var)
{
    // Workaround for GCC bug 
    //  Known to exist on 9.3.0 on Ubuntu 20.04 LTS.
    //  std::distance(list.begin(), list.end()) != list.size()
    //
    //  Note that this is not the case in 'inequalityAnd'. Somehow, 'range' is being changed when
    //  it gets passed by reference.
    size_t intervalCount = std::distance(range.data().begin(), range.data().end()); 

    if (intervalCount == 0)
        return new BoolNode(false);

    if (intervalCount == 1)
        return toExpression(range.data().front(), var);
    
    ExprNode* node = new OpNode("and");
    for (auto e : range.data())
        node->children().push_back(toExpression(e, var));
    return node; 
}

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
        Float f1 = toFloat(lhs);
        Float f2 = toFloat(rhs);
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

ExprNode* inequalityConnective(ExprNode* expr, const std::string& op)
{
    auto it = expr->children().begin();
    while (it != expr->children().end())
    {
        auto it2 = std::next(it);
        bool changed = false;
        while (it2 != expr->children().end())
        {
            std::list<std::string> lvars = extractVariables(*it);
            std::list<std::string> rvars = extractVariables(*it2);
            if (lvars.size() == 1 && rvars.size() == 1 && // (and (x ? a) (x ? b) ...) where a,b are numbers
                lvars.front() == rvars.front())
            {
                Range lhs = extractRange(*it);
                Range rhs = extractRange(*it2);
                ExprNode* res;
                
                if (op == "and")        res = toExpression(lhs.conjoin(rhs), lvars.front());
                else if (op == "or")    res = toExpression(lhs.disjoin(rhs), lvars.front());
                res->setParent(expr);

                freeExpression(*it2);
                freeExpression(*it);
                it = expr->children().erase(it);
                it = expr->children().insert(it, res);
                it2 = expr->children().erase(it2);   
            }
            else
            {
                ++it2;
            }
        }

        if (!changed) ++it;
    }

    if (expr->children().size() == 1) // ex: (and (x > a)) ==> x
        return moveNode(expr, expr->children().front());
    return expr;
}

ExprNode* inequalityAnd(ExprNode* op)
{
    if (op->children().size() < 2)     
    {
        gErrorManager.log("Arity mismatch: " + toInfixString(op) + " , expected at least 2 arguments", ErrorManager::ERROR, __FILE__, __LINE__); 
        return op;
    }

    if (isBooleanExpr(op))    return evaluateExpr(op);

    auto it = op->children().begin();
    while (it != op->children().end())
    {
        if ((*it)->type() == ExprNode::BOOLEAN)
        {
            if (((BoolNode*)*it)->value())
            {
                it = op->children().erase(it);
            }
            else
            {
                ExprNode* res = new BoolNode(false, op->parent());
                freeExpression(op);
                return res;
            }   
        }
        else
        {
            ++it;
        }
    }

    return inequalityConnective(op, "and");
}

ExprNode* inequalityOr(ExprNode* op)
{
    if (op->children().size() < 2)     
    {
        gErrorManager.log("Arity mismatch: " + toInfixString(op) + " , expected at least 2 arguments", ErrorManager::ERROR, __FILE__, __LINE__); 
        return op;
    }

    if (isBooleanExpr(op))    return evaluateExpr(op);

    auto it = op->children().begin();
    while (it != op->children().end())
    {
        if ((*it)->type() == ExprNode::BOOLEAN)     it = op->children().erase(it); 
        else ++it;
    }
 
    return inequalityConnective(op, "or");
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