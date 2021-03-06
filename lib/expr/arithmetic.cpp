#include "arithmetic.h"

namespace MathSolver
{

bool isArithmeticNode(ExprNode* node)
{
    if (node->type() == ExprNode::FUNCTION)
    {
        FuncNode* func = (FuncNode*)node;   // TODO: move these functions out
        return (func->name() == "exp" || func->name() == "log" ||
                func->name() == "sin" || func->name() == "cos" || func->name() == "tan");
    }
    else if (node->isOperator())
    {
        OpNode* op = (OpNode*)node;
        return (op->name() == "-*" ||
                op->name() == "+" || op->name() == "-" || op->name() == "**" ||
                op->name() == "*" || op->name() == "/" || op->name() == "%" || op->name() == "mod" || 
                op->name() == "^" || op->name() == "!");
    }

    return node->isNumber() || node->type() == ExprNode::VARIABLE;
}

std::list<ExprNode*> commonTerm(ExprNode* expr1, ExprNode* expr2)
{
    if (expr1->isOperator() && expr2->isOperator() && 
        ((OpNode*)expr1)->name() == "-*" && ((OpNode*)expr2)->name() == "-*")
        return commonTerm(expr1->children().front(), expr2->children().front());
    if (expr1->isOperator() && ((OpNode*)expr1)->name() == "-*")
        return commonTerm(expr1->children().front(), expr2);
    if (expr2->isOperator() && ((OpNode*)expr2)->name() == "-*")
        return commonTerm(expr1, expr2->children().front());

    std::list<ExprNode*> common;
    if (expr1->isOperator() && expr2->isOperator() && 
        (((OpNode*)expr1)->name() == "*" || ((OpNode*)expr1)->name() == "**") && 
        (((OpNode*)expr2)->name() == "*" || ((OpNode*)expr2)->name() == "**"))
    {
        for (auto i = expr1->children().begin(); i != expr1->children().end(); ++i)
        {
            for (auto j = expr2->children().begin(); j != expr2->children().end(); ++j)
            {
                if (eqvExpr(*i, *j))
                    common.push_back(copyOf(*i));
            }
        }
    }
    else if (expr1->isOperator() && (((OpNode*)expr1)->name() == "*" || ((OpNode*)expr1)->name() == "**") && 
             (expr2->type() == ExprNode::VARIABLE || expr2->type() == ExprNode::CONSTANT))
    {
        for (auto i = expr1->children().begin(); i != expr1->children().end(); ++i)
        {
            if (eqvExpr(*i, expr2))
                common.push_back(copyOf(*i));
        }
    }
    else if ((expr1->type() == ExprNode::VARIABLE || expr1->type() == ExprNode::CONSTANT) && 
             expr2->isOperator() && (((OpNode*)expr2)->name() == "*" || ((OpNode*)expr2)->name() == "**"))
    {
        for (auto i = expr2->children().begin(); i != expr2->children().end(); ++i)
        {
            if (eqvExpr(expr1, *i))
                common.push_back(copyOf(*i));
        }
    }
    else if ((expr1->type() == ExprNode::VARIABLE || expr1->type() == ExprNode::CONSTANT) && 
             (expr2->type() == ExprNode::VARIABLE || expr2->type() == ExprNode::CONSTANT))
    {
        if (eqvExpr(expr1, expr2))
            common.push_back(copyOf(expr1));
    }

    return common;
}

std::list<ExprNode*> coeffTerm(ExprNode* expr, ExprNode* term)
{
    std::list<ExprNode*> coeff;
    if (expr->isOperator() && ((OpNode*)expr)->name() == "-*")
    {
        coeff = coeffTerm(expr->children().front(), term);
        coeff.push_front(new IntNode(-1));
    }
    else if (expr->isOperator() && term->isOperator() &&
             (((OpNode*)expr)->name() == "*" || ((OpNode*)expr)->name() == "**") && 
             (((OpNode*)term)->name() == "*" || ((OpNode*)term)->name() == "**"))
    {
        for (auto i = expr->children().begin(); i != expr->children().end(); ++i)
        {
            bool containedIn = false;
            for (auto j = term->children().begin(); !containedIn && j != term->children().end(); ++j)
            {
                if (eqvExpr(*i, *j))
                    containedIn = true;
            }

            if (!containedIn)
                coeff.push_back(*i);
        }
    }

    if (coeff.empty())  coeff.push_back(new IntNode(1));
    return coeff;
}

void removeTerm(ExprNode* expr, const std::list<ExprNode*>& terms)
{
    for (auto e : terms)
    {
        auto it = expr->children().begin();
        while (it != expr->children().end())
        {
            if ((*it)->isOperator() && ((OpNode*)*it)->name() == "-*")
            {
                removeTerm(*it, terms);
            }
            else if (eqvExpr(*it, e))
            {
                delete *it;
                it = expr->children().erase(it);
            }
            else
            {
                ++it;
            }          
        }
    }
}

ExprNode* extractPowBase(ExprNode* op)
{
    if (op->isOperator() && ((OpNode*)op)->name() == "^")    return copyOf(op->children().front());
    else                                                     return copyOf(op);
}

ExprNode* extractPowExp(ExprNode* op)
{
    if (op->isOperator() && ((OpNode*)op)->name() == "^")    return copyOf(op->children().back());
    else                                                     return new IntNode(1);
}

ExprNode* peekPowBase(ExprNode* op)
{
    if (op->isOperator() && ((OpNode*)op)->name() == "^")    return op->children().front();
    else                                                     return op;
}

ExprNode* peekPowExp(ExprNode* op)
{
    static IntNode one = IntNode(1); // TODO: definitely bad
    if (op->isOperator() && ((OpNode*)op)->name() == "^")       return op->children().back();
    else                                                        return &one;
}

}