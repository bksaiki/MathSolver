#include "arithmetic.h"

namespace MathSolver
{

bool isArithmetic(ExprNode* expr)
{
    for (ExprNode* child : expr->children())
    {
        if (!isArithmetic(child))
            return false;
    }

    if (expr->type() == ExprNode::FUNCTION)
    {
        FuncNode* func = (FuncNode*)expr;
        return (func->name() == "exp" || func->name() == "log" ||
                func->name() == "sin" || func->name() == "cos" || func->name() == "tan");
    }
    else if (expr->isOperator())
    {
        OpNode* op = (OpNode*)expr;
        return (op->name() == "-*" ||
                op->name() == "+" || op->name() == "-" || op->name() == "**" ||
                op->name() == "*" || op->name() == "/" || op->name() == "%" || op->name() == "mod" || 
                op->name() == "^" || op->name() == "!");
    }
    else
    {
        return true;
    }
}

std::list<ExprNode*> commonTerm(ExprNode* expr1, ExprNode* expr2)
{
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
    if (expr->isOperator() && term->isOperator() &&
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

    return coeff;
}

ExprNode* getPowBase(ExprNode* op)
{
    if (op->isOperator() && ((OpNode*)op)->name() == "^")       return op->children().front();
    else                                                        return op;
}

ExprNode* getPowExp(ExprNode* op)
{
    if (op->isOperator() && ((OpNode*)op)->name() == "^")       return op->children().back();
    else                                                        return new IntNode(1);
}

}