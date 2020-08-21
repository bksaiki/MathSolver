#include "arithrr.h"
#include "../expr/polynomial.h"
#include "../expr/matcher.h"

namespace MathSolver
{

ExprNode* rewriteNeg(ExprNode* op)
{
    if (op->children().size() != 1)     
    {
        gErrorManager.log("Arity mismatch: " + toInfixString(op) + " , expected 1 argument", ErrorManager::ERROR, __FILE__, __LINE__); 
        return op;
    }

    UniqueTransformMatcher utm;
    utm.add("(-* 0)", "0");
    utm.add("(-* (-* ?a))", "?a");

    return utm.transform(op);
}

ExprNode* rewriteAddSub(ExprNode* op)
{
    flattenExpr(op);
    auto it = op->children().begin();
    if ((*it)->isOperator() && ((OpNode*)*it)->name() == "-*")        // (+ (-* a) b c ...) ==> (+ (- b a) c ...)
    {
        auto it2 = std::next(it); 
        if ((*it2)->isOperator() && ((OpNode*)*it2)->name() == "-*")    // (+ (-* a) (-* b) c ...) ==> (+ (-* (+ a b)) c ...)
        {
            ExprNode* add = new OpNode("+", *it);
            add->children().push_back((*it)->children().front());
            add->children().push_back((*it2)->children().front());
            add = rewriteAdd(add);
            (*it)->children().clear();
            (*it2)->children().clear();

            (*it)->children().push_back(add);
            freeExpression(*it2);
            op->children().erase(it2);
        }
        else              
        {
            ((OpNode*)*it)->setName("-");                                              
            (*it)->children().push_front(*it2);
            op->children().erase(it2);
        }
    }

    while (it != op->children().end())
    {
        auto it2 = std::next(it);
        bool changed = false;

        if (it2 == op->children().end())
            break;

        if (isZeroNode(*it)) // (+ a 0 b ...) ==> (+ a b ...)
        {
            freeExpression(*it);
            it = op->children().erase(it);
            changed = true;
            continue;
        }
        else if ((*it2)->isOperator() && ((OpNode*)*it2)->name() == "-*") // (+ a (-* b) c ...) ==> (+ (- a b) c ...)
        {
            ((OpNode*)*it2)->setName("-");
            (*it2)->children().push_front(*it);
            it = op->children().erase(it);
            continue;
        }

        for (; it2 != op->children().end(); ++it2)
        {
            if ((*it2)->isOperator() && ((OpNode*)*it2)->name() == "-*" &&  // (+ a b (-* a) c ...) ==> (+ b c ...)
                eqvExpr(*it, (*it2)->children().front()))
            {
                freeExpression(*it);
                freeExpression(*it2);
                op->children().erase(it2);
                it = op->children().erase(it);
                changed = true;
                break;
            }   
        }

        if (!changed)   ++it;
    }

    flattenExpr(op);
    if (op->children().size() < 2) // Post: (+ x) ==> x or (+) ==> 0
    {
        ExprNode* ret;
        if (op->children().size() == 1)     
        {
            ret = op->children().front();
            op->children().clear();
        }
        else                                
        {
            ret = new IntNode();
        }

        ret->setParent(op->parent());
        freeExpression(op);
        return ret;
    } 

    return op;
}

ExprNode* rewriteAdd(ExprNode* op)
{   
    if (op->children().size() < 2)     
    {
        gErrorManager.log("Arity mismatch: " + toInfixString(op) + " , expected at least 2 arguments", ErrorManager::ERROR, __FILE__, __LINE__); 
        return op;
    }

    if (isPolynomial(op))  op = reorderPolynomial(op);
    return rewriteAddSub(op);
}

ExprNode* rewriteSub(ExprNode* op)
{
    if (op->children().size() < 2)     
    {
        gErrorManager.log("Arity mismatch: " + toInfixString(op) + " , expected at least 2 arguments", ErrorManager::ERROR, __FILE__, __LINE__); 
        return op;
    }

    if (isPolynomial(op)) op = reorderPolynomial(op);
    
    ((OpNode*)op)->setName("+");
    for (auto it = std::next(op->children().begin()); it != op->children().end(); ++it)
    {
        ExprNode* neg = new OpNode("-*", op);
        (*it)->setParent(neg);
        neg->children().push_back(*it);
        neg = rewriteArithmetic(neg);
        it = replaceChild(op, neg, it);
    }

    return rewriteAddSub(op);
}

ExprNode* rewriteMul(ExprNode* op)
{
    if (op->children().size() < 2)     
    {
        gErrorManager.log("Arity mismatch: " + toInfixString(op) + " , expected at least 2 arguments", ErrorManager::ERROR, __FILE__, __LINE__); 
        return op;
    }

    auto it = op->children().begin();
    while (it != op->children().end())
    {
        ExprNode* arg = *it;
        bool changed = false;

        if (isZeroNode(arg))    // (* a 0 b ...) ==> 0
        {
            ExprNode* zero = new IntNode(0, op->parent());
            freeExpression(op);
            return zero;
        }

        if (isIdentityNode(arg))     // (* a 1 b ...) ==> (* a b ...)    
        {
            freeExpression(*it);
            it = op->children().erase(it);
            changed = true;
        }

        if (!changed)  ++it;
    }

    if (op->children().size() < 2) // Post: (* x) ==> x or (*) ==> 1
    {
        ExprNode* ret;
        if (op->children().size() == 1)     
        {
            ret = op->children().front();
            op->children().clear();
        }
        else                                
        {
            ret = new IntNode(1);
        }

        ret->setParent(op->parent());
        freeExpression(op);
        return ret;
    } 

    return op;
}

ExprNode* rewriteDiv(ExprNode* op)
{
    if (op->children().size() != 2)     
    {
        gErrorManager.log("Arity mismatch: " + toInfixString(op) + " , expected 2 arguments", ErrorManager::ERROR, __FILE__, __LINE__); 
        return op;
    }

    UniqueTransformMatcher utm;
    utm.add("(/ 0 ?a)", "0");
    utm.add("(/ ?a 0", "undef");
    utm.add("(/ ?a 1)", "?a");

    return utm.transform(op);
}

ExprNode* rewritePow(ExprNode* op)
{
    if (op->children().size() != 2)     
    {
        gErrorManager.log("Arity mismatch: " + toInfixString(op) + " , expected 2 arguments", ErrorManager::ERROR, __FILE__, __LINE__); 
        return op;
    }

    UniqueTransformMatcher utm;
    utm.add("(^ ?a 0)", "1");
    utm.add("(^ ?a 1)", "1");

    return utm.transform(op);
}

ExprNode* rewriteExp(ExprNode* op)
{
    if (op->children().size() != 1)     
    {
        gErrorManager.log("Arity mismatch: " + toInfixString(op) + " , expected 1 argument", ErrorManager::ERROR, __FILE__, __LINE__); 
        return op;
    }

    UniqueTransformMatcher utm;
    utm.add("(exp (log ?a))", "?a");

    return utm.transform(op);
}

ExprNode* rewriteLog(ExprNode* op)
{
    if (op->children().size() != 1)     
    {
        gErrorManager.log("Arity mismatch: " + toInfixString(op) + " , expected 1 argument", ErrorManager::ERROR, __FILE__, __LINE__); 
        return op;
    }

    UniqueTransformMatcher utm;
    utm.add("(log (exp ?a))", "?a");

    return utm.transform(op);
}

ExprNode* rewriteArithmetic(ExprNode* expr, int data)
{
    if (expr->isNumber())                       return expr;
    if (expr->type() == ExprNode::CONSTANT)     return expr;
    if (expr->type() == ExprNode::VARIABLE)     return expr;

    if (expr->isOperator())
    {
        OpNode* op = (OpNode*)expr;
        if (op->name() == "-*")                         return rewriteNeg(op);
        if (op->name() == "+")                          return rewriteAdd(op);
        if (op->name() == "-")                          return rewriteSub(op);
        if (op->name() == "*" || op->name() == "**")    return rewriteMul(op);
        if (op->name() == "/")                          return rewriteDiv(op);
        if (op->name() == "%" || op->name() == "mod")   return expr; // no rewrite rules
        if (op->name() == "^")                          return rewritePow(op);
        if (op->name() == "!")                          return expr; // no rewrite rules
    }
    else if (expr->type() == ExprNode::FUNCTION)
    {
        FuncNode* func = (FuncNode*)expr;   
        if (func->name() == "exp")              return rewriteExp(expr);
        if (func->name() == "log")              return rewriteLog(expr);
        if (func->name() == "sin")              return expr;    // no rewrite rules
        if (func->name() == "cos")              return expr;    // no rewrite rules
        if (func->name() == "tan")              return expr;    // no rewrite rules
    }

    gErrorManager.log("Unimpemented rewrite rule: " + toInfixString(expr), ErrorManager::ERROR, __FILE__, __LINE__);
    return expr;
}
    
} // namespace MathSolver
