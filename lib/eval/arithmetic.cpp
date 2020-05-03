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
                op->name() == "*" || op->name() == "/" || op->name() == "%" ||
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
                if (equivExpression(*i, *j))
                    common.push_back(copyOf(*i));
            }
        }
    }
    else if (expr1->isOperator() && (((OpNode*)expr1)->name() == "*" || ((OpNode*)expr1)->name() == "**") && 
             (expr2->type() == ExprNode::VARIABLE || expr2->type() == ExprNode::CONSTANT))
    {
        for (auto i = expr1->children().begin(); i != expr1->children().end(); ++i)
        {
            if (equivExpression(*i, expr2))
                common.push_back(copyOf(*i));
        }
    }
    else if ((expr1->type() == ExprNode::VARIABLE || expr1->type() == ExprNode::CONSTANT) && 
             expr2->isOperator() && (((OpNode*)expr2)->name() == "*" || ((OpNode*)expr2)->name() == "**"))
    {
        for (auto i = expr2->children().begin(); i != expr2->children().end(); ++i)
        {
            if (equivExpression(expr1, *i))
                common.push_back(copyOf(*i));
        }
    }
    else if ((expr1->type() == ExprNode::VARIABLE || expr1->type() == ExprNode::CONSTANT) && 
             (expr2->type() == ExprNode::VARIABLE || expr2->type() == ExprNode::CONSTANT))
    {
        if (equivExpression(expr1, expr2))
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
ExprNode* numericNeg(ExprNode* op)
{
    if (op->children().size() != 1)     return op; // TODO: arity mismatch

    if (op->children().front()->type() == ExprNode::INTEGER)
        ((IntNode*)op->children().front())->setValue(-((IntNode*)op->children().front())->value());
    else
        ((FloatNode*)op->children().front())->setValue(-((FloatNode*)op->children().front())->value());

    return moveNode(op, op->children().front());
}

// Evaluates "(+ <num>...)"
ExprNode* numericAdd(ExprNode* op)
{
    ExprNode* acc;
    if (op->children().front()->type() == ExprNode::INTEGER)  
        acc = new IntNode(((IntNode*)op->children().front())->value());
    else                                                      
        acc = new FloatNode(((FloatNode*)op->children().front())->value());
    delete op->children().front();

    for (auto it = std::next(op->children().begin()); it != op->children().end(); ++it)
    {
        if (acc->type() == ExprNode::INTEGER && (*it)->type() == ExprNode::INTEGER) // <exact> += <exact>
            ((IntNode*)acc)->value() += ((IntNode*)*it)->value();
        // TODO: inexact
        
        delete *it;
    }

    return moveNode(op, acc);
}

// Evaluates "(- <num>...)"
ExprNode* numericSub(ExprNode* op)
{
    ExprNode* acc;
    if (op->children().front()->type() == ExprNode::INTEGER)  
        acc = new IntNode(((IntNode*)op->children().front())->value());
    else                                                      
        acc = new FloatNode(((FloatNode*)op->children().front())->value());
    delete op->children().front();

    for (auto it = std::next(op->children().begin()); it != op->children().end(); ++it)
    {
        if (acc->type() == ExprNode::INTEGER && (*it)->type() == ExprNode::INTEGER) // <exact> -= <exact>
            ((IntNode*)acc)->value() -= ((IntNode*)*it)->value();
        // TODO: inexact
        
        delete *it;
    }

    return moveNode(op, acc);
}

// Evaluates "(* <num>...)" or "(** <num>...)"
ExprNode* numericMul(ExprNode* op)
{
    ExprNode* acc;
    if (op->children().front()->type() == ExprNode::INTEGER)  
        acc = new IntNode(((IntNode*)op->children().front())->value());
    else                                                      
        acc = new FloatNode(((FloatNode*)op->children().front())->value());
    delete op->children().front();

    for (auto it = std::next(op->children().begin()); it != op->children().end(); ++it)
    {
        if (acc->type() == ExprNode::INTEGER && (*it)->type() == ExprNode::INTEGER) // <exact> *= <exact>
            ((IntNode*)acc)->value() *= ((IntNode*)*it)->value();
        // TODO: inexact
        
        delete *it;
    }
 
    return moveNode(op, acc);
}

// Evaluates "<num> / <num>"
ExprNode* numericDiv(ExprNode* op)
{
    if (op->children().size() != 2)     return op; // TODO: arity mismatch

    ExprNode* lhs = op->children().front();
    ExprNode* rhs = op->children().back();

    if (lhs->type() == ExprNode::INTEGER && rhs->type() == ExprNode::INTEGER) // <exact> / <exact>
    {   
        return op; // no exact numerical division: a/b --> a/b
    }

    return op;
}

//
//  Symbolic arithmetic evaluators
//

// Evaluates "(exp x)"
ExprNode* symbolicExp(ExprNode* op)
{
    if (op->children().size() != 1)      return op;  // TODO: arity mismatch

    if (op->children().front()->isOperator() && // (exp (log x)) --> x
        ((OpNode*)op->children().front())->name() == "log" && op->children().front()->children().size() == 1) 
    {
        ExprNode* ln = op->children().front();
        op = moveNode(op, ln->children().front());
        delete ln;
    }

    // TODO: more simplifications

    return op;
}

// Evaluates "(log x) or (log b x)"
ExprNode* symbolicLog(ExprNode* op)
{
    if (op->children().size() != 1 && op->children().size() != 2)      return op;  // TODO: arity mismatch

    if (op->children().front()->isOperator() && // (log (exp x)) --> x
        ((OpNode*)op->children().front())->name() == "exp" && op->children().front()->children().size() == 1) 
    {
        ExprNode* ln = op->children().front();
        op = moveNode(op, ln->children().front());
        delete ln;
    }

    // TODO: more simplifications

    return op;
}

// Evaluates "(sin x)"
ExprNode* symbolicSin(ExprNode* op)
{
    if (op->children().size() != 1)      return op;  // TODO: arity mismatch

    // TODO: more simplifications

    return op;
}

// Evaluates "(cos x)"
ExprNode* symbolicCos(ExprNode* op)
{
    if (op->children().size() != 1)      return op;  // TODO: arity mismatch

    // TODO: more simplifications

    return op;
}

// Evaluates "(tan x)"
ExprNode* symbolicTan(ExprNode* op)
{
    if (op->children().size() != 1)      return op;  // TODO: arity mismatch

    // TODO: more simplifications

    return op;
}

// Evalutes "(-* x)"
ExprNode* symbolicNeg(ExprNode* op)
{
    if (op->children().size() != 1)     return op; // TODO: arity mismatch

    ExprNode* arg = op->children().front();
    if (arg->isOperator() && ((OpNode*)arg)->name() == "-*") // (- (- x)) ==> x
    {
        op = moveNode(op, arg->children().front());
        delete arg;
    }
    
    return op;
}

// Shared evaluator for symbolic + and -
ExprNode* symbolicAddSub(ExprNode* op, const char* str)
{
    auto i = op->children().begin();
    while (i != op->children().end())
    {       
        bool added = false;
        for (auto j = std::next(i); j != op->children().end(); ++j)
        {
            std::list<ExprNode*> common = commonTerm(*i, *j);
            if (!common.empty())    // (coeff_a +- coeff_b) * common
            {
                ExprNode* co = new OpNode("**");
                for (ExprNode* child : common)
                {
                    co->children().push_back(child);
                    child->setParent(co);
                }
                
                std::list<ExprNode*> coeff_lhs = coeffTerm(*i, co);
                std::list<ExprNode*> coeff_rhs = coeffTerm(*j, co);
                ExprNode *lhs, *rhs;
                
                if (coeff_lhs.size() == 0) // coeff = 1
                {
                    lhs = new IntNode(Integer(1));
                }
                else if (coeff_lhs.size() == 1)
                {
                    lhs = coeff_lhs.front();
                }
                else
                {
                    lhs = new OpNode(str);
                    lhs->children() = coeff_lhs;
                    for (ExprNode* child : lhs->children())
                        child->setParent(lhs);
                }

                if (coeff_rhs.size() == 0) // coeff = 1
                {
                    rhs = new IntNode(Integer(1));
                }
                else if (coeff_rhs.size() == 1)
                {
                    rhs = coeff_rhs.front();
                }
                else
                {
                    rhs = new OpNode("**");
                    rhs->children() = coeff_rhs;
                    for (ExprNode* child : rhs->children())
                        child->setParent(rhs);
                }

                if (co->children().size() == 1) // correct: (* x) => x
                    co = moveNode(co, co->children().front());

                ExprNode* add = new OpNode(str); // (coeff_a +- coeff_b)
                add->children().push_back(lhs);
                add->children().push_back(rhs);
                lhs->setParent(add);
                rhs->setParent(add);
                add = evaluateArithmetic(add); // simplify the coefficients

                ExprNode* mul = new OpNode("**"); // coeff * common
                mul->children().push_back(add);
                mul->children().push_back(co);
                add->setParent(mul);
                co->setParent(mul);

                for (ExprNode* itr : co->children()) // delete common terms in *i and *j
                {
                    auto ichild = (*i)->children().begin();
                    while (ichild != (*i)->children().end())
                    {
                        if (equivExpression(*ichild, itr))
                        {
                            delete *ichild;
                            ichild = (*i)->children().erase(ichild);
                        }
                        else
                        {
                            ++ichild;
                        }           
                    }
                    
                    auto jchild = (*j)->children().begin();
                    while (jchild != (*j)->children().end())
                    {
                        if (equivExpression(*jchild, itr))
                        {
                            delete *jchild;
                            ichild = (*j)->children().erase(jchild);
                        }
                        else
                        {
                            ++jchild;
                        }           
                    }
                }
                
                mul = evaluateArithmetic(mul);
                delete *i; // delete *i and *j
                delete *j;
                op->children().insert(i, mul);
                op->children().erase(i, std::next(j));
                i = op->children().begin();
                added = true;
                break;
            }
        }

        if (added)      i = op->children().begin();
        else            ++i;   
    }
    
    if (op->children().size() == 1) // (+- (...)) ==> (...)
        return moveNode(op, op->children().front());

    flattenExpr(op);
    return op;
}

// Evalutes "(+ <arg0> <arg1> ... )"
ExprNode* symbolicAdd(ExprNode* op)
{
    if (op->children().size() < 2)     return op; // TODO: arity mismatch

    auto it = op->children().begin();
    auto next = std::next(it); 
    if ((*it)->isOperator() && ((OpNode*)*it)->name() == "-*" && // (+ (-* a) b ...) ==> (+ (- b a) ...) 
        (!(*next)->isOperator() || ((OpNode*)*next)->name() != "-*"))
    {
        ((OpNode*)*it)->setName("-");
        (*it)->children().push_front(*next);
        op->children().erase(next);
        next = std::next(it);
    }

    for (; next != op->children().end(); ++next) // (+ a (-* b) c d) ==> (+ (- a b) c d)
    {
        if ((*next)->isOperator() && ((OpNode*)*next)->name() == "-*")
        {
            (*next)->children().push_front(*it);
            op->children().erase(it);
            ((OpNode*)*next)->setName("-");
            it = next;
        }
        else
        {
            ++it;
        }    
    }

    if (op->children().size() == 1) // (- (+ a b)) ==> (+ a b)
        return symbolicAddSub(moveNode(op, op->children().front()), "-");

    return symbolicAddSub(op, "+");
}

// Evalutes "(- <arg0> <arg1> ... )"
ExprNode* symbolicSub(ExprNode* op)
{
    if (op->children().size() < 2)     return op; // TODO: arity mismatch
    
    auto it = op->children().begin();
    for (auto next = std::next(it); next != op->children().end(); ++next) // (- a (-* b) c d) ==> (- (+ a b) c d)
    {
        if ((*next)->isOperator() && ((OpNode*)*next)->name() == "-*")
        {
            (*next)->children().push_front(*it);
            op->children().erase(it);
            ((OpNode*)*next)->setName("+");
            it = next;
        }
        else
        {
            ++it;
        }    
    }

    if (op->children().size() == 1) // (- (+ a b)) ==> (+ a b)
        return symbolicAddSub(moveNode(op, op->children().front()), "+");

    return symbolicAddSub(op, "-");
}

// Evalutes "(* <arg0> <arg1> ... )" or "(** <arg0> <arg1> ... )"
ExprNode* symbolicMul(ExprNode* op)
{
    if (op->children().size() == 0)     return op; // TODO: arity mismatch
    if (op->children().size() == 1) // special case: (* x) ==> x, internal use only
    {
        return moveNode(op, op->children().front());
    }

    auto it = op->children().begin();
    while (it != op->children().end())
    {
        auto it2 = std::next(it);
        if (((*it)->type() == ExprNode::INTEGER && ((IntNode*)*it)->value().isZero()) || // (* 0 a ...) ==> 0
            ((*it)->type() == ExprNode::FLOAT && ((FloatNode*)*it)->value() == 0.0))
        {
            for (auto c : op->children())
                freeExpression(c);
            return moveNode(op, new IntNode());
        }
        else if (((*it)->type() == ExprNode::INTEGER && ((IntNode*)*it)->value() == Integer(1)) || // (* 1 a ...) ==> (* a ...)
                ((*it)->type() == ExprNode::FLOAT && ((FloatNode*)*it)->value() == 1.0))
        {
            op->children().erase(it);
            delete *it;
            it = it2;
        }
        else if ((*it)->isOperator() &&     // (* a... (* b... ) c...) ==> (* a... b... c...)
                (((OpNode*)*it)->name() == "*" || ((OpNode*)*it)->name() == "**"))
        {        
            op->children().insert(it, (*it)->children().begin(), (*it)->children().end());
            op->children().erase(it);
            delete *it;
            it = it2;
        }   
        else if (it2 != op->children().end())
        {
            if ((*it)->isOperator() && (*it2)->isOperator() && // (* (/ a b) (/ c d) ...) ==> (* (/ (* a c) (* b d)) ...)
                ((OpNode*)*it)->name() == "/" && ((OpNode*)*it2)->name() == "/") 
            {
                ExprNode* num = (*it)->children().front();
                ExprNode* den = (*it)->children().back();

                ExprNode* mul1 = new OpNode("**");
                mul1->children().push_back(num);
                mul1->children().push_back((*it2)->children().front());
                mul1 = evaluateArithmetic(mul1);

                ExprNode* mul2 = new OpNode("**");
                mul2->children().push_back(den);
                mul2->children().push_back((*it2)->children().back());
                mul2 = evaluateArithmetic(mul2);
                
                (*it)->children().clear();
                (*it)->children().push_front(mul1);
                (*it)->children().push_back(mul2);
                num->setParent(mul1);
                (*it2)->children().front()->setParent(mul1);
                den->setParent(mul2);
                (*it2)->children().back()->setParent(mul2);
                
                delete *it2;
                op->children().erase(it2);
                num = evaluateArithmetic(num);
                den = evaluateArithmetic(den);
                *it = evaluateArithmetic(*it);
                ++it;  
            }
            else if ((*it)->isOperator() && ((OpNode*)*it)->name() == "/") // (* (/ a b) c ...) ==> (* (/ (* a c) b) ...)
            {
                ExprNode* num = (*it)->children().front();
                ExprNode* mul = new OpNode("**");

                (*it)->children().erase((*it)->children().begin());
                mul->children().push_back(num);
                mul->children().push_back(*it2);
                (*it2)->setParent(mul);
                num->setParent(mul);
                mul = evaluateArithmetic(mul);

                (*it)->children().push_front(mul);
                mul->setParent(*it);
                op->children().erase(it2);
                num = evaluateArithmetic(num);
                *it = evaluateArithmetic(*it);
                ++it;  
            }
            else if ((*it2)->isOperator() && ((OpNode*)*it2)->name() == "/") // (* ... a (/ b c)) ==> (* ... (/ (* a b) c))
            {              
                ExprNode* num = (*it2)->children().front();
                ExprNode* mul = new OpNode("**");

                (*it2)->children().erase((*it2)->children().begin());
                mul->children().push_back(*it);
                mul->children().push_back(num);
                (*it)->setParent(mul);
                num->setParent(mul);
                mul = evaluateArithmetic(mul);

                (*it2)->children().push_front(mul);
                mul->setParent(*it2);
                it = op->children().erase(it);
                *it2 = evaluateArithmetic(*it2);
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

    if (op->children().size() == 1) // possibly need second pass
        return symbolicMul(op);
  
    return op;
}

// Evalutes "(/ <arg0> <arg1>)"
ExprNode* symbolicDiv(OpNode* op)
{
    if (op->children().size() != 2)  return op; // TODO: arity mismatch

    ExprNode* num = op->children().front();
    ExprNode* den = op->children().back();
    
    if (num->isOperator() && den->isOperator() &&  // (/ (/ a b) (/ c d)) ==> (/ (* a d) (* b c))
        ((OpNode*)num)->name() == "/" && ((OpNode*)den)->name() == "/") 
    {
        ExprNode* tmp = num->children().back();
        ExprNode* tmp2 = den->children().back();

        ((OpNode*)num)->setName("**");
        ((OpNode*)den)->setName("**");
        num->children().erase(std::prev(num->children().end()));
        den->children().erase(std::prev(den->children().end()));
        num->children().push_back(tmp2);
        den->children().push_front(tmp);
        tmp->setParent(den);
        tmp2->setParent(num);

        num = evaluateArithmetic(num);
        den = evaluateArithmetic(den);
    }
    else if (num->isOperator() && ((OpNode*)num)->name() == "/") // (/ (/ a b) c) ==> (/ a (* b c))
    {
        ExprNode* tmp = num->children().front();
        ExprNode* tmp2 = op->children().back();

        ((OpNode*)num)->setName("**");
        num->children().erase(num->children().begin());
        op->children().erase(std::prev(op->children().end()));
        op->children().push_front(tmp);
        num->children().push_back(tmp2);
        tmp->setParent(op);
        tmp2->setParent(num);
        
        num = op->children().front(); // reassign numerator and denominator
        den = op->children().back();
        den = evaluateArithmetic(den);
    }
    else if (den->isOperator() && ((OpNode*)den)->name() == "/") // (/ a (/ b c)) ==> (/ (* a c) b)
    {
        ExprNode* tmp = op->children().front();
        ExprNode* tmp2 = den->children().front();

        ((OpNode*)den)->setName("**");
        op->children().erase(op->children().begin());
        den->children().erase(den->children().begin());
        den->children().push_front(tmp);
        op->children().push_back(tmp2);
        tmp->setParent(den);
        tmp2->setParent(op);
        num = op->children().front(); // reassign numerator and denominator
        den = op->children().back();
        
        num = evaluateArithmetic(num);
        replaceChild(op, num, op->children().begin());
    }

    // second pass
    if (num->isOperator() && den->isOperator() &&
        (((OpNode*)num)->name() == "*" || ((OpNode*)num)->name() == "**") && // (/ (* a b) (* b c)) ==> (/ a c)
        (((OpNode*)den)->name() == "*" || ((OpNode*)den)->name() == "**"))
    {
        bool changed = false;
        ((OpNode*)num)->setName("**");
        ((OpNode*)den)->setName("**");
        for (auto it = num->children().begin(); it != num->children().end(); ++it)
        {
            for (auto it2 = den->children().begin(); it2 != den->children().end(); ++it2)
            {
                if (equivExpression(*it, *it2))
                {
                    freeExpression(*it);
                    freeExpression(*it2);
                    num->children().erase(it--);
                    den->children().erase(it2);
                    changed = true;
                    break;
                }
            }
        }

        if (changed)
        {
            if (num->children().empty())            num = moveNode(num, new IntNode(Integer(1)));
            else if (num->children().size() == 1)   num = moveNode(num, num->children().front());
            else                                    num = evaluateArithmetic(num);

            if (den->children().empty())            den = moveNode(den, new IntNode(Integer(1)));
            else if (den->children().size() == 1)   den = moveNode(den, den->children().front());
            else                                    den = evaluateArithmetic(den);

            replaceChild(op, num, op->children().begin());
            replaceChild(op, den, std::next(op->children().begin()));
        }
    }
    else if (num->isOperator() && (((OpNode*)num)->name() == "*" || ((OpNode*)num)->name() == "**")) // (/ (* a b c) b) ==> (* a c)
    {
        ((OpNode*)num)->setName("**");
        for (auto it = num->children().begin(); it != num->children().end(); ++it)
        {
            if (equivExpression(*it, den))
            {
                freeExpression(*it);
                freeExpression(den);
                num->children().erase(it);
                op->children().erase(std::prev(op->children().end()));
                num = evaluateArithmetic(num); // simplify numerator
                replaceChild(op, num, op->children().begin());
                break;
            }
        }
    }
    else if (den->isOperator() && (((OpNode*)den)->name() == "*" || ((OpNode*)den)->name() == "**")) // (/ a (* a b c)) ==> (/ 1 (* b c))
    {
        ((OpNode*)den)->setName("**");
        for (auto it = den->children().begin(); it != den->children().end(); ++it)
        {
            if (equivExpression(*it, num))
            {
                num = moveNode(num, new IntNode(Integer(1)));
                freeExpression(*it);
                den->children().erase(it);
                den = evaluateArithmetic(den); // simplify denominator
                replaceChild(op, num, (op->children().begin()));
                replaceChild(op, den, std::next(op->children().begin()));
                break;
            }
        }
    }
    else if (equivExpression(num, den)) // (/ a a) ==> 1
    {
        freeExpression(num);
        freeExpression(den);
        return moveNode(op, new IntNode(Integer(1)));
    } 
    else if (op->children().back()->type() == ExprNode::INTEGER && ((IntNode*)op->children().back())->value() == Integer(1)) // (/ x 1) == x
    {
        delete op->children().back();
        op->children().clear();
        return moveNode(op, op->children().front());
    }  // else do nothing

    if (((OpNode*)op)->name() == "/" && op->children().size() == 1)
        return moveNode(op, op->children().front());
  
    return op;
}

//
// Arithmetic evaluator
//

ExprNode* evaluateArithmetic(ExprNode* expr)
{
    if (expr->isNumber())                         return expr;     // fully evaluated
    if (expr->type() == ExprNode::CONSTANT)       return expr;    // TODO: constant table
    
    if (isNumerical(expr))
    {
        if (expr->type() == ExprNode::OPERATOR)
        {
            OpNode* op = (OpNode*)expr;                  
            if (op->name() == "-*")                             return numericNeg(op);
            else if (op->name() == "+")                         return numericAdd(op);
            else if (op->name() == "-")                         return numericSub(op);
            else if (op->name() == "*" || op->name() == "**")   return numericMul(op);
            else if (op->name() == "/")                         return expr;    // TODO: rational simplication
        }
        else
        {
            FuncNode* func = (FuncNode*)expr; 
            if (func->name() == "exp")              return expr;   // Unsupported: numerical exp
            else if (func->name() == "log")         return expr;   // Unsupported: numerical log
            else if (func->name() == "sin")         return expr;   // Unsupported: numerical sin
            else if (func->name() == "cos")         return expr;   // Unsupported: numerical cos
            else if (func->name() == "tan")         return expr;   // Unsupported: numerical tan
        }
    }
    else // symbolic
    {
        if (expr->type() == ExprNode::OPERATOR)
        {
            OpNode* op = (OpNode*)expr; 
            if (op->name() == "-*")                             return symbolicNeg(op);
            else if (op->name() == "+")                         return symbolicAdd(op);
            else if (op->name() == "-")                         return symbolicSub(op);
            else if (op->name() == "*" || op->name() == "**")   return symbolicMul(op);
            else if (op->name() == "/")                         return symbolicDiv(op);  
        }
        else
        {
            FuncNode* func = (FuncNode*)expr; 
            if (func->name() == "exp")              return symbolicExp(func);
            else if (func->name() == "log")         return symbolicLog(func);
            else if (func->name() == "sin")         return symbolicSin(func);
            else if (func->name() == "cos")         return symbolicCos(func);
            else if (func->name() == "tan")         return symbolicTan(func);    
        }                                     
    }

    return expr;
}

} // end