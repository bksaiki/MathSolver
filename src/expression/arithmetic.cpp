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

ExpressionNode* containsLikeTerm(ExpressionNode* expr, ExpressionNode* term)
{
    if (expr->mStr == "+" || expr->mStr == "-")
    {
        for (ExpressionNode* child : expr->mChildren)
        {
            ExpressionNode* like = containsLikeTerm(child, term);
            if (like != nullptr)
                return like;
        }
    }

    return nullptr;
}

//
//  Numerical arithmetic evaluator
//

// Evalutes "(-* <num>)"
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

// Evaluates "(+ <num>...)"
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

// Evaluates "(- <num>...)"
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

// Evaluates "(* <num>...)" or "(** <num>...)"
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

// Evaluates "<num> / <num>"
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
    return true;
}

//
//  Symbolic arithmetic evaluators
//

// Evaluates "(exp x)"
bool symbolicExp(ExpressionNode* op)
{
    if (op->mChildren.size() != 1)      return false;  // TODO: arity mismatch

    if (op->mChildren.front()->mStr == "log" && op->mChildren.front()->mChildren.size() == 1) // exp(log (x)) --> x
    {
        ExpressionNode* ln = op->mChildren.front();
        assignExprNode(op, ln->mChildren.front());
        delete ln->mChildren.front();
        delete ln;
    }

    // TODO: more simplifications

    return true;
}

// Evaluates "(log x) or (log b x)"
bool symbolicLog(ExpressionNode* op)
{
    if (op->mChildren.size() != 1 && op->mChildren.size() != 2)      return false;  // TODO: arity mismatch

    if (op->mChildren.front()->mStr == "exp" && op->mChildren.front()->mChildren.size() == 1) // (log (exp x)) --> x
    {
        ExpressionNode* ln = op->mChildren.front();
        assignExprNode(op, ln->mChildren.front());
        delete ln->mChildren.front();
        delete ln;
    }

    // TODO: more simplifications

    return true;
}

// Evaluates "(sin x)"
bool symbolicSin(ExpressionNode* op)
{
    if (op->mChildren.size() != 1)      return false;  // TODO: arity mismatch

    // TODO: more simplifications

    return true;
}

// Evaluates "(cos x)"
bool symbolicCos(ExpressionNode* op)
{
    if (op->mChildren.size() != 1)      return false;  // TODO: arity mismatch

    // TODO: more simplifications

    return true;
}

// Evaluates "(tan x)"
bool symbolicTan(ExpressionNode* op)
{
    if (op->mChildren.size() != 1)      return false;  // TODO: arity mismatch

    // TODO: more simplifications

    return true;
}


bool evaluateArithmetic(ExpressionNode* expr)
{
    if (isNumerical(expr))
    {
        if (expr->mStr == "-*")                             return numericNeg(expr);
        else if (expr->mStr == "+")                         return numericAdd(expr);
        else if (expr->mStr == "-")                         return numericSub(expr);
        else if (expr->mStr == "*" || expr->mStr == "**")   return numericMul(expr);
        else if (expr->mStr == "/")                         return true; // no evaluation

        else if (expr->mStr == "exp")                       return false;   // Unsupported: numerical exp
        else if (expr->mStr == "log")                       return false;   // Unsupported: numerical log
        else if (expr->mStr == "sin")                       return false;   // Unsupported: numerical sin
        else if (expr->mStr == "cos")                       return false;   // Unsupported: numerical cos
        else if (expr->mStr == "tan")                       return false;   // Unsupported: numerical tan
    }
    else // symbolic
    {
        if (expr->mStr == "-*")                             return false;   // Unsupported: symbolic negation
        else if (expr->mStr == "+")                         return false;   // Unsupported: symbolic +
        else if (expr->mStr == "-")                         return false;   // Unsupported: symbolic -
        else if (expr->mStr == "*" || expr->mStr == "**")   return false;   // Unsupported: symbolic *, **
        else if (expr->mStr == "/")                         return false;   // Unsupported: symbolic /   

        else if (expr->mStr == "exp")                       return symbolicExp(expr);
        else if (expr->mStr == "log")                       return symbolicLog(expr);
        else if (expr->mStr == "sin")                       return symbolicSin(expr);
        else if (expr->mStr == "cos")                       return symbolicCos(expr);
        else if (expr->mStr == "tan")                       return symbolicTan(expr);                                         
    }

    return false; // TODO: unsupported op
}

} // end