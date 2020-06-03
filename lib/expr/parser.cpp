#include <iostream>
#include "parser.h"

namespace MathSolver
{

// See header for description.
std::list<ExprNode*> tokenizeStr(const std::string& expr)
{
    std::list<ExprNode*> tokens;
    std::list<std::string> brackets;
    size_t len = expr.length();
    size_t itr = 0;

    while (itr != len)
    {
        if (isspace(expr[itr]))
        {
            ++itr;
        }
        else if (isdigit(expr[itr]) ||        // <digit> OR <dot><digit> 
                (expr[itr] == '.' && itr < (len - 1) && isdigit(expr[itr + 1])))
        {
            ExprNode* node;
            size_t i = itr + 1;
            for (; i != len && (isdigit(expr[i]) || expr[i] == '.'); ++i);

            std::string str = expr.substr(itr, i - itr);
            if (str.find('.') == std::string::npos)     node = new IntNode(Integer(str));
            else                                        node = new FloatNode(Float(str));
            tokens.push_back(node);
            itr = i;
        }
        else if (isalpha(expr[itr]))
        {
            size_t i = itr + 1;
            for (; i != len && isalpha(expr[i]); ++i);

            std::string name = expr.substr(itr, i - itr);
            ExprNode* node;

            if (name == "mod")          node = new OpNode("mod");     // special case
            else if (isFunction(name))  node = new FuncNode(name);
            else                        node = new VarNode(name);
            tokens.push_back(node);
            itr = i;
        }
        else if (expr[itr] == ',') // TODO: syntax nodes other than brackets
        {
            tokens.push_back(new SyntaxNode(std::string(1, expr[itr])));
            ++itr;
        }
        else if (isBracket(expr[itr]))
        {
            if (expr[itr] == '(' || expr[itr] == '{' || expr[itr] == '[')
            {
                brackets.push_front(std::string(1, expr[itr]));
            }
            else
            {
                if (brackets.empty())
                {
                    SyntaxNode* rest = new SyntaxNode(expr.substr(itr, len - itr));
                    tokens.push_back(rest);
                    gErrorManager.log("Unexpected bracket: \"" + expr.substr(itr, 1)  + "\" Rest=\"" + rest->name() + "\"", ErrorManager::ERROR);
                    return tokens;
                }

                std::string match = brackets.front();
                brackets.pop_front();
                if ((expr[itr] == ')' && match != "(") || (expr[itr] == '}' && match != "{") || (expr[itr] == ']' && match != "["))
                {
                    SyntaxNode* rest = new SyntaxNode(expr.substr(itr, len - itr));
                    tokens.push_back(rest);
                    gErrorManager.log("Wrong closing bracket: \"" + expr.substr(itr, 1)  + "\" Rest=\"" + rest->name() + "\"", ErrorManager::ERROR);
                    return tokens;
                }
            }
            
            ExprNode* bracket = new SyntaxNode(std::string(1, expr[itr]));
            tokens.push_back(bracket);
            ++itr;
        }
        else if (isOperator(expr[itr]))
        {
            OpNode* op = new OpNode();
            std::string name = expr.substr(itr, 1);
            size_t i = itr + 1;

            while (i != len && isOperator(name + expr[i]))
                name += expr[i++];
            op->setName(name);
            tokens.push_back(op);
            itr = i;
        }
        else
        {
            gErrorManager.log("Unknown character: \"" + expr.substr(itr, 1) + "\"", ErrorManager::ERROR);
            return tokens;
        }
        
    }

    if (!brackets.empty())
    {
        gErrorManager.log("Mismatched brackets, Attempted to fix", ErrorManager::MESSAGE);
        for (auto e : brackets)
        {
            if (e == "(")         tokens.push_back(new SyntaxNode(")"));
            else if (e == "[")    tokens.push_back(new SyntaxNode("]"));
            else /* e == "{" */   tokens.push_back(new SyntaxNode("}"));
        }
    
        brackets.clear();
    }

    expandTokens(tokens);
    return tokens;
}

/* 
  Token expansion

  Negative
    <begin><negative><not operator>   -5 -pi -sin(x) -(x)
    <operator><negative><not operator> (-5) x+-5
    
  Implicit multiplication
   <number><constant>           3pi
   <number><function>           3sin(x)
   <number><variable>           3x
   <number><bracket>            3(...)
   <constant><function>         bsin(x)
   <constant><variable>         bx
   <constant><bracket>          b(...)
   <variable><function>          xsin(x)
   <variable><bracket>          x(...)
   <factorial><not operator>    5!x, 5!2, 5!sin(x), 5!2!
   <bracket><bracket>           (2)(3)
*/

void expandTokens(std::list<ExprNode*>& tokens)
{
    for (auto it = tokens.begin(); it != tokens.end(); ++it)
    {
        auto next = std::next(it);
        if (it == tokens.begin() && next != tokens.end() && (*it)->isOperator() && ((OpNode*)*it)->name() == "-" &&  // beginning negative
            (!(*next)->isOperator() || ((*next)->isSyntax() && ((SyntaxNode*)*next)->name() == "(")))
        {
            ((OpNode*)*it)->setName("-*");
        }
        else if (next != tokens.end() && ((*it)->isOperator() || ((*it)->isSyntax() && ((SyntaxNode*)*it)->name() == "(")) &&
                (*next)->isOperator() && ((OpNode*)*next)->name() == "-")
        {
            ((OpNode*)*next)->setName("-*");
        }
        else if (next != tokens.end() && // implicit multiplication
            (((*it)->isNumber() && ((*next)->type() == ExprNode::VARIABLE || (*next)->type() == ExprNode::FUNCTION ||
                (*next)->type() == ExprNode::CONSTANT || ((*next)->isSyntax() && ((SyntaxNode*)*next)->name() == "("))) ||
             ((*it)->type() == ExprNode::CONSTANT && ((*next)->type() == ExprNode::VARIABLE || // constant
                (*next)->type() == ExprNode::FUNCTION || ((*next)->isSyntax() && ((SyntaxNode*)*next)->name() == "("))) ||
             ((*it)->type() == ExprNode::VARIABLE && ((*next)->type() == ExprNode::FUNCTION || // variable
                ((*next)->isSyntax() && ((SyntaxNode*)*next)->name() == "("))) ||
             ((*it)->isOperator() && ((OpNode*)*it)->name() == "!" && !(*next)->isOperator() && !(*next)->isSyntax()) || // factorial
             ((*it)->isSyntax() && ((SyntaxNode*)*it)->name() == ")" && (*next)->isSyntax() && ((SyntaxNode*)*next)->name() == "("))) // factorial
        {
            it = tokens.insert(next, new OpNode("**"));
        }
    }
}

// parseTokens(tokens) and related functions

bool bracketedExpr(std::list<ExprNode*>::const_iterator begin, std::list<ExprNode*>::const_iterator end)
{
    if (!((*begin)->isSyntax() && ((SyntaxNode*)*begin)->name() == "(" && (*end)->isSyntax() && ((SyntaxNode*)*end)->name() == ")") && 
        !((*begin)->isSyntax() && ((SyntaxNode*)*begin)->name() == "[" && (*end)->isSyntax() && ((SyntaxNode*)*end)->name() == "]")  &&
        !((*begin)->isSyntax() && ((SyntaxNode*)*begin)->name() == "{" && (*end)->isSyntax() && ((SyntaxNode*)*end)->name() == "}"))
        return false;

    size_t bracketLevel = 0;
    for (auto it = begin; it != end; ++it)
    {
        if ((*it)->isSyntax() && (((SyntaxNode*)*it)->name() == "(" || ((SyntaxNode*)*it)->name() == "[" || ((SyntaxNode*)*it)->name() == "{"))
            ++bracketLevel;
        else if ((*it)->isSyntax() && (((SyntaxNode*)*it)->name() == ")" || ((SyntaxNode*)*it)->name() == "]" || ((SyntaxNode*)*it)->name() == "}"))
        {
            if (bracketLevel <= 1 && it != end)
                return false;
        } 
    }

    return true;
}

ExprNode* parseTokensR(std::list<ExprNode*>::const_iterator begin, std::list<ExprNode*>::const_iterator end)
{
    if (bracketedExpr(begin, end))
    {
        ExprNode* ret = parseTokensR(std::next(begin),std::prev(end));
        delete *begin;
        delete *end;
        return ret;
    }

    auto split = end; // loop through tokens from end to beginning
    auto rend = std::prev(begin);
    size_t bracketLevel = 0;
    for (auto it = end; it != rend; --it)
    {
        if ((*it)->isSyntax() && (((SyntaxNode*)*it)->name() == ")" || ((SyntaxNode*)*it)->name() == "]" || ((SyntaxNode*)*it)->name() == "}"))
        {
            ++bracketLevel;
        }
        else if ((*it)->isSyntax() && (((SyntaxNode*)*it)->name() == "(" || ((SyntaxNode*)*it)->name() == "[" || ((SyntaxNode*)*it)->name() == "{"))
        {
            --bracketLevel;
        }
        else if (bracketLevel == 0) // find lowest precision when not within the bracket
        {
            if ((*it)->prec() > opPrec("^"))
            {
                if((*it)->prec() > (*split)->prec())
                    split = it;
            }
            else if ((*it)->prec() > 0)
            {
                if((*it)->prec() >= (*split)->prec())
                    split = it;
            }
        }
    }

    ExprNode* node = *split;
    auto next = std::next(split);
    auto prev = std::prev(split);
    if (node->type() == ExprNode::FUNCTION)
    {
        FuncNode* func = (FuncNode*)node;
        if (split == end) // TODO: arity mismatch
        {
            gErrorManager.log("Expected \"<func> (<args>...)\" for: " + func->name(), ErrorManager::ERROR);
            return nullptr;
        }

        if (!((*next)->isSyntax() && ((SyntaxNode*)*next)->name() == "(")) // func <arg>"
        {
            ExprNode* arg = parseTokensR(next, end);
            arg->setParent(node);
            node->children().push_back(arg);
        }
        else // func (<arg>, <arg>, ...)
        {
            auto it = next;
            while (!(*it)->isSyntax() || ((SyntaxNode*)*it)->name() != ")") // iterate forward now
            {
                auto it2 = std::next(it);
                bracketLevel = 0;
                while (it2 != end && (bracketLevel > 0 || !(*it2)->isSyntax() || !(((SyntaxNode*)*it2)->name() == ")" || ((SyntaxNode*)*it2)->name() == ",")))
                {
                    if ((*it2)->isSyntax() && ((SyntaxNode*)*it2)->name() == "(")         ++bracketLevel;
                    else if ((*it2)->isSyntax() && ((SyntaxNode*)*it2)->name() == ")")    --bracketLevel;
                    ++it2; // split arg vector
                }

                ExprNode* arg = parseTokensR(std::next(it), std::prev(it2));
                arg->setParent(node);
                node->children().push_back(arg);
                delete *it;
                it = it2; 
            }
            delete *it;
        }        
    }
    else if (node->isOperator())
    {
        OpNode* op = (OpNode*)node;
        if (op->name() == "+" || op->name() == "-" || op->name() == "**" || op->name() == "*" ||
            op->name() == "/" || op->name() == "%" || op->name() == "mod" || op->name() == "^" ||
            op->name() == "<" || op->name() == ">" || op->name() == "<=" || op->name() == ">=" || op->name() == "=") 
        {         
            if (split == begin || split == end) // arity mismatch
            {
                gErrorManager.log("Expected \"<lhs> " + op->name() + " <rhs>\"", ErrorManager::ERROR);
                return nullptr;
            }

            ExprNode* lhs = parseTokensR(begin, prev);
            ExprNode* rhs = parseTokensR(next, end);

            lhs->setParent(node);
            rhs->setParent(node);
            node->children().push_back(lhs);
            node->children().push_back(rhs);  
        }
        else if (op->name() == "-*")
        {
            if (split == end) // arity mismatch
            {
                gErrorManager.log("Expected \"" + op->name() + " <arg>\"", ErrorManager::ERROR);
                return nullptr;
            }

            ExprNode* arg = parseTokensR(next, end);
            arg->setParent(node);
            node->children().push_back(arg);
        }
        else if (op->name() == "!")
        {
            if (split == begin) // TODO: arity match
            {
                gErrorManager.log("Expected \"<arg> " + op->name() + "\"", ErrorManager::ERROR);
                return nullptr;
            }

            ExprNode* arg = parseTokensR(begin, prev);
            arg->setParent(node);
            node->children().push_back(arg);
        }
    }
  
    return node;
}

ExprNode* parseTokens(const std::list<ExprNode*>& tokens)
{
    if (gErrorManager.hasError()) // Don't try to parse if there's an error =
        return nullptr;

    return parseTokensR(tokens.begin(), std::prev(tokens.end()));
}

}