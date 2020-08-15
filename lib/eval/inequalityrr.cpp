#include "inequality.h"
#include "inequalityrr.h"

namespace MathSolver
{

ExprNode* rewriteInequalityCommon(ExprNode* op)
{
    if (op->children().size() < 2)     
    {
        gErrorManager.log("Arity mismatch: " + toInfixString(op) + " , expected 2 arguments",
                          ErrorManager::ERROR, __FILE__, __LINE__); 
        return op;
    }

    if (op->children().size() == 2) 
    {      
        if (op->children().front()->isOperator() && ((OpNode*)op->children().front())->name() == "and")
        {
            ExprNode* ineq = new OpNode(op->toString(), op->children().front());
            ExprNode* lhs = copyOf(op->children().front()->children().back()->children().back());
            ExprNode* rhs = op->children().back();

            op->children().pop_back();
            lhs->setParent(ineq);
            rhs->setParent(ineq);
            ineq->children().push_back(lhs);
            ineq->children().push_back(rhs);
            op->children().front()->children().push_back(ineq);
            return moveNode(op, op->children().front()); // (< (and ...)) ==> (and ...)
        }
        else if (isHalfBoundedInequality(op->children().front()))
        {
            ExprNode* ineq = new OpNode(op->toString(), op);
            ExprNode* lhs = copyOf(op->children().front()->children().back());
            ExprNode* rhs = op->children().back();

            op->children().pop_back();
            lhs->setParent(ineq);
            rhs->setParent(ineq);
            ineq->children().push_back(lhs);
            ineq->children().push_back(rhs);
            op->children().push_back(ineq);
            ((OpNode*)op)->setName("and");
        }

        return op;
    }

    ExprNode* res = new OpNode("and", op->parent());
    auto it = op->children().begin();
    for (auto it2 = std::next(it); it2 != op->children().end(); ++it, ++it2)
    {
        ExprNode* sub = new OpNode(op->toString(), res);
        sub->children().push_back(copyOf(*it));
        sub->children().push_back(copyOf(*it2));
        sub->children().front()->setParent(sub);
        sub->children().back()->setParent(sub);
        res->children().push_back(sub);
    }

    freeExpression(op);
    return res;
}

ExprNode* rewriteInequalityNotEq(ExprNode* op)
{
    if (op->children().size() < 2)     
    {
        gErrorManager.log("Arity mismatch: " + toInfixString(op) + " , expected 2 arguments", ErrorManager::ERROR, __FILE__, __LINE__); 
        return op;
    }

// TODO: 'x < a and x > a' ==> x != a in 'inequality' evaluator
#ifdef MATHSOLVER_ALLOW_UNSTABLE
    if (op->children().size() == 2)
    {
        ExprNode* var;
        ExprNode* bound;
        if (op->children().front()->isNumber())
        {
            var = op->children().back();
            bound = op->children().front();
        }
        else
        {   
            var = op->children().front();
            bound = op->children().back();
        }
        
        ExprNode* res = new OpNode("or", op->parent());
        ExprNode* lhs = new OpNode("<", res);
        ExprNode* rhs = new OpNode(">", res);

        lhs->children().push_back(copyOf(var));
        lhs->children().push_back(copyOf(bound));
        lhs->children().front()->setParent(lhs);
        lhs->children().back()->setParent(lhs);
        res->children().push_back(lhs);
        
        rhs->children().push_back(copyOf(var));
        rhs->children().push_back(copyOf(bound));
        rhs->children().front()->setParent(rhs);
        rhs->children().back()->setParent(rhs);
        res->children().push_back(rhs);

        freeExpression(op);
        return res; 
    }
#endif

    // TODO: possibly need to expand into combinations pairs
    // ex: a != b != c ==> a != b and b != c and a != c

    return op;           
}

ExprNode* rewriteInequality(ExprNode* expr, int data)
{
    if (expr->isOperator())
    {
        OpNode* op = (OpNode*)expr;
        if (op->name() == ">" || op->name() == "<" || op->name() == ">=" || op->name() == "<=")
            return rewriteInequalityCommon(expr);
        if (op->name() == "!=")         
            return rewriteInequalityNotEq(expr);
    }

    return expr;
}

}