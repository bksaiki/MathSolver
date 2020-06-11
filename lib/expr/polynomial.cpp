#include <algorithm>
#include "polynomial.h"

namespace MathSolver
{

bool isMonomialBasis(ExprNode* expr)
{
    return (expr->type() == ExprNode::VARIABLE) ||     // x or x^n
           (expr->isOperator() && ((OpNode*)expr)->name() == "^" && containsType(expr->children().front(), ExprNode::VARIABLE) &&
            expr->children().back()->type() == ExprNode::INTEGER && !((IntNode*)expr->children().back())->value().sign());
        
}

bool isMonomialEqv(ExprNode* expr)
{
    if (!expr->isOperator() || !(((OpNode*)expr)->name() == "*" || ((OpNode*)expr)->name() == "**" || ((OpNode*)expr)->name() == "/")) // single value
        return expr->isValue() || isMonomialBasis(expr);

    if (expr->isOperator() && (((OpNode*)expr)->name() == "/" || ((OpNode*)expr)->name() == "-*")) // for '/' or '-*', check children
        return std::all_of(expr->children().begin(), expr->children().end(), isMonomialEqv);

    for (auto e : expr->children())
    {
        if (!(e->isValue() || isMonomialBasis(e)))
            return false;
    }

    return true;
}

bool isPolynomialEqv(ExprNode* expr)
{
    if (!expr->isOperator() || (((OpNode*)expr)->name() != "+" && ((OpNode*)expr)->name() != "-")) // single term
        return isMonomialEqv(expr);

    for (auto e : expr->children())
    {
        if (((((OpNode*)e)->name() == "+" || ((OpNode*)e)->name() == "-") && !isPolynomial(e)) || // unflattened polynomial
            !isMonomialEqv(e))     // monomial term
            return false;
    }

    return true;
}

bool isMonomial(ExprNode* expr)
{
    if (expr->isOperator() && (((OpNode*)expr)->name() == "/" || ((OpNode*)expr)->name() == "-*")) // for '/' or '-*', check children
        return std::all_of(expr->children().begin(), expr->children().end(), isMonomial);

    if (expr->isOperator() || (((OpNode*)expr)->name() == "*" || ((OpNode*)expr)->name() == "**"))
    {
        if (!(expr->children().front()->isValue() || isMonomialBasis(expr->children().front())))  // the first operand may optionally be a number
            return false;    

        for (auto e = std::next(expr->children().begin()); e != expr->children().end(); ++e)    // all others must be monomial basis
        {
            if (!isMonomialBasis(*e))
                return false;
        }

        return true;
    }

    return expr->isValue() || isMonomialBasis(expr);
}

bool isPolynomial(ExprNode* expr)
{
    if (!expr->isOperator() || (((OpNode*)expr)->name() != "+" && ((OpNode*)expr)->name() != "-")) // single term
        return isMonomial(expr);

    for (auto e : expr->children())
    {
        if (!isMonomial(e) && !isPolynomial(e)) // unflattened polynomial
            return false;
    }

    return true;
}

int monomialBasisOrder(ExprNode* expr) // TODO: replace stoi with Integer->int conversion
{
    // expr->isOperator() <===> (^ x n)
    if (expr->isOperator()) return std::stoi(((IntNode*)expr->children().back())->value().toString());
    if (expr->type() == ExprNode::VARIABLE) return 1;
    return 0;
}

int monomialOrder(ExprNode* expr)
{
#ifdef MATHSOLVER_DEBUG
    if (!isMonomial(expr))
    {
        gErrorManager.log("Expected a monomial: " + toInfixString(expr), ErrorManager::ERROR, __FILE__, __LINE__);
        return 0;
    }
#endif

    if (!expr->isOperator() || !(((OpNode*)expr)->name() == "*" || ((OpNode*)expr)->name() == "**" || ((OpNode*)expr)->name() == "/"))
        return monomialBasisOrder(expr);

    if (expr->isOperator() && ((OpNode*)expr)->name() == "/")
        return monomialOrder(expr->children().front()) - monomialOrder(expr->children().back());

    if (expr->isOperator() && ((OpNode*)expr)->name() == "-*")
        return monomialOrder(expr->children().front());

    int order = 0;
    for (auto e : expr->children())
        order += monomialBasisOrder(e);
    return order;
}

//
// Comparators
//

int monomialBasisCmp(ExprNode* lhs, ExprNode* rhs)
{
    if (lhs->children().empty() && rhs->children().empty())
    {
        if (lhs->type() == ExprNode::VARIABLE && rhs->type() != ExprNode::VARIABLE) return -1;
        if (lhs->type() != ExprNode::VARIABLE && rhs->type() == ExprNode::VARIABLE) return 1;

        if (lhs->isNumber() && !rhs->isNumber())                                    return 1;
        if (!lhs->isNumber() && rhs->isNumber())                                    return -1;
        if (lhs->type() == ExprNode::CONSTANT && rhs->type() != ExprNode::CONSTANT) return 1;
        if (lhs->type() != ExprNode::CONSTANT && rhs->type() == ExprNode::CONSTANT) return -1;
    } // else: x^n and y^m
    else if (lhs->isOperator() && rhs->isOperator() && ((OpNode*)lhs)->name() == "^" && ((OpNode*)rhs)->name() == "^")
    {
        int lo = monomialOrder(lhs);
        int ro = monomialOrder(rhs);
        if (lo > ro)   return 1;
        if (lo < ro)   return -1;  
    }

    return 0;
}

bool monomialBasisGt(ExprNode* lhs, ExprNode* rhs) { return (monomialBasisCmp(lhs, rhs) > 0); }
// bool monomialBasisLt(ExprNode* lhs, ExprNode* rhs) { return (monomialBasisCmp(lhs, rhs) > 0); } 

int monomialOrderCmp(ExprNode* lhs, ExprNode* rhs)
{
    if (lhs->children().empty() && rhs->children().empty())
    {
        if (lhs->type() == ExprNode::VARIABLE && rhs->type() != ExprNode::VARIABLE) return 1;
        if (lhs->type() != ExprNode::VARIABLE && rhs->type() == ExprNode::VARIABLE) return -1;
        return 0;
    }
   
    if (lhs->children().empty()) return -1;
    if (rhs->children().empty()) return 1;

    int lo = monomialOrder(lhs);
    int ro = monomialOrder(rhs);
    if (lo > ro)   return 1;
    if (lo < ro)   return -1;  

    if (((OpNode*)lhs)->name() == "^" && ((OpNode*)rhs)->name() == "^" &&
        eqvExpr(lhs->children().front(), rhs->children().front()))
    {
        lo = monomialBasisOrder(lhs);
        ro = monomialBasisOrder(rhs);
    }
    else if (((OpNode*)lhs)->name() == "^")
    {
        for (auto i : rhs->children())
        {
            if (!i->isValue() && eqvExpr(lhs->children().front(), i->children().front()))
            {
                lo = monomialBasisOrder(lhs);
                ro = monomialBasisOrder(i);
                break;
            }
        }
    }
    else if (((OpNode*)rhs)->name() == "^")
    {
        for (auto i : lhs->children())
        {
            if (!i->isValue() && eqvExpr(i->children().front(), rhs->children().front()))
            {
                lo = monomialBasisOrder(i);
                ro = monomialBasisOrder(rhs);
                break;
            }
        }
    }
    else
    {
        lo = ro = 0;
        for (auto i : lhs->children())  // compare order of like terms
        {
            for (auto j : rhs->children())
            {
                if (isMonomialBasis(i) && isMonomialBasis(j) && !i->isValue() && !j->isValue() &&
                    eqvExpr(i->children().front(), j->children().front()))
                {
                    lo += monomialBasisOrder(i);
                    ro += monomialBasisOrder(j);
                }
            }
        }
    }

    if (lo > ro)   return 1;
    if (lo < ro)   return -1; 
    return 0;
}

bool monomialOrderGt(ExprNode* lhs, ExprNode* rhs) { return (monomialOrderCmp(lhs, rhs) > 0); }
bool monomialOrderLt(ExprNode* lhs, ExprNode* rhs) { return (monomialOrderCmp(lhs, rhs) > 0); }

//
//  End comparators
//

ExprNode* reorderMonomial(ExprNode* expr)
{
#ifdef MATHSOLVER_DEBUG
    if (!isMonomial(expr))
    {
        gErrorManager.log("Expected a monomial: " + toInfixString(expr), ErrorManager::ERROR, __FILE__, __LINE__);
        return expr;
    }
#endif

    expr->children().sort(monomialBasisGt);
    return expr;
}

ExprNode* reorderPolynomial(ExprNode* expr, bool down)
{
#ifdef MATHSOLVER_DEBUG
    if (!isPolynomial(expr))
    {
        gErrorManager.log("Expected a polynomial: " + toInfixString(expr), ErrorManager::ERROR, __FILE__, __LINE__);
        return expr;
    }
#endif

    for (auto e : expr->children())
        e->children().sort(monomialBasisGt);

    if (!expr->isOperator() || (((OpNode*)expr)->name() != "+" && ((OpNode*)expr)->name() != "-")) // single term
        return expr;
    
    expr->children().sort((down ? monomialOrderGt : monomialOrderLt));
    return expr;
}

}