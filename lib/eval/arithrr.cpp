#include "arithrr.h"
#include "../expr/polynomial.h"

namespace MathSolver
{

ExprNode* rewriteNeg(ExprNode* op)
{
    if (op->children().size() != 1)     
    {
        gErrorManager.log("Arity mismatch: " + toInfixString(op) + " , expected 1 argument", ErrorManager::ERROR, __FILE__, __LINE__); 
        return op;
    }

    ExprNode* arg = op->children().front();
    if (isZeroNode(arg))     // (-* 0) ==> 0
    {
        ExprNode* ret = new IntNode(0, op->parent());
        freeExpression(op);
        op = ret;
    }
    else if (arg->isOperator() && ((OpNode*)arg)->name() == "-*") // (-* (-* x)) ==> x
    {
        ExprNode* iarg = arg->children().front();
        arg->children().pop_front();
        iarg->setParent(op->parent());
        freeExpression(op);
        op = iarg;
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

    flattenExpr(op);
    if (isPolynomial(op))  op = reorderPolynomial(op);
    
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

ExprNode* rewriteSub(ExprNode* op)
{
    if (op->children().size() < 2)     
    {
        gErrorManager.log("Arity mismatch: " + toInfixString(op) + " , expected at least 2 arguments", ErrorManager::ERROR, __FILE__, __LINE__); 
        return op;
    }

    ((OpNode*)op)->setName("+");
    for (auto it = std::next(op->children().begin()); it != op->children().end(); ++it)
    {
        ExprNode* neg = new OpNode("-*", op);
        neg->children().push_back(*it);
        neg = rewriteArithmetic(neg);
        (*it)->setParent(neg);
        it = replaceChild(op, neg, it);
    }

    return rewriteAdd(op);
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

    return op;
}

ExprNode* rewriteDiv(ExprNode* op)
{
    if (op->children().size() == 2)     
    {
        gErrorManager.log("Arity mismatch: " + toInfixString(op) + " , expected 2 arguments", ErrorManager::ERROR, __FILE__, __LINE__); 
        return op;
    }

    ExprNode* num = op->children().front();
    ExprNode* den = op->children().back();

    if (isZeroNode(num))    // (/ 0 n) ==> 0
    {
        ExprNode* zero = new IntNode(0, op->parent());
        freeExpression(op);
        return zero;
    }

    if (isZeroNode(den))    // (/ n 0) ==> undef
    {
        ExprNode* ret = new ConstNode("undef", op->parent());
        gErrorManager.log("Division by zero: " + toInfixString(op), ErrorManager::WARNING);      
        freeExpression(op);
        return ret;
    }

    if (isIdentityNode(den))    // (/ n 1) ==> n
    {
        ExprNode* arg = op->children().front();
        arg->setParent(op->parent());
        op->children().pop_front();
        freeExpression(op);
        return arg;
    }

    return op;
}

ExprNode* rewriteExp(ExprNode* op)
{
    if (op->children().size() != 1)     
    {
        gErrorManager.log("Arity mismatch: " + toInfixString(op) + " , expected 1 argument", ErrorManager::ERROR, __FILE__, __LINE__); 
        return op;
    }

    ExprNode* arg = op->children().front();
    if (op->isOperator() && ((OpNode*)op)->name() == "log")  // (exp (log x)) ==> x
    {
        ExprNode* iarg = arg->children().front();
        arg->children().pop_front();
        iarg->setParent(op->parent());
        freeExpression(op);
        return iarg;
    }

    return op;
}

ExprNode* rewriteLog(ExprNode* op)
{
    if (op->children().size() != 1)     
    {
        gErrorManager.log("Arity mismatch: " + toInfixString(op) + " , expected 1 argument", ErrorManager::ERROR, __FILE__, __LINE__); 
        return op;
    }

    ExprNode* arg = op->children().front();
    if (op->isOperator() && ((OpNode*)op)->name() == "exp")   // (log (exp x)) ==> x
    {
        ExprNode* iarg = arg->children().front();
        arg->children().pop_front();
        iarg->setParent(op->parent());
        freeExpression(op);
        return iarg;
    }

    return op;
}

ExprNode* rewriteArithmetic(ExprNode* expr)
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
        if (op->name() == "^")                          return expr; // no rewrite rules
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
