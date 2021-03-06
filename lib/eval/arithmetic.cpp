#include <algorithm>
#include "arithmetic.h"
#include "arithrr.h"
#include "../expr/arithmetic.h"
#include "../expr/polynomial.h"
#include "../math/float-math.h"
#include "../math/integer-math.h"

namespace MathSolver
{

//
//  Numerical arithmetic evaluators
//

ExprNode* numericExp(ExprNode* op)
{
    if (op->children().size() != 1)     
    {
        gErrorManager.log("Arity mismatch: " + toInfixString(op) + " , expected 1 argument", ErrorManager::ERROR, __FILE__, __LINE__); 
        return op;
    }

    Float v = exp((op->children().front()->type() == ExprNode::FLOAT) ? ((FloatNode*)op->children().front())->value() : 
                                                                        Float(((IntNode*)op->children().front())->value().toString()));
    ExprNode* res = new FloatNode(v, op->parent());
    freeExpression(op);
    return res;
}

ExprNode* numericLog(ExprNode* op)
{
    if (op->children().size() != 1 && op->children().size() != 2)     
    {
        gErrorManager.log("Arity mismatch: " + toInfixString(op) + " , expected 1 or 2 arguments", ErrorManager::ERROR, __FILE__, __LINE__); 
        return op;
    }

    if (op->children().size() != 1)    // TODO
    {
        gErrorManager.log("Log of base " + op->children().front()->toString() + " unimplemented", ErrorManager::ERROR, __FILE__, __LINE__); 
        return op;
    }

    // TODO log(x, n)
    Float v = log((op->children().front()->type() == ExprNode::FLOAT) ? ((FloatNode*)op->children().front())->value() : 
                                                                        Float(((IntNode*)op->children().front())->value().toString()));
    ExprNode* res = new FloatNode(v, op->parent());
    freeExpression(op);
    return res;
}

ExprNode* numericSin(ExprNode* op)
{
    if (op->children().size() != 1)     
    {
        gErrorManager.log("Arity mismatch: " + toInfixString(op) + " , expected 1 argument", ErrorManager::ERROR, __FILE__, __LINE__); 
        return op;
    }

    Float v = sin((op->children().front()->type() == ExprNode::FLOAT) ? ((FloatNode*)op->children().front())->value() : 
                                                                        Float(((IntNode*)op->children().front())->value().toString()));
    ExprNode* res = new FloatNode(v, op->parent());
    freeExpression(op);
    return res;
}

ExprNode* numericCos(ExprNode* op)
{
    if (op->children().size() != 1)     
    {
        gErrorManager.log("Arity mismatch: " + toInfixString(op) + " , expected 1 argument", ErrorManager::ERROR, __FILE__, __LINE__); 
        return op;
    }

    Float v = cos((op->children().front()->type() == ExprNode::FLOAT) ? ((FloatNode*)op->children().front())->value() : 
                                                                        Float(((IntNode*)op->children().front())->value().toString()));
    ExprNode* res = new FloatNode(v, op->parent());
    freeExpression(op);
    return res;
}

ExprNode* numericTan(ExprNode* op)
{
    if (op->children().size() != 1)     
    {
        gErrorManager.log("Arity mismatch: " + toInfixString(op) + " , expected 1 argument", ErrorManager::ERROR, __FILE__, __LINE__); 
        return op;
    }

    Float v = tan((op->children().front()->type() == ExprNode::FLOAT) ? ((FloatNode*)op->children().front())->value() : 
                                                                        Float(((IntNode*)op->children().front())->value().toString()));
    ExprNode* res = new FloatNode(v, op->parent());
    freeExpression(op);
    return res;
}

// Evalutes "(-* <num>)"
ExprNode* numericNeg(ExprNode* op)
{
    if (op->children().size() != 1)     
    {
        gErrorManager.log("Arity mismatch: " + toInfixString(op) + " , expected 1 argument", ErrorManager::ERROR, __FILE__, __LINE__); 
        return op;
    }

    if (op->children().front()->type() == ExprNode::INTEGER)
        ((IntNode*)op->children().front())->setValue(-((IntNode*)op->children().front())->value());
    else
        ((FloatNode*)op->children().front())->setValue(-((FloatNode*)op->children().front())->value());

    return moveNode(op, op->children().front());
}

// Evaluates "(+ <num>...)"
ExprNode* numericAdd(ExprNode* op)
{
    ExprNode* res;
    if (std::any_of(op->children().begin(), op->children().end(), [](ExprNode* x) { return x->type() == ExprNode::FLOAT; }))
    {
        for (auto it = op->children().begin(); it != op->children().end(); ++it)
        {
            if ((*it)->type() == ExprNode::INTEGER)
                it = replaceChild(op, new FloatNode(((IntNode*)*it)->value().toString()), it, true); // TODO: naive conversion
        }

        auto it = op->children().begin();
        while (it != op->children().end())
        {
            auto it2 = std::next(it);
            bool match = false;
            while (it2 != op->children().end())
            {
                if (((FloatNode*)*it)->value() == -((FloatNode*)*it2)->value())
                {
                    delete *it;
                    delete *it2;
                    op->children().erase(it2);
                    it = op->children().erase(it);
                    match = true;
                    break;
                }
                else
                {
                    ++it2;
                }
            }

            if (!match) ++it;
        }

        res = new FloatNode();
        for (auto e : op->children())
            ((FloatNode*)res)->value() += ((FloatNode*)e)->value();
    }
    else
    {
        auto it = op->children().begin();
        while (it != op->children().end())
        {
            auto it2 = std::next(it);
            bool match = false;
            while (it2 != op->children().end())
            {
                if (((IntNode*)*it)->value() == -((IntNode*)*it2)->value())
                {
                    delete *it;
                    delete *it2;
                    op->children().erase(it2);
                    it = op->children().erase(it);
                    match = true;
                    break;
                }
                else
                {
                    ++it2;
                }
            }

            if (!match) ++it;
        }

        res = new IntNode();
        for (auto e : op->children())
            ((IntNode*)res)->value() += ((IntNode*)e)->value();
    }
    
    res->setParent(op->parent());
    freeExpression(op);
    return res;
}

// Evaluates "(- <num>...)"
ExprNode* numericSub(ExprNode* op)
{
    ((OpNode*)op)->setName("+");
    for (auto it = std::next(op->children().begin()); it != op->children().end(); ++it) // (- a b c ...) ==> (+ a (-* b) (-* c) ...)
    {
        ExprNode* neg = new OpNode("-*", op);
        neg->children().push_back(*it);
        (*it)->setParent(neg);
        neg = evaluateArithmetic(neg);
        it = replaceChild(op, neg, it);
    }

    return evaluateArithmetic(op);
}

// Evaluates "(* <num>...)" or "(** <num>...)"
ExprNode* numericMul(ExprNode* op)
{
    ExprNode* res;
    if (std::any_of(op->children().begin(), op->children().end(), [](ExprNode* x) { return x->type() == ExprNode::FLOAT; }))
    {
        Float first = ((op->children().front()->type() == ExprNode::FLOAT) ? ((FloatNode*)op->children().front())->value() : 
                                                                             Float(((IntNode*)op->children().front())->value().toString()));
       res = new FloatNode(first, op->parent());
        for (auto e = std::next(op->children().begin()); e != op->children().end(); ++e)
        {
            if ((*e)->type() == ExprNode::FLOAT) ((FloatNode*)res)->value() *= ((FloatNode*)*e)->value();
            else                                 ((FloatNode*)res)->value() *= Float(((IntNode*)*e)->value().toString()); // TODO: this conversion is naive
        }
    }
    else
    {
        res = new IntNode(((IntNode*)op->children().front())->value(), op->parent());
        for (auto e = std::next(op->children().begin()); e != op->children().end(); ++e)
            ((IntNode*)res)->value() *= ((IntNode*)*e)->value();
    }

    res->setParent(op->parent());
    freeExpression(op);
    return res;
}

// Evaluates "(/ <num> <num>)"
ExprNode* numericDiv(ExprNode* op)
{
    if (op->children().size() != 2)     
    {
        gErrorManager.log("Arity mismatch: " + toInfixString(op) + " , expected 2 arguments", ErrorManager::ERROR, __FILE__, __LINE__); 
        return op;
    }

    ExprNode* lhs = op->children().front();
    ExprNode* rhs = op->children().back();

    if (isZeroNode(rhs)) // (/ x 0) ==> undef
    {
        ExprNode* ret = new ConstNode("undef", op->parent());
        gErrorManager.log("Division by zero: " + toInfixString(op), ErrorManager::WARNING);      
        freeExpression(op);
        return ret;
    }

    if (lhs->type() == ExprNode::INTEGER && rhs->type() == ExprNode::INTEGER) // <exact> / <exact>
    {   
        Integer factor = gcd(((IntNode*)lhs)->value(), ((IntNode*)rhs)->value());
        if (factor > Integer(1))
        {
            ((IntNode*)lhs)->value() /= factor;
            ((IntNode*)rhs)->value() /= factor;
        }

        if (((IntNode*)rhs)->value() == Integer(1))
        {
            delete rhs;
            return moveNode(op, lhs);
        }

        return op; // no exact numerical division: a/b --> a/b
    }
    else
    {
        Float n = ((lhs->type() == ExprNode::FLOAT) ? ((FloatNode*)lhs)->value() : Float(((IntNode*)lhs)->value().toString()));
        Float d = ((rhs->type() == ExprNode::FLOAT) ? ((FloatNode*)rhs)->value() : Float(((IntNode*)rhs)->value().toString()));
        ExprNode* res = new FloatNode(n / d, op->parent());
        freeExpression(op);
        return res;
    }
}

// Evaluates (% <num> <num>)
ExprNode* numericMod(ExprNode* op)
{
    if (op->children().size() != 2)     
    {
        gErrorManager.log("Arity mismatch: " + toInfixString(op) + " , expected 2 arguments", ErrorManager::ERROR, __FILE__, __LINE__); 
        return op;
    } 

    ExprNode* lhs = op->children().front();
    ExprNode* rhs = op->children().back();
    ExprNode* res;
    if (lhs->type() == ExprNode::INTEGER && rhs->type() == ExprNode::INTEGER) // <exact> mod <exact>
    {
        res = new IntNode(((IntNode*)op->children().front())->value() % ((IntNode*)op->children().back())->value(), op->parent());
    }
    else
    {
        Float n = ((lhs->type() == ExprNode::FLOAT) ? ((FloatNode*)lhs)->value() : Float(((IntNode*)lhs)->value().toString()));
        Float d = ((rhs->type() == ExprNode::FLOAT) ? ((FloatNode*)rhs)->value() : Float(((IntNode*)rhs)->value().toString()));
        res = new FloatNode(mod(n, d), op->parent());
    }
    
    freeExpression(op);
    return res;
}

// Evaluates (^ <num> <num>)
ExprNode* numericPow(ExprNode* op)
{
    if (op->children().size() != 2)     
    {
        gErrorManager.log("Arity mismatch: " + toInfixString(op) + " , expected 2 arguments", ErrorManager::ERROR, __FILE__, __LINE__); 
        return op;
    }

    ExprNode* lhs = op->children().front();
    ExprNode* rhs = op->children().back();
    if (lhs->type() == ExprNode::INTEGER && rhs->type() == ExprNode::INTEGER)
    {
        if (((IntNode*)rhs)->value().sign())  // (^ x -n)
        {
            ((OpNode*)op)->setName("/");
            ((IntNode*)rhs)->setValue(pow(((IntNode*)lhs)->value(), -((IntNode*)rhs)->value()));
            ((IntNode*)lhs)->setValue(Integer(1));
            return op;                                 
        }
        else
        {
            ExprNode* res = new IntNode(pow(((IntNode*)op->children().front())->value(), ((IntNode*)op->children().back())->value()), op->parent());
            freeExpression(op);
            return res;
        }
    }
    else
    {
        Float x = ((lhs->type() == ExprNode::FLOAT) ? ((FloatNode*)lhs)->value() : Float(((IntNode*)lhs)->value().toString()));
        Float y = ((rhs->type() == ExprNode::FLOAT) ? ((FloatNode*)rhs)->value() : Float(((IntNode*)rhs)->value().toString()));
        ExprNode* res = new FloatNode(pow(x, y), op->parent());
        freeExpression(op);
        return res;
    }
}

// Evaluates (! <num>)
ExprNode* numericFact(ExprNode* op)
{
    if (op->children().size() != 1)     
    {
        gErrorManager.log("Arity mismatch: " + toInfixString(op) + " , expected 2 arguments", ErrorManager::ERROR, __FILE__, __LINE__); 
        return op;
    }

    if (op->children().front()->type() == ExprNode::FLOAT)
    {
        gErrorManager.log("Factorial is defined for integers only. Try the gamma function.", ErrorManager::ERROR, __FILE__, __LINE__);
        return op;
    }

    ExprNode* res = new IntNode(fact(((IntNode*)op->children().front())->value().toInt()), op->parent());
    freeExpression(op);

    if (gErrorManager.hasError())
        return moveNode(res, new ConstNode("undef"));
    return res;
}

//
//  Symbolic arithmetic evaluators
//

// Evaluates "(exp x)"
ExprNode* symbolicExp(ExprNode* op)
{
    if (op->children().size() != 1)     
    {
        gErrorManager.log("Arity mismatch: " + toInfixString(op) + " , expected 1 argument", ErrorManager::ERROR, __FILE__, __LINE__); 
        return op;
    }

    // TODO: more simplifications
    return op;
}

// Evaluates "(log x) or (log b x)"
ExprNode* symbolicLog(ExprNode* op)
{
    if (op->children().size() != 1 && op->children().size() != 2)     
    {
        gErrorManager.log("Arity mismatch: " + toInfixString(op) + " , expected 1 or 2 arguments", ErrorManager::ERROR, __FILE__, __LINE__); 
        return op;
    }

    // TODO: more simplifications

    return op;
}

// Evaluates "(sin x)"
ExprNode* symbolicSin(ExprNode* op)
{
    if (op->children().size() != 1)     
    {
        gErrorManager.log("Arity mismatch: " + toInfixString(op) + " , expected 1 argument", ErrorManager::ERROR, __FILE__, __LINE__); 
        return op;
    }

    // TODO: more simplifications

    return op;
}

// Evaluates "(cos x)"
ExprNode* symbolicCos(ExprNode* op)
{
    if (op->children().size() != 1)     
    {
        gErrorManager.log("Arity mismatch: " + toInfixString(op) + " , expected 1 argument", ErrorManager::ERROR, __FILE__, __LINE__); 
        return op;
    }

    // TODO: more simplifications

    return op;
}

// Evaluates "(tan x)"
ExprNode* symbolicTan(ExprNode* op)
{
    if (op->children().size() != 1)     
    {
        gErrorManager.log("Arity mismatch: " + toInfixString(op) + " , expected 1 argument", ErrorManager::ERROR, __FILE__, __LINE__); 
        return op;
    }

    // TODO: more simplifications

    return op;
}

// Shared evaluator for symbolic + and -
ExprNode* symbolicAddSub(ExprNode* op, const char* str)
{
    auto i = op->children().begin();
    while (std::next(i) != op->children().end())
    {       
        auto j = std::next(i);

        // (- a b c ...) for any pair excluding a ==> (- a (+ b c ...)) is actually addition
        if (i != op->children().begin()) str = "+";

        if ((*i)->isNumber() && (*j)->isNumber()) // simplify with temporary expression (+- i j)
        {
            ExprNode* tmp = new OpNode(((OpNode*)op)->name(), op);
            tmp->children().push_back(*i);
            tmp->children().push_back(*j);
            if (std::string(str) == "+")        tmp = numericAdd(tmp);
            else /* name == "-" */              tmp = numericSub(tmp);
            i = op->children().erase(i, std::next(j));
            i = op->children().insert(i, tmp); 
        }
        else
        {
            bool added = false;
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
                
                if (coeff_lhs.size() == 1)
                {
                    lhs = coeff_lhs.front();
                }
                else
                {
                    lhs = new OpNode("**");
                    lhs->children() = coeff_lhs;
                    for (ExprNode* child : lhs->children())
                        child->setParent(lhs);
                    lhs = evaluateArithmetic(lhs);
                }

                if (coeff_rhs.size() == 1)
                {
                    rhs = coeff_rhs.front();
                }
                else
                {
                    rhs = new OpNode("**");
                    rhs->children() = coeff_rhs;
                    for (ExprNode* child : rhs->children())
                        child->setParent(rhs);
                    rhs = evaluateArithmetic(rhs);
                }

                if (co->children().size() == 1) // correct: (* x) => x
                {
                    co = moveNode(co, co->children().front());
                    common.pop_front();
                    common.push_front(co);
                }

                removeTerm(*i, common);
                removeTerm(*j, common);

                ExprNode* add = new OpNode(str); // (coeff_a +- coeff_b)
                add->children().push_back(lhs);
                add->children().push_back(rhs);
                lhs->setParent(add);
                rhs->setParent(add);
                add = evaluateArithmetic(add); // simplify the coefficients

                ExprNode* mul = new OpNode("**"); // coeff * common

                if (co->isNumber() || co->type() == ExprNode::CONSTANT || // ordering
                    containsType(add, ExprNode::FUNCTION))
                {
                    mul->children().push_back(co);
                    mul->children().push_back(add);
                }
                else
                {
                    mul->children().push_back(add);
                    mul->children().push_back(co);
                }
                
                add->setParent(mul);
                co->setParent(mul);
                mul = evaluateArithmetic(mul);

                delete *i; // delete *i and *j
                delete *j;
                op->children().insert(i, mul);
                op->children().erase(i, std::next(j));
                added = true;
            }

            i = (added) ? op->children().begin() : std::next(i);
        }
    }
    
    if (op->children().size() == 1) // (+- (...)) ==> (...)
        return moveNode(op, op->children().front());

    flattenExpr(op);
    return op;
}

// Evalutes "(+ <arg0> <arg1> ... )"
ExprNode* symbolicAdd(ExprNode* op)
{
    if (op->children().size() < 2)     
    {
        gErrorManager.log("Arity mismatch: " + toInfixString(op) + " , expected at least 2 arguments", ErrorManager::ERROR, __FILE__, __LINE__); 
        return op;
    }

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
    if (op->children().size() < 2)     
    {
        gErrorManager.log("Arity mismatch: " + toInfixString(op) + " , expected at least 2 arguments", ErrorManager::ERROR, __FILE__, __LINE__); 
        return op;
    }
    
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
    if (op->children().size() < 2)     
    {
        gErrorManager.log("Arity mismatch: " + toInfixString(op) + " , expected at least 2 arguments", ErrorManager::ERROR, __FILE__, __LINE__); 
        return op;
    }

    // reorder monomial
    if (isMonomial(op))    op = reorderMonomial(op);

    for (auto it = op->children().begin(); it != op->children().end() && std::next(it) != op->children().end(); ++it)
    {
        auto it2 = std::next(it);
        while (it2 != op->children().end())
        {
            if (eqvExpr(peekPowBase(*it), peekPowBase(*it2))) // (* (^ a n) ... (^ a m)) ==> (* (^ a (+ n m)) ... )
            {
                ExprNode* pow = new OpNode("^", op);
                ExprNode* add = new OpNode("+", pow);  

                add->children().push_back(extractPowExp(*it));
                add->children().push_back(extractPowExp(*it2));
                add = evaluateArithmetic(add);

                pow->children().push_back(extractPowBase(*it));
                pow->children().push_back(add);
                pow = evaluateArithmetic(pow);

                freeExpression(*it);
                freeExpression(*it2);
                it2 = op->children().erase(it2);
                it = replaceChild(op, pow, it);              
            }  
            else
            {
                ++it2;
            } 
        }
    }

    // initial pass for like-terms 
    auto it = op->children().begin();
    while (it != op->children().end())
    {
        auto it2 = std::next(it);
        if (isZeroNode(*it)) // (* 0 a ...) ==> 0
        {
            for (auto c : op->children()) freeExpression(c);
            return moveNode(op, new IntNode());
        }
        else if (isIdentityNode(*it)) // (* 1 a ...) ==> (* a ...)
        {
            delete *it;
            it = op->children().erase(it);
        }
        else if (((*it)->type() == ExprNode::INTEGER && ((IntNode*)*it)->value() == Integer(-1)) || // (* -1 a ...) ==> (-* (* a ...))
                 ((*it)->type() == ExprNode::FLOAT && ((FloatNode*)*it)->value() == Float("-1.0")))
        {
            if (op->children().size() == 2) // specific: (* -1 a) ==> (-* a)
            {
                ((OpNode*)op)->setName("-*");
                delete *it;
                op->children().erase(it);
                return op;
            }
            else  // general
            {
                ExprNode* neg = new OpNode("-*", op->parent());
                delete *it;
                op->children().erase(it);
                op->setParent(neg);
                neg->children().push_back(op);
                return neg;
            }          
        }

        if (it2 == op->children().end())
            break;

        // second pass (requires two operands)
        if ((*it)->isOperator() &&     // (* a... (* b... ) c...) ==> (* a... b... c...)
           (((OpNode*)*it)->name() == "*" || ((OpNode*)*it)->name() == "**"))
        {        
            op->children().insert(it, (*it)->children().begin(), (*it)->children().end());
            delete *it;
            op->children().erase(it);
            it = it2;
        }   
        else if ((*it)->isOperator() && (*it2)->isOperator() && // (* (/ a b) (/ c d) ...) ==> (* (/ (* a c) (* b d)) ...)
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

    if (op->children().size() == 1) // correct: (* x) => x
        return moveNode(op, op->children().front());
    return op;
}

// Evalutes "(/ <arg0> <arg1>)"
ExprNode* symbolicDiv(OpNode* op)
{
    if (op->children().size() != 2)     
    {
        gErrorManager.log("Arity mismatch: " + toInfixString(op) + " , expected 2 arguments", ErrorManager::ERROR, __FILE__, __LINE__); 
        return op;
    }

    ExprNode* num = op->children().front();
    ExprNode* den = op->children().back();

    if (isZeroNode(den)) // (/ x 0) ==> undef
    {
        ExprNode* ret = new ConstNode("undef", op->parent());   
        freeExpression(op);
        return ret;
    }

    if (isIdentityNode(den)) // (/ x 1) ==> x
    {
        num->setParent(op->parent());
        op->children().pop_front();      
        freeExpression(op);
        return num;
    }

    if (eqvExpr(num, den)) // (/ x x) ==> 1
    {
        ExprNode* ret = new IntNode(1, op->parent());
        freeExpression(op);
        return ret;
    }
    
    if (num->isOperator() && den->isOperator() &&       // (/ (^ x n) (^ x m)) ==> (^ x (- n m))
        ((OpNode*)num)->name() == "^" && ((OpNode*)den)->name() == "^" && 
        eqvExpr(peekPowBase(num), peekPowBase(den)))    
    {
        ExprNode* sub = new OpNode("-", num);
        sub->children().push_back(num->children().back());
        sub->children().push_back(den->children().back());
        num->children().back()->setParent(sub);
        den->children().back()->setParent(sub);
        num->children().pop_back();
        den->children().pop_back();
        sub = evaluateArithmetic(sub);

        num->children().push_back(sub);
        freeExpression(den);
        return moveNode(op, num);
    }

    if (num->isOperator() && ((OpNode*)num)->name() == "^" && den->isValue() && // (/ (^ x n) x) ==> (^ x (- n 1))
        eqvExpr(peekPowBase(num), peekPowBase(den)))    
    {
        ExprNode* sub = new OpNode("-", num);
        sub->children().push_back(num->children().back());
        sub->children().push_back(new IntNode(1, sub));
        num->children().back()->setParent(sub);
        num->children().pop_back();
        sub = evaluateArithmetic(sub);

        num->children().push_back(sub);
        freeExpression(den);
        return moveNode(op, num);
    }
    
    if (num->isValue() && den->isOperator() && ((OpNode*)den)->name() == "^" && // (/ x (^ x n)) ==> (/ 1 (^ x (- n 1))
        eqvExpr(peekPowBase(num), peekPowBase(den)))    
    {
        ExprNode* sub = new OpNode("-", den);
        sub->children().push_back(den->children().back());
        sub->children().push_back(new IntNode(1, sub));
        den->children().back()->setParent(sub);
        den->children().pop_back();
        sub = evaluateArithmetic(sub);

        den->children().push_back(sub);
        freeExpression(num);
        op->children().pop_front();
        op->children().push_front(new IntNode(1, op));
        return op;
    }
    
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
                if (eqvExpr(peekPowBase(*it), peekPowBase(*it2)))
                {
                    ExprNode* sub = new OpNode("-");
                    sub->children().push_back(extractPowExp(*it));
                    sub->children().push_back(extractPowExp(*it2));
                    sub = evaluateArithmetic(sub);

                    if (sub->type() == ExprNode::INTEGER && ((IntNode*)sub)->value().isZero())
                    {
                        freeExpression(*it);
                        freeExpression(*it2);
                        num->children().erase(it--);
                        den->children().erase(it2);
                        freeExpression(sub);
                    }
                    else if ((sub->isOperator() && ((OpNode*)sub)->name() == "-*") ||
                             (sub->type() == ExprNode::INTEGER && ((IntNode*)sub)->value().sign()))
                    {
                        ExprNode* pow = new OpNode("^", den);
                        ExprNode* neg = new OpNode("-*", pow);

                        neg->children().push_back(sub);
                        sub->setParent(neg);
                        neg = evaluateArithmetic(neg);

                        pow->children().push_back(extractPowBase(*it));
                        pow->children().push_back(neg);
                        pow = evaluateArithmetic(pow);

                        freeExpression(*it);
                        freeExpression(*it2);
                        num->children().erase(it--);
                        replaceChild(den, pow, it2);   
                    }
                    else if (sub->isOperator() || sub->type() == ExprNode::INTEGER)
                    {
                        ExprNode* pow = new OpNode("^", den);
                        sub->setParent(pow);
                        pow->children().push_back(extractPowBase(*it));
                        pow->children().push_back(sub);
                        pow = evaluateArithmetic(pow);

                        freeExpression(*it);
                        freeExpression(*it2);
                        it = replaceChild(num, pow, it);
                        den->children().erase(it2);
                    }
                    else // TODO: remove once this operation is sound
                    { 
                        gErrorManager.log("Should not have reached this point", ErrorManager::FATAL, __FILE__, __LINE__);
                        freeExpression(sub);
                        return op;
                    }
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
            if (eqvExpr(peekPowBase(*it), peekPowBase(den)))
            {
                ExprNode* pow = new OpNode("^", num);
                ExprNode* sub = new OpNode("-", pow);

                sub->children().push_back(extractPowExp(*it));
                sub->children().push_back(extractPowExp(den));
                sub = evaluateArithmetic(sub);

                pow->children().push_back(extractPowBase(*it));
                pow->children().push_back(sub);
                pow = evaluateArithmetic(pow);

                freeExpression(*it);
                freeExpression(den);
                it = replaceChild(num, pow, it);
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
            if (eqvExpr(peekPowBase(num), peekPowBase(*it)))
            {
                ExprNode* pow = new OpNode("^", den);
                ExprNode* sub = new OpNode("-", pow);

                sub->children().push_back(extractPowExp(*it));
                sub->children().push_back(extractPowExp(num));
                sub = evaluateArithmetic(sub);
  
                pow->children().push_back(extractPowBase(*it));
                pow->children().push_back(sub);
                pow = evaluateArithmetic(pow);

                freeExpression(num);
                freeExpression(*it);
                num = new IntNode(Integer(1), op);
                it = replaceChild(den, pow, it);
                den = evaluateArithmetic(den); // simplify denominator
                replaceChild(op, num, op->children().begin());
                replaceChild(op, den, std::next(op->children().begin()));
                break;
            }
        }
    }
    else if (eqvExpr(num, den)) // (/ a a) ==> 1
    {
        freeExpression(num);
        freeExpression(den);
        return moveNode(op, new IntNode(Integer(1)));
    } 
    else if (op->children().back()->type() == ExprNode::INTEGER && ((IntNode*)op->children().back())->value() == Integer(1)) // (/ x 1) ==> x
    {
        delete op->children().back();
        op->children().clear();
        return moveNode(op, op->children().front());
    }  // else do nothing

    if (((OpNode*)op)->name() == "/" && op->children().size() == 1)
        return moveNode(op, op->children().front()); 
    return op;
}

// Evalutes "(% <arg0> <arg1>)"
ExprNode* symbolicMod(ExprNode* op)
{
    if (op->children().size() != 2)     
    {
        gErrorManager.log("Arity mismatch: " + toInfixString(op) + " , expected 2 arguments", ErrorManager::ERROR, __FILE__, __LINE__); 
        return op;
    }

    if (op->children().front()->isOperator() &&   // (% (% x n) n) ==> (% x n)
        (((OpNode*)op->children().front())->name() == "%" || ((OpNode*)op->children().front())->name() == "mod") && 
        eqvExpr(op->children().front()->children().back(), op->children().back()))
    {
        delete op->children().back();
        return moveNode(op, op->children().front());
    }
    else if (op->children().front()->isOperator() && ((OpNode*)op->children().front())->name() == "^" && // (% (^ n x) n) ==> 0, where x ∈ N
             eqvExpr(op->children().front()->children().front(), op->children().back()) &&
             op->children().front()->children().back()->type() == ExprNode::INTEGER && 
             ((IntNode*)op->children().front()->children().back())->value() > Integer(0))
    {
        ExprNode* res = new IntNode(0, op->parent());
        freeExpression(op);
        return res;
    }
    else if (op->children().front()->isOperator() && (((OpNode*)op->children().front())->name() == "+" ||               // (% (+ (% a n) (% b n)) n) ==> (% (+ a b) n)
            ((OpNode*)op->children().front())->name() == "*" || ((OpNode*)op->children().front())->name() == "**") &&   // (% (* (% a n) (% b n)) n) ==> (% (* a b) n)
             op->children().front()->children().front()->isOperator() && (((OpNode*)op->children().front()->children().front())->name() == "%" ||
             ((OpNode*)op->children().front()->children().front())->name() == "mod") && 
             op->children().front()->children().back()->isOperator() && (((OpNode*)op->children().front()->children().back())->name() == "%" ||
             ((OpNode*)op->children().front()->children().back())->name() == "mod") &&
             eqvExpr(op->children().front()->children().front()->children().back(), op->children().front()->children().back()->children().back()))
    {
        ExprNode* lhs = op->children().front()->children().front()->children().front();
        ExprNode* rhs = op->children().front()->children().back()->children().front();
        op->children().front()->children().front()->children().pop_front();
        op->children().front()->children().back()->children().pop_front();
        freeExpression(op->children().front()->children().front());
        freeExpression(op->children().front()->children().back());
        replaceChild(op->children().front(), lhs, op->children().front()->children().begin());
        replaceChild(op->children().front(), rhs, std::next(op->children().front()->children().begin()));
    }

    return op;
}

// Evalutes "(^ <arg0> <arg1>)"
ExprNode* symbolicPow(ExprNode* op)
{
    if (op->children().size() != 2)     
    {
        gErrorManager.log("Arity mismatch: " + toInfixString(op) + " , expected 2 arguments", ErrorManager::ERROR, __FILE__, __LINE__); 
        return op;
    }

    ExprNode* base = op->children().front();
    ExprNode* ex = op->children().back();

    if (isZeroNode(ex)) // (^ x 0) ==> 1
    {
        ExprNode* ret = new IntNode(1, op->parent());
        freeExpression(op);
        return ret;
    }
    else if (isIdentityNode(ex))// (^ x 1) ==> x
    {
        base->setParent(op->parent());
        op->children().pop_front();      
        freeExpression(op);
        op = base;
    }
    else if (base->isOperator() && ((OpNode*)base)->name() == "^") // (^ (^ x n) m) ==> (^ x (* n m))
    {
        ExprNode* base2 = base->children().front();
        ExprNode* mul = new OpNode("**", op);

        mul->children().push_back(base->children().back());
        mul->children().push_back(ex);
        mul->children().front()->setParent(mul);
        mul->children().back()->setParent(mul);
        mul = evaluateArithmetic(mul);

        base->children().clear();
        delete base;
        
        op->children().clear();
        op->children().push_back(base2);
        op->children().push_back(mul);
        base2->setParent(op);
    }

    return op;
}

//
// Arithmetic evaluator
//

ExprNode* evaluateArithmetic(ExprNode* expr, bool firstPass)
{
    if (expr->isNumber())                         return expr;
    if (expr->type() == ExprNode::CONSTANT)       return expr;    // TODO: constant table
    if (expr->type() == ExprNode::VARIABLE)       return expr;
    
    if (expr->type() == ExprNode::OPERATOR)
    {
        OpNode* op = (OpNode*)expr;                  
        if (isNumerical(expr))
        {
            if (op->name() == "-*")                             return numericNeg(op);
            else if (op->name() == "+")                         return numericAdd(op);
            else if (op->name() == "-")                         return numericSub(op);
            else if (op->name() == "*" || op->name() == "**")   return numericMul(op);
            else if (op->name() == "/")                         return numericDiv(op);
            else if (op->name() == "%" || op->name() == "mod")  return numericMod(op);
            else if (op->name() == "^")                         return numericPow(op);
            else if (op->name() == "!")                         return numericFact(op);
        }
        else
        {
            if (op->name() == "-*")                             return expr;    // handles in 'rewrite'
            else if (op->name() == "+")                         return symbolicAdd(op);
            else if (op->name() == "-")                         return symbolicSub(op);
            else if (op->name() == "*" || op->name() == "**")   return symbolicMul(op);
            else if (op->name() == "/")                         return symbolicDiv(op);
            else if (op->name() == "%" || op->name() == "mod")  return symbolicMod(op);
            else if (op->name() == "^")                         return symbolicPow(op);
            else if (op->name() == "!")                         return expr;    // No simplification needed?
        }     
    }
    else
    {
        FuncNode* func = (FuncNode*)expr; 
        if (!firstPass && isNumerical(expr))
        {
            if (func->name() == "exp")              return numericExp(expr);
            else if (func->name() == "log")         return numericExp(expr);
            else if (func->name() == "sin")         return numericSin(expr);
            else if (func->name() == "cos")         return numericCos(expr);
            else if (func->name() == "tan")         return numericTan(expr);
        }
        else
        {
            if (func->name() == "exp")              return symbolicExp(func);
            else if (func->name() == "log")         return symbolicLog(func);
            else if (func->name() == "sin")         return symbolicSin(func);
            else if (func->name() == "cos")         return symbolicCos(func);
            else if (func->name() == "tan")         return symbolicTan(func); 
        }
    }

    gErrorManager.log("Unimpemented operation: " + toInfixString(expr), ErrorManager::ERROR, __FILE__, __LINE__);
    return expr;
}

} // end