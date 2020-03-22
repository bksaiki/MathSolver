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
        return (expr->mStr == "-*" ||
                expr->mStr == "+" || expr->mStr == "-" || expr->mStr == "**" ||
                expr->mStr == "*" || expr->mStr == "/" || expr->mStr == "%" ||
                expr->mStr == "^" || expr->mStr == "!");
    }
    else
    {
        return true;
    }
}

std::list<ExpressionNode*> commonTerm(ExpressionNode* expr1, ExpressionNode* expr2)
{
    std::list<ExpressionNode*> common;
    if ((expr1->mStr == "*" || expr1->mStr == "**") && (expr2->mStr == "*" || expr2->mStr == "**"))
    {
        for (std::list<ExpressionNode*>::iterator i = expr1->mChildren.begin(); i != expr1->mChildren.end(); ++i)
        {
            for (std::list<ExpressionNode*>::iterator j = expr2->mChildren.begin(); j != expr2->mChildren.end(); ++j)
            {
                if (equivExpression(*i, *j))
                    common.push_back(copyOf(*i));
            }
        }
    }

    return common;
}

std::list<ExpressionNode*> coeffTerm(ExpressionNode* expr, ExpressionNode* term)
{
    std::list<ExpressionNode*> coeff;
    if ((expr->mStr == "*" || expr->mStr == "**") && (term->mStr == "*" || term->mStr == "**"))
    {
        for (std::list<ExpressionNode*>::iterator i = expr->mChildren.begin(); i != expr->mChildren.end(); ++i)
        {
            bool containedIn = false;
            for (std::list<ExpressionNode*>::iterator j = term->mChildren.begin(); !containedIn && j != term->mChildren.end(); ++j)
            {
                if (equivExpression(*i, *j))
                    containedIn = true;
            }

            if (!containedIn)
                coeff.push_back(*i);
        }
    }

    return coeff;
}

//
//  Numerical arithmetic evaluators
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

// Evalutes "(-* x)"
bool symbolicNeg(ExpressionNode* op)
{
    if (op->mChildren.size() != 1)     return false; // TODO: arity mismatch

    ExpressionNode* arg = op->mChildren.front();
    if (arg->mStr == "-*") // (- (- x)) ==> x
    {
        assignExprNode(op, arg->mChildren.front());
        delete arg->mChildren.front();
        delete arg;
    }
    
    return true;
}

// Evalutes "(+ <arg0> <arg1> ... )"
bool symbolicAdd(ExpressionNode* op)
{
    if (op->mChildren.size() < 2)     return false; // TODO: arity mismatch

    std::list<ExpressionNode*>::iterator i = op->mChildren.begin();
    while (i != op->mChildren.end())
    {       
        bool added = false;
        for (std::list<ExpressionNode*>::iterator j = std::next(i); j != op->mChildren.end(); ++j)
        {
            std::list<ExpressionNode*> common = commonTerm(*i, *j);
            if (!common.empty())                               // (coeff_a + coeff_b) * common
            {
                ExpressionNode* co = new ExpressionNode();
                co->mType = ExpressionNode::OPERATOR;
                co->mStr = "**";
                co->mPrecedence = operatorPrec(co->mStr);
                for (ExpressionNode* child : common)
                {
                    co->mChildren.push_back(child);
                    child->mParent = co;
                }
                
                std::list<ExpressionNode*> coeff_lhs = coeffTerm(*i, co);
                std::list<ExpressionNode*> coeff_rhs = coeffTerm(*j, co);
                ExpressionNode *lhs, *rhs;
                
                if (coeff_lhs.size() == 0) // coeff = 1
                {
                    lhs = new ExpressionNode();
                    lhs->mType = ExpressionNode::INTEGER;
                    lhs->mExact = 1;
                }
                else if (coeff_lhs.size() == 1)
                {
                    lhs = coeff_lhs.front();
                }
                else
                {
                    lhs = new ExpressionNode();
                    lhs->mType = ExpressionNode::OPERATOR;
                    lhs->mStr = "**";
                    lhs->mPrecedence = operatorPrec(lhs->mStr);
                    lhs->mChildren = coeff_lhs;
                    for (ExpressionNode* child : lhs->mChildren)
                        child->mParent = lhs;
                }

                if (coeff_rhs.size() == 0) // coeff = 1
                {
                    rhs = new ExpressionNode();
                    rhs->mType = ExpressionNode::INTEGER;
                    rhs->mExact = 1;
                }
                else if (coeff_rhs.size() == 1)
                {
                    rhs = coeff_rhs.front();
                }
                else
                {
                    rhs = new ExpressionNode();
                    rhs->mType = ExpressionNode::OPERATOR;
                    rhs->mStr = "**";
                    rhs->mPrecedence = operatorPrec(rhs->mStr);
                    rhs->mChildren = coeff_rhs;
                    for (ExpressionNode* child : rhs->mChildren)
                        child->mParent = rhs;
                }

                ExpressionNode* add = new ExpressionNode(); // (coeff_a + coeff_b)
                add->mType = ExpressionNode::OPERATOR;
                add->mStr = "+";
                add->mPrecedence = operatorPrec(add->mStr);
                add->mChildren.push_back(lhs);
                add->mChildren.push_back(rhs);
                lhs->mParent = add;
                rhs->mParent = add;
                evaluateArithmetic(add); // simplify the coefficients

                ExpressionNode* mul = new ExpressionNode(); // coeff * common
                mul->mType = ExpressionNode::OPERATOR;
                mul->mStr = "**";
                mul->mPrecedence = operatorPrec(mul->mStr);
                mul->mChildren.push_back(add);
                mul->mChildren.push_back(co);
                add->mParent = mul;
                co->mParent = mul;

                for (ExpressionNode* itr : co->mChildren) // delete common terms in *i and *j
                {
                    std::list<ExpressionNode*>::iterator ichild = (*i)->mChildren.begin();
                    while (ichild != (*i)->mChildren.end())
                    {
                        std::list<ExpressionNode*>::iterator tmp = std::next(ichild);
                        if (equivExpression(*ichild, itr))
                            delete *ichild;
                        ichild = tmp;
                    }
                    
                    std::list<ExpressionNode*>::iterator jchild = (*j)->mChildren.begin();
                    while (jchild != (*j)->mChildren.end())
                    {
                        std::list<ExpressionNode*>::iterator tmp = std::next(jchild);
                        if (equivExpression(*jchild, itr))
                            delete *jchild;
                        jchild = tmp;
                    }
                }

                if (co->mChildren.size() == 1) // correct: (* x) => x
                {
                    assignExprNode(co, co->mChildren.front());
                    delete co->mChildren.front();
                }

                delete *i; // delete *i and *j
                delete *j;
                op->mChildren.insert(i, mul);
                op->mChildren.erase(i,  std::next(j));
                i = op->mChildren.begin();
                added = true;
                break;
            }
        }

        if (added)      i = op->mChildren.begin();
        else            ++i;   
    }
    
    flattenExpr(op);
    return true;
}

// Evalutes "(* <arg0> <arg1> ... )" or "(** <arg0> <arg1> ... )"
bool symbolicMul(ExpressionNode* op)
{
    if (op->mChildren.size() < 2)     return false; // TODO: arity mismatch

    std::list<ExpressionNode*>::iterator it = op->mChildren.begin();
    while (it != op->mChildren.end()) // // (* a... (* b... ) c...) ==> (* a... b... c...)
    {
        ExpressionNode* child = *it;
        if (child->mStr == "*" || child->mStr == "**")
        {        
            op->mChildren.insert(it, child->mChildren.begin(), child->mChildren.end());
            op->mChildren.erase(it++);
            delete child;
        }
        else
        {
            ++it;
        }     
    }
    
    return true;
}

//
// Arithmetic evaluator
//

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
        if (expr->mStr == "-*")                             return symbolicNeg(expr);
        else if (expr->mStr == "+")                         return symbolicAdd(expr);
        else if (expr->mStr == "-")                         return false;   // Unsupported: symbolic -
        else if (expr->mStr == "*" || expr->mStr == "**")   return symbolicMul(expr);
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