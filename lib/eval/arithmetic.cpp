#include "arithmetic.h"

namespace MathSolver
{

bool isArithmetic(ExprNode* expr)
{
    for (ExprNode* child : expr->children)
    {
        if (!isArithmetic(child))
            return false;
    }

    if (expr->type == ExprNode::FUNCTION)
    {
        return (expr->str == "exp" || expr->str == "log" ||
                expr->str == "sin" || expr->str == "cos" || expr->str == "tan");
    }
    else if (expr->type == ExprNode::OPERATOR)
    {
        return (expr->str == "-*" ||
                expr->str == "+" || expr->str == "-" || expr->str == "**" ||
                expr->str == "*" || expr->str == "/" || expr->str == "%" ||
                expr->str == "^" || expr->str == "!");
    }
    else
    {
        return true;
    }
}

std::list<ExprNode*> commonTerm(ExprNode* expr1, ExprNode* expr2)
{
    std::list<ExprNode*> common;
    if ((expr1->str == "*" || expr1->str == "**") && (expr2->str == "*" || expr2->str == "**"))
    {
        for (auto i = expr1->children.begin(); i != expr1->children.end(); ++i)
        {
            for (auto j = expr2->children.begin(); j != expr2->children.end(); ++j)
            {
                if (equivExpression(*i, *j))
                    common.push_back(copyOf(*i));
            }
        }
    }
    else if ((expr1->str == "*" || expr1->str == "**") && (expr2->type == ExprNode::VARIABLE || expr2->type == ExprNode::CONSTANT))
    {
        for (auto i = expr1->children.begin(); i != expr1->children.end(); ++i)
        {
            if (equivExpression(*i, expr2))
                common.push_back(copyOf(*i));
        }
    }
    else if ((expr1->type == ExprNode::VARIABLE || expr1->type == ExprNode::CONSTANT) && (expr2->str == "*" || expr2->str == "**"))
    {
        for (auto i = expr2->children.begin(); i != expr2->children.end(); ++i)
        {
            if (equivExpression(expr1, *i))
                common.push_back(copyOf(*i));
        }
    }
    else if ((expr1->type == ExprNode::VARIABLE || expr1->type == ExprNode::CONSTANT) && 
             (expr2->type == ExprNode::VARIABLE || expr2->type == ExprNode::CONSTANT))
    {
        if (equivExpression(expr1, expr2))
            common.push_back(copyOf(expr1));
    }

    return common;
}

std::list<ExprNode*> coeffTerm(ExprNode* expr, ExprNode* term)
{
    std::list<ExprNode*> coeff;
    if ((expr->str == "*" || expr->str == "**") && (term->str == "*" || term->str == "**"))
    {
        for (auto i = expr->children.begin(); i != expr->children.end(); ++i)
        {
            bool containedIn = false;
            for (auto j = term->children.begin(); !containedIn && j != term->children.end(); ++j)
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
bool numericNeg(ExprNode* op)
{
    if (op->children.size() != 1)     return false; // TODO: arity mismatch

    if (op->children.front()->type == ExprNode::INTEGER)
    {
        op->type = ExprNode::INTEGER;
        op->exact = -(op->children.front()->exact);
    }

    delete op->children.front();
    op->children.clear();
    op->prec = 0;
    return true;
}

// Evaluates "(+ <num>...)"
bool numericAdd(ExprNode* op)
{
    if (op->children.front()->type == ExprNode::INTEGER)
    {
        op->type = ExprNode::INTEGER;
        op->exact = op->children.front()->exact;
    }  
    else // <inexact>
    { 
        op->type = ExprNode::FLOAT;
        op->inexact = op->children.front()->inexact;
    }
    
    for (auto it = std::next(op->children.begin()); it != op->children.end(); ++it)
    {
        if (op->type == ExprNode::INTEGER && (*it)->type == ExprNode::INTEGER) // <exact> += <exact>
            op->exact += (*it)->exact;
        // TODO: inexact
        
        delete *it;
    }

    op->children.clear();  
    op->prec = 0; 
    return true;
}

// Evaluates "(- <num>...)"
bool numericSub(ExprNode* op)
{
    if (op->children.front()->type == ExprNode::INTEGER)
    {
        op->type = ExprNode::INTEGER;
        op->exact = op->children.front()->exact;
    }  
    else // <inexact>
    { 
        op->type = ExprNode::FLOAT;
        op->inexact = op->children.front()->inexact;
    }
    
    for (auto it = std::next(op->children.begin()); it != op->children.end(); ++it)
    {
        if (op->type == ExprNode::INTEGER && (*it)->type == ExprNode::INTEGER) // <exact> += <exact>
            op->exact -= (*it)->exact;
        // TODO: inexact
        
        delete *it;
    }

    op->children.clear(); 
    op->prec = 0;  
    return true;
}

// Evaluates "(* <num>...)" or "(** <num>...)"
bool numericMul(ExprNode* op)
{
    if (op->children.front()->type == ExprNode::INTEGER)
    {
        op->type = ExprNode::INTEGER;
        op->exact = op->children.front()->exact;
    }  
    else // <inexact>
    { 
        op->type = ExprNode::FLOAT;
        op->inexact = op->children.front()->inexact;
    }
    
    delete op->children.front();
    for (auto it = std::next(op->children.begin()); it != op->children.end(); ++it)
    {
        if (op->type == ExprNode::INTEGER && (*it)->type == ExprNode::INTEGER) // <exact> *= <exact>
            op->exact *= (*it)->exact;
        // TODO: inexact
        
        delete *it;
    }

    op->children.clear();
    op->prec = 0;   
    return true;
}

// Evaluates "<num> / <num>"
bool numericDiv(ExprNode* op)
{
    if (op->children.size() != 2)     return false; // TODO: arity mismatch

    ExprNode* lhs = op->children.front();
    ExprNode* rhs = op->children.back();

    if (lhs->type == ExprNode::INTEGER && rhs->type == ExprNode::INTEGER) // <exact> / <exact>
    {   
        return true; // no exact numerical division: a/b --> a/b
    }

    op->children.clear();
    op->prec = 0;
    delete lhs;
    delete rhs;
    return true;
}

//
//  Symbolic arithmetic evaluators
//

// Evaluates "(exp x)"
bool symbolicExp(ExprNode* op)
{
    if (op->children.size() != 1)      return false;  // TODO: arity mismatch

    if (op->children.front()->str == "log" && op->children.front()->children.size() == 1) // exp(log (x)) --> x
    {
        ExprNode* ln = op->children.front();
        assignExprNode(op, ln->children.front());
        delete ln->children.front();
        delete ln;
    }

    // TODO: more simplifications

    return true;
}

// Evaluates "(log x) or (log b x)"
bool symbolicLog(ExprNode* op)
{
    if (op->children.size() != 1 && op->children.size() != 2)      return false;  // TODO: arity mismatch

    if (op->children.front()->str == "exp" && op->children.front()->children.size() == 1) // (log (exp x)) --> x
    {
        ExprNode* ln = op->children.front();
        assignExprNode(op, ln->children.front());
        delete ln->children.front();
        delete ln;
    }

    // TODO: more simplifications

    return true;
}

// Evaluates "(sin x)"
bool symbolicSin(ExprNode* op)
{
    if (op->children.size() != 1)      return false;  // TODO: arity mismatch

    // TODO: more simplifications

    return true;
}

// Evaluates "(cos x)"
bool symbolicCos(ExprNode* op)
{
    if (op->children.size() != 1)      return false;  // TODO: arity mismatch

    // TODO: more simplifications

    return true;
}

// Evaluates "(tan x)"
bool symbolicTan(ExprNode* op)
{
    if (op->children.size() != 1)      return false;  // TODO: arity mismatch

    // TODO: more simplifications

    return true;
}

// Evalutes "(-* x)"
bool symbolicNeg(ExprNode* op)
{
    if (op->children.size() != 1)     return false; // TODO: arity mismatch

    ExprNode* arg = op->children.front();
    if (arg->str == "-*") // (- (- x)) ==> x
    {
        assignExprNode(op, arg->children.front());
        delete arg->children.front();
        delete arg;
    }
    
    return true;
}

// Shared evaluator for symbolic + and -
void symbolicAddSub(ExprNode* op, const char* str)
{
    auto i = op->children.begin();
    while (i != op->children.end())
    {       
        bool added = false;
        for (auto j = std::next(i); j != op->children.end(); ++j)
        {
            std::list<ExprNode*> common = commonTerm(*i, *j);
            if (!common.empty())    // (coeff_a +- coeff_b) * common
            {
                ExprNode* co = new ExprNode();
                co->type = ExprNode::OPERATOR;
                co->str = "**";
                co->prec = operatorPrec(co->str);
                for (ExprNode* child : common)
                {
                    co->children.push_back(child);
                    child->parent = co;
                }
                
                std::list<ExprNode*> coeff_lhs = coeffTerm(*i, co);
                std::list<ExprNode*> coeff_rhs = coeffTerm(*j, co);
                ExprNode *lhs, *rhs;
                
                if (coeff_lhs.size() == 0) // coeff = 1
                {
                    lhs = new ExprNode();
                    lhs->type = ExprNode::INTEGER;
                    lhs->exact = 1;
                }
                else if (coeff_lhs.size() == 1)
                {
                    lhs = coeff_lhs.front();
                }
                else
                {
                    lhs = new ExprNode();
                    lhs->type = ExprNode::OPERATOR;
                    lhs->str = str;
                    lhs->prec = operatorPrec(lhs->str);
                    lhs->children = coeff_lhs;
                    for (ExprNode* child : lhs->children)
                        child->parent = lhs;
                }

                if (coeff_rhs.size() == 0) // coeff = 1
                {
                    rhs = new ExprNode();
                    rhs->type = ExprNode::INTEGER;
                    rhs->exact = 1;
                }
                else if (coeff_rhs.size() == 1)
                {
                    rhs = coeff_rhs.front();
                }
                else
                {
                    rhs = new ExprNode();
                    rhs->type = ExprNode::OPERATOR;
                    rhs->str = "**";
                    rhs->prec = operatorPrec(rhs->str);
                    rhs->children = coeff_rhs;
                    for (ExprNode* child : rhs->children)
                        child->parent = rhs;
                }

                ExprNode* add = new ExprNode(); // (coeff_a +- coeff_b)
                add->type = ExprNode::OPERATOR;
                add->str = str;
                add->prec = operatorPrec(add->str);
                add->children.push_back(lhs);
                add->children.push_back(rhs);
                lhs->parent = add;
                rhs->parent = add;
                evaluateArithmetic(add); // simplify the coefficients

                ExprNode* mul = new ExprNode(); // coeff * common
                mul->type = ExprNode::OPERATOR;
                mul->str = "**";
                mul->prec = operatorPrec(mul->str);
                mul->children.push_back(add);
                mul->children.push_back(co);
                add->parent = mul;
                co->parent = mul;

                for (ExprNode* itr : co->children) // delete common terms in *i and *j
                {
                    auto ichild = (*i)->children.begin();
                    while (ichild != (*i)->children.end())
                    {
                        auto tmp = std::next(ichild);
                        if (equivExpression(*ichild, itr))
                            delete *ichild;
                        ichild = tmp;
                    }
                    
                    auto jchild = (*j)->children.begin();
                    while (jchild != (*j)->children.end())
                    {
                        auto tmp = std::next(jchild);
                        if (equivExpression(*jchild, itr))
                            delete *jchild;
                        jchild = tmp;
                    }
                }

                if (co->children.size() == 1) // correct: (* x) => x
                {
                    assignExprNode(co, co->children.front());
                    delete co->children.front();
                }
                
                evaluateArithmetic(mul);
                delete *i; // delete *i and *j
                delete *j;
                op->children.insert(i, mul);
                op->children.erase(i,  std::next(j));
                i = op->children.begin();
                added = true;
                break;
            }
        }

        if (added)      i = op->children.begin();
        else            ++i;   
    }
    
    if (op->children.size() == 1) // (+- (...)) ==> (...)
    {
        ExprNode* arg = op->children.front();
        assignExprNode(op, arg);
        delete arg;
    }

    flattenExpr(op);
}

// Evalutes "(+ <arg0> <arg1> ... )"
bool symbolicAdd(ExprNode* op)
{
    if (op->children.size() < 2)     return false; // TODO: arity mismatch

    auto it = op->children.begin();
    auto next = std::next(it); 
    if ((*it)->str == "-*" && (*next)->str != "-*") // (+ (-* a) b ...) ==> (+ (- a b) ...) 
    {
        (*it)->str = "-";
        (*it)->children.push_front(*next);
        op->children.erase(next);
        next = std::next(it);
    }

    while (next != op->children.end()) // (+ a (-* b) ...) ==> (+ (- a b) ...)
    {
        if ((*next)->str == "-*")
        {
            (*next)->children.push_front(*it);
            op->children.erase(it);
            (*next)->str = "-";
            it = next;
            ++next;
        }
        else
        {
            ++it;
            ++next;
        }    
    }

    if (op->children.size() == 1) // (+ (- a b)) ==> (- a b)
    {
        ExprNode* arg = op->children.front();
        assignExprNode(op, arg);
        delete arg;
        symbolicAddSub(op, "-");
    }
    else
    {
        symbolicAddSub(op, "+");
    }
  
    return true;
}

// Evalutes "(- <arg0> <arg1> ... )"
bool symbolicSub(ExprNode* op)
{
    if (op->children.size() < 2)     return false; // TODO: arity mismatch
    
    auto it = op->children.begin();
    auto next = std::next(it);
    while (next != op->children.end()) // (- a (-* b) c d) ==> (- (+ a b) c d)
    {
        if ((*next)->str == "-*")
        {
            (*next)->children.push_front(*it);
            op->children.erase(it);
            (*next)->str = "+";
            it = next;
            ++next;
        }
        else
        {
            ++it;
            ++next;
        }    
    }

    if (op->children.size() == 1) // (- (+ a b)) ==> (+ a b)
    {
        ExprNode* arg = op->children.front();
        assignExprNode(op, arg);
        delete arg;
        symbolicAddSub(op, "+");
    }
    else
    {
        symbolicAddSub(op, "-");
    }
  
    return true;
}

// Evalutes "(* <arg0> <arg1> ... )" or "(** <arg0> <arg1> ... )"
bool symbolicMul(ExprNode* op)
{
    if (op->children.size() == 0)     return false; // TODO: arity mismatch
    if (op->children.size() == 1) // special case: (* x) ==> x, internal use only
    {
        ExprNode* arg = op->children.front();
        assignExprNode(op, arg);
        delete arg;
        return true;
    }

    auto it = op->children.begin();
    while (it != op->children.end())
    {
        auto it2 = std::next(it);
        if (((*it)->type == ExprNode::INTEGER && (*it)->exact.isZero()) || // (* 0 a ...) ==> 0
            ((*it)->type == ExprNode::FLOAT && (*it)->inexact == 0.0))
        {
            for (auto c : op->children)
                freeExpression(c);
            op->children.clear();
            op->type = ExprNode::INTEGER;
            op->exact = Integer(0);
            op->prec = 0;
            it = op->children.begin();
        }
        else if (((*it)->type == ExprNode::INTEGER && (*it)->exact == Integer(1)) || // (* 1 a ...) ==> (* a ...)
            ((*it)->type == ExprNode::FLOAT && (*it)->inexact == 1.0))
        {
            op->children.erase(it);
            delete *it;
            it = it2;
        }
        else if ((*it)->str == "*" || (*it)->str == "**") // (* a... (* b... ) c...) ==> (* a... b... c...)
        {        
            op->children.insert(it, (*it)->children.begin(), (*it)->children.end());
            op->children.erase(it);
            delete *it;
            it = it2;
        }   
        else if (it2 != op->children.end())
        {
            if ((*it)->str == "/" && (*it2)->str == "/") // (* (/ a b) (/ c d) ...) ==> (* (/ (* a c) (* b d)) ...)
            {
                ExprNode* num = (*it)->children.front();
                ExprNode* den = (*it)->children.back();
                (*it)->children.clear();

                ExprNode* mul1 = new ExprNode();
                mul1->type = ExprNode::OPERATOR;
                mul1->str = "**";
                mul1->prec = operatorPrec(mul1->str);
                mul1->children.push_back(num);
                mul1->children.push_back((*it2)->children.front());
                evaluateArithmetic(mul1);

                ExprNode* mul2 = new ExprNode();
                mul2->type = ExprNode::OPERATOR;
                mul2->str = "**";
                mul2->prec = operatorPrec(mul2->str);
                mul2->children.push_back(den);
                mul2->children.push_back((*it2)->children.back());
                evaluateArithmetic(mul2);

                (*it)->children.push_front(mul1);
                (*it)->children.push_back(mul2);
                num->parent = mul1;
                (*it2)->children.front()->parent = mul1;
                den->parent = mul2;
                (*it2)->children.back()->parent = mul2;
                
                delete *it2;
                op->children.erase(it2);
                evaluateArithmetic(num);
                evaluateArithmetic(den);
                evaluateArithmetic(*it);
                ++it;  
            }
            else if ((*it)->str == "/") // (* (/ a b) c ...) ==> (* (/ (* a c) b) ...)
            {
                ExprNode* num = (*it)->children.front();
                (*it)->children.erase((*it)->children.begin());

                ExprNode* mul = new ExprNode();
                mul->type = ExprNode::OPERATOR;
                mul->str = "**";
                mul->prec = operatorPrec(mul->str);
                mul->children.push_back(num);
                mul->children.push_back(*it2);
                (*it2)->parent = mul;
                num->parent = mul;
                evaluateArithmetic(mul);

                (*it)->children.push_front(mul);
                mul->parent = *it;
                op->children.erase(it2);
                evaluateArithmetic(num);
                evaluateArithmetic(*it);
                ++it;  
            }
            else if ((*it2)->str == "/") // (* ... a (/ b c)) ==> (* ... (/ (* a b) c))
            {              
                ExprNode* num = (*it2)->children.front();
                (*it2)->children.erase((*it2)->children.begin());

                ExprNode* mul = new ExprNode();
                mul->type = ExprNode::OPERATOR;
                mul->str = "**";
                mul->prec = operatorPrec(mul->str);
                mul->children.push_back(*it);
                mul->children.push_back(num);
                (*it)->parent = mul;
                num->parent = mul;
                evaluateArithmetic(mul);

                (*it2)->children.push_front(mul);
                mul->parent = *it2;
                it = op->children.erase(it);
                evaluateArithmetic(num);
                evaluateArithmetic(*it2);
            }     
            else
            {
                ++it;
            }
        }
        else
        {
            ++it;
        }
    }

    if (op->children.size() == 1) // possibly need second pass
        symbolicMul(op);
    
    return true;
}

// Evalutes "(/ <arg0> <arg1>)"
bool symbolicDiv(ExprNode* op)
{
    if (op->children.size() != 2)  return false; // TODO: arity mismatch

    ExprNode* num = op->children.front();
    ExprNode* den = op->children.back();

    if (num->str == "/" && den->str == "/") // (/ (/ a b) (/ c d)) ==> (/ (* a d) (* b c))
    {
        num->str = "**";
        num->prec = operatorPrec(num->str);
        den->str = "**";
        den->prec = operatorPrec(den->str);

        ExprNode* tmp = num->children.back();
        ExprNode* tmp2 = den->children.back();

        num->children.erase(std::prev(num->children.end()));
        den->children.erase(std::prev(den->children.end()));
        num->children.push_back(tmp2);
        den->children.push_front(tmp);
        tmp->parent = den;
        tmp2->parent = num;

        evaluateArithmetic(num);
        evaluateArithmetic(den);
    }
    else if (num->str == "/") // (/ (/ a b) c) ==> (/ a (* b c))
    {
        num->str = "**";
        num->prec = operatorPrec(num->str);

        ExprNode* tmp = num->children.front();
        ExprNode* tmp2 = op->children.back();

        num->children.erase(num->children.begin());
        op->children.erase(std::prev(op->children.end()));
        op->children.push_front(tmp);
        num->children.push_back(tmp2);
        tmp->parent = op;
        tmp2->parent = num;
        
        num = op->children.front(); // reassign numerator and denominator
        den = op->children.back();
        evaluateArithmetic(den);
    }
    else if (den->str == "/") // (/ a (/ b c)) ==> (/ (* a c) b)
    {
        den->str = "**";
        den->prec = operatorPrec(num->str);

        ExprNode* tmp = op->children.front();
        ExprNode* tmp2 = den->children.front();

        op->children.erase(op->children.begin());
        den->children.erase(den->children.begin());
        den->children.push_front(tmp);
        op->children.push_back(tmp2);
        tmp->parent = den;
        tmp2->parent = op;
        num = op->children.front(); // reassign numerator and denominator
        den = op->children.back();
        evaluateArithmetic(num);
    }

    // second pass
    if ((num->str == "*" || num->str == "**") && // (/ (* a b) (* b c)) ==> (/ a c)
        (den->str == "*" || den->str == "**"))
    {
        bool changed = false;
        num->str = "**";
        den->str = "**";
        for (auto it = num->children.begin(); it != num->children.end(); ++it)
        {
            for (auto it2 = den->children.begin(); it2 != den->children.end(); ++it2)
            {
                if (equivExpression(*it, *it2))
                {
                    freeExpression(*it);
                    freeExpression(*it2);
                    num->children.erase(it--);
                    den->children.erase(it2);
                    changed = true;
                    break;
                }
            }
        }

        if (changed)
        {
            if (num->children.empty())
            {
                num->type = ExprNode::INTEGER;
                num->exact = 1;
                num->prec = 0;
                num->str.clear();
                num->children.clear();
            }     
        
            if (den->children.empty())
            {
                den->type = ExprNode::INTEGER;
                den->exact = 1;
                den->prec = 0;
                den->str.clear();
                den->children.clear();
            }
            
            evaluateArithmetic(num);
            evaluateArithmetic(den);
        }
    }
    else if (num->str == "*" || num->str == "**") // (/ (* a b c) b) ==> (* a c)
    {
        num->str = "**";
        for (auto it = num->children.begin(); it != num->children.end(); ++it)
        {
            if (equivExpression(*it, den))
            {
                freeExpression(*it);
                freeExpression(den);
                num->children.erase(it);
                op->children.erase(std::prev(op->children.end()));
                evaluateArithmetic(num); // simplify numerator
                break;
            }
        }
    }
    else if (den->str == "*" || den->str == "**") // (/ a (* a b c)) ==> (/ 1 (* b c))
    {
        den->str = "**";
        for (auto it = den->children.begin(); it != den->children.end(); ++it)
        {
            if (equivExpression(*it, num))
            {
                freeExpression(*it);
                den->children.erase(it);
                num->type = ExprNode::INTEGER;
                num->exact = 1;
                num->prec = 0;
                num->str.clear();
                num->children.clear();
                evaluateArithmetic(den); // simplify denominator
                break;
            }
        }
    }
    else if (equivExpression(num, den)) // (/ a a) ==> 1
    {
        op->type = ExprNode::INTEGER;
        op->exact = 1;
        op->prec = 0;
        op->str.clear();
        op->children.clear();
        freeExpression(num);
        freeExpression(den);
    } 
    else if (op->children.back()->exact == Integer(1)) // (/ x 1) == x
    {
        ExprNode* arg = op->children.front();
        delete op->children.back();
        op->children.clear();
        assignExprNode(op, arg);
        delete arg;
    }  // else do nothing

    if (op->str == "/" && op->children.size() == 1)
    {
        ExprNode* arg = op->children.front();
        assignExprNode(op, arg);
        delete arg;
    }
  
    return true; // TODO: evaluate
}

//
// Arithmetic evaluator
//

bool evaluateArithmetic(ExprNode* expr)
{
    if (isNumerical(expr))
    {
        if (expr->str == "-*")                             return numericNeg(expr);
        else if (expr->str == "+")                         return numericAdd(expr);
        else if (expr->str == "-")                         return numericSub(expr);
        else if (expr->str == "*" || expr->str == "**")   return numericMul(expr);
        else if (expr->str == "/")                         return true; // no evaluation

        else if (expr->str == "exp")                       return false;   // Unsupported: numerical exp
        else if (expr->str == "log")                       return false;   // Unsupported: numerical log
        else if (expr->str == "sin")                       return false;   // Unsupported: numerical sin
        else if (expr->str == "cos")                       return false;   // Unsupported: numerical cos
        else if (expr->str == "tan")                       return false;   // Unsupported: numerical tan
    }
    else // symbolic
    {
        if (expr->str == "-*")                             return symbolicNeg(expr);
        else if (expr->str == "+")                         return symbolicAdd(expr);
        else if (expr->str == "-")                         return symbolicSub(expr);
        else if (expr->str == "*" || expr->str == "**")   return symbolicMul(expr);
        else if (expr->str == "/")                         return symbolicDiv(expr);  

        else if (expr->str == "exp")                       return symbolicExp(expr);
        else if (expr->str == "log")                       return symbolicLog(expr);
        else if (expr->str == "sin")                       return symbolicSin(expr);
        else if (expr->str == "cos")                       return symbolicCos(expr);
        else if (expr->str == "tan")                       return symbolicTan(expr);                                         
    }

    return false; // TODO: unsupported op
}

} // end