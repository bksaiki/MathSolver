#include "arithmetic.h"

namespace MathSolver
{

bool isArithmetic(ExpressionNode* expr)
{
    for (ExpressionNode* child : expr->mChildren)
    {
        if (!isArithmetic(child))
            return false;
    }

    if (expr->mType == ExpressionNode::FUNCTION)
    {
        return (expr->mStr == "exp" || expr->mStr == "log" ||
                expr->mStr == "sin" || expr->mStr == "cos" || expr->mStr == "tan");
    }
    else if (expr->mType == ExpressionNode::OPERATOR)
    {
        return (expr->mStr == "+" || expr->mStr == "-" || expr->mStr == "**" ||
                expr->mStr == "*" || expr->mStr == "/" || expr->mStr == "%" ||
                expr->mStr == "^" || expr->mStr == "!");
    }
    else
    {
        return true;
    }
}

//
//  Numerical arithmetic evaluator
//

// Evalutes (-* <num>)
bool numericNeg(ExpressionNode* op)
{
    if (op->mChildren.size() != 1)     return false; // TODO: arity mismatch

    if (op->mChildren.front()->mType == ExpressionNode::INTEGER)
    {
        op->mType = ExpressionNode::INTEGER;
        op->mExact = -(op->mChildren.front()->mExact);
    }

    delete op->mChildren.front();
    op->mChildren.clear();
    op->mPrecedence = 0;
    return true;
}

// Evaluates (+ <num>...)
bool numericAdd(ExpressionNode* op)
{
    if (op->mChildren.front()->mType == ExpressionNode::INTEGER)
    {
        op->mType = ExpressionNode::INTEGER;
        op->mExact = op->mChildren.front()->mExact;
    }  
    else // <inexact>
    { 
        op->mType = ExpressionNode::FLOAT;
        op->mInexact = op->mChildren.front()->mInexact;
    }
    
    for (std::list<ExpressionNode*>::iterator it = std::next(op->mChildren.begin()); it != op->mChildren.end(); ++it)
    {
        if (op->mType == ExpressionNode::INTEGER && (*it)->mType == ExpressionNode::INTEGER) // <exact> += <exact>
            op->mExact += (*it)->mExact;
        // TODO: inexact
        
        delete *it;
    }

    op->mChildren.clear();  
    op->mPrecedence = 0; 
    return true;
}

// Evaluates (- <num>...)
bool numericSub(ExpressionNode* op)
{
    if (op->mChildren.front()->mType == ExpressionNode::INTEGER)
    {
        op->mType = ExpressionNode::INTEGER;
        op->mExact = op->mChildren.front()->mExact;
    }  
    else // <inexact>
    { 
        op->mType = ExpressionNode::FLOAT;
        op->mInexact = op->mChildren.front()->mInexact;
    }
    
    for (std::list<ExpressionNode*>::iterator it = std::next(op->mChildren.begin()); it != op->mChildren.end(); ++it)
    {
        if (op->mType == ExpressionNode::INTEGER && (*it)->mType == ExpressionNode::INTEGER) // <exact> += <exact>
            op->mExact -= (*it)->mExact;
        // TODO: inexact
        
        delete *it;
    }

    op->mChildren.clear(); 
    op->mPrecedence = 0;  
    return true;
}

// Evaluates (* <num>...) or (** <num>...)
bool numericMul(ExpressionNode* op)
{
    if (op->mChildren.front()->mType == ExpressionNode::INTEGER)
    {
        op->mType = ExpressionNode::INTEGER;
        op->mExact = op->mChildren.front()->mExact;
    }  
    else // <inexact>
    { 
        op->mType = ExpressionNode::FLOAT;
        op->mInexact = op->mChildren.front()->mInexact;
    }
    
    for (std::list<ExpressionNode*>::iterator it = std::next(op->mChildren.begin()); it != op->mChildren.end(); ++it)
    {
        if (op->mType == ExpressionNode::INTEGER && (*it)->mType == ExpressionNode::INTEGER) // <exact> *= <exact>
            op->mExact *= (*it)->mExact;
        // TODO: inexact
        
        delete *it;
    }

    op->mChildren.clear();
    op->mPrecedence = 0;   
    return true;
}

// Evaluates <num> / <num>
bool numericDiv(ExpressionNode* op)
{
    if (op->mChildren.size() != 2)     return false; // TODO: arity mismatch

    ExpressionNode* lhs = op->mChildren.front();
    ExpressionNode* rhs = op->mChildren.back();

    if (lhs->mType == ExpressionNode::INTEGER && rhs->mType == ExpressionNode::INTEGER) // <exact> / <exact>
    {   
        return true; // no exact numerical division: a/b --> a/b
    }

    op->mChildren.clear();
    op->mPrecedence = 0;
    delete lhs;
    delete rhs;
    return false;
}

bool evaluateArithmetic(ExpressionNode* expr)
{
    if (expr->mType == ExpressionNode::FUNCTION)
    {

    }
    else if (expr->mType == ExpressionNode::OPERATOR)
    {
        if (expr->mStr == "+")
        {
            if (isNumerical(expr))  return numericAdd(expr);
            // TODO: symbolic add
        }
        else if (expr->mStr == "-")
        {
            if (isNumerical(expr))  return numericSub(expr);
            // TODO: symbolic sub
        }
        else if (expr->mStr == "-*")
        {
            if (isNumerical(expr))  return numericNeg(expr);
            // TODO: symbolic sub
        }
        else if (expr->mStr == "*" || expr->mStr == "**")
        {
            if (isNumerical(expr))  return numericMul(expr);
            // TODO: symbolic mul
        }
        else if (expr->mStr == "/")
        {
            if (isNumerical(expr))  return numericDiv(expr);
            // TODO: symbolic mul
        }
    }

    return false; // TODO: unsupported op
}

} // end