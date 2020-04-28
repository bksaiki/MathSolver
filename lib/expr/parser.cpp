#include <iostream>
#include <stack>
#include "parser.h"

namespace MathSolver
{

// See header for description.
std::list<ExprNode*> tokenizeStr(const std::string& expr)
{
    std::list<ExprNode*> tokens;
    std::stack<std::string> brackets;
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
            ExprNode* num = new ExprNode();
            size_t i = itr + 1;
            for (; i != len && (isdigit(expr[i]) || expr[i] == '.'); ++i);

            std::string str = expr.substr(itr, i - itr);
            if (str.find('.') == std::string::npos)
            {
                num->exact = str;
                num->type = ExprNode::INTEGER;
            }
            else
            {
                num->inexact = std::stod(str);
                num->type = ExprNode::FLOAT;
            }

            tokens.push_back(num);
            itr = i;
        }
        else if (isalpha(expr[itr]))
        {
            size_t i = itr + 1;
            for (; i != len && isalpha(expr[i]); ++i);
            ExprNode* str = new ExprNode();
            str->str = expr.substr(itr, i - itr);

            if (isFunction(str->str))
            {
                str->type = ExprNode::FUNCTION;
                str->prec = 1;
            }

            tokens.push_back(str);
            itr = i;
        }
        else if (isBracket(expr[itr]))
        {
            if (expr[itr] == '(' || expr[itr] == '{' || expr[itr] == '[')
            {
                brackets.push(std::string(1, expr[itr]));
            }
            else
            {
                if (brackets.empty())
                {
                    ExprNode* rest = new ExprNode();
                    rest->str = expr.substr(itr, len - itr);
                    rest->type = ExprNode::VARIABLE;
                    tokens.push_back(rest);
                    std::cout << "Unexpected bracket: \"" << expr[itr] << "\" Rest=\"" << rest->str << "\"" << std::endl; // TODO: error - expected different parentheses
                    return tokens;
                }

                std::string match = brackets.top();
                brackets.pop();
                if ((expr[itr] == ')' && match != "(") || (expr[itr] == '}' && match != "{") || (expr[itr] == ']' && match != "["))
                {
                    ExprNode* rest = new ExprNode();
                    rest->str = expr.substr(itr, len - itr);
                    rest->type = ExprNode::VARIABLE;
                    tokens.push_back(rest);
                    std::cout << "Wrong closing bracket: \"" << expr[itr] << "\" Rest=\"" << rest->str << "\"" << std::endl; // TODO: error - expected different parentheses
                    return tokens;
                }
            }
            
            ExprNode* bracket = new ExprNode();
            bracket->str = std::string(1, expr[itr]);
            bracket->type = ExprNode::OPERATOR;
            tokens.push_back(bracket);
            ++itr;
        }
        else if (isOperatorChar(expr[itr]))
        {
            ExprNode* op = new ExprNode();
            size_t i = itr;
      
            do
            {
                ++i;
                op->str = expr.substr(itr, i - itr); 
            } while (i != len && isOperator(op->str + expr[i]));

            op->type = ExprNode::OPERATOR;
            op->prec = operatorPrec(op->str);
            tokens.push_back(op);
            itr = i;
        }
        else // TODO: error "Unknown character"
        {
            std::cout << "Unknown character ";
            return tokens;
        }
        
    }

    if (!brackets.empty())
    {
        std::cout << "Mismatched brackets "; // TODO: error - expected different parentheses
        return tokens;
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
        if (it == tokens.begin() && next != tokens.end() && (*it)->str == "-" &&  // beginning negative
            ((*next)->type != ExprNode::OPERATOR || (*next)->str == "("))
        {
            (*it)->str = "-*";
            (*it)->prec = operatorPrec((*it)->str);
        }
        else if (next != tokens.end() && (*it)->type == ExprNode::OPERATOR && (*next)->str == "-")
        {
            (*next)->str = "-*";
            (*next)->prec = operatorPrec((*next)->str);
        }
        else if (next != tokens.end() && // implicit multiplication
            ((((*it)->type == ExprNode::INTEGER || (*it)->type == ExprNode::FLOAT) &&
                ((*next)->type == ExprNode::VARIABLE || (*next)->type == ExprNode::FUNCTION ||
                (*next)->type == ExprNode::CONSTANT || (*next)->str == "(")) ||
            ((*it)->type == ExprNode::CONSTANT && ((*next)->type == ExprNode::VARIABLE || // constant
                (*next)->type == ExprNode::FUNCTION || (*next)->str == "(")) ||
            ((*it)->type == ExprNode::VARIABLE && ((*next)->type == ExprNode::FUNCTION || // variable
                (*next)->str == "(")) ||
            ((*it)->str == "!" && (*next)->type != ExprNode::OPERATOR) || // factorial
            ((*it)->str == ")" && (*next)->str == "("))) // factorial
        {
            ExprNode* mul = new ExprNode();
            mul->str = "**";
            mul->type = ExprNode::OPERATOR;
            mul->prec = operatorPrec(mul->str);
            it = tokens.insert(next, mul);
        }
    }
}

// parseTokens(tokens) and related functions

bool bracketedExpr(std::list<ExprNode*>::const_iterator begin, std::list<ExprNode*>::const_iterator end)
{
    if (!((*begin)->str == "(" && (*end)->str == ")") && !((*begin)->str == "[" && (*end)->str == "]") &&
        !((*begin)->str == "{" && (*end)->str == "}"))
        return false;

    size_t bracketLevel = 0;
    for (auto it = begin; it != end; ++it)
    {
        if ((*it)->str == "(" || (*it)->str == "[" || (*it)->str == "{")
        {
            ++bracketLevel;
        }
        else if ((*it)->str == ")" || (*it)->str == "]" || (*it)->str == "}")
        {
            if (--bracketLevel == 0 && it != end)
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
        if ((*it)->str == ")" || (*it)->str == "]" || (*it)->str == "}")
        {
            ++bracketLevel;
        }
        else if ((*it)->str == "(" || (*it)->str == "[" || (*it)->str == "{")
        {
            --bracketLevel;
        }
        else if (bracketLevel == 0) // find lowest precision when not within the bracket
        {
            if ((*it)->prec > operatorPrec("^"))
            {
                if((*it)->prec > (*split)->prec)
                    split = it;
            }
            else if ((*it)->prec > 0)
            {
                if((*it)->prec >= (*split)->prec)
                    split = it;
            }
        }
    }

    ExprNode* node = *split;
    auto next = std::next(split);
    auto prev = std::prev(split);
    if (node->type == ExprNode::FUNCTION)
    {
        if (split == end) // TODO: arity match
        {
            std::cout << "Expected " << node->str << " <args>...";
            return nullptr;
        }

        if ((*next)->str != "(") // func <arg>"
        {
            ExprNode* arg = parseTokensR(next, end);
            arg->parent = node;
            node->children.push_back(arg);
        }
        else // func (<arg>, <arg>, ...)
        {
            auto it = next;
            while ((*it)->str != ")") // iterate forward now
            {
                auto it2 = std::next(it);
                bracketLevel = 0;
                while (it2 != end && (((*it2)->str != ")" && (*it2)->str != ",") || bracketLevel > 0))
                {
                    if ((*it2)->str == "(")         ++bracketLevel;
                    else if ((*it2)->str == ")")    --bracketLevel;
                    ++it2; // split arg vector
                }

                ExprNode* arg = parseTokensR(std::next(it), std::prev(it2));
                arg->parent = node;
                node->children.push_back(arg);
                delete *it;
                it = it2; 
            }
            delete *it;
        }        
    }
    else if (node->type == ExprNode::OPERATOR)
    {
        if (node->str == "+" || node->str == "-" || node->str == "**" || node->str == "*" ||
            node->str == "/" || node->str == "%" || node->str == "^" || node->str == "<" ||
            node->str == ">" || node->str == "<=" || node->str == ">=" || node->str == "=") 
        {         
            if (split == begin || split == end) // TODO: arity match
            {
                std::cout << "Expected <lhs> " << node->str << " <rhs>.";
                return nullptr;
            }

            ExprNode* lhs = parseTokensR(begin, prev);
            ExprNode* rhs = parseTokensR(next, end);

            lhs->parent = node;
            rhs->parent = node;
            node->children.push_back(lhs);
            node->children.push_back(rhs);  
        }
        else if (node->str == "-*")
        {
            if (split == end) // TODO: arity match
            {
                std::cout << "Expected " << node->str << " <arg>.";
                return nullptr;
            }

            ExprNode* arg = parseTokensR(next, end);
            arg->parent = node;
            node->children.push_back(arg);
        }
        else if (node->str == "!")
        {
            if (split == begin) // TODO: arity match
            {
                std::cout << "Expected <arg> " << node->str << ".";
                return nullptr;
            }

            ExprNode* arg = parseTokensR(begin, prev);
            arg->parent = node;
            node->children.push_back(arg);
        }
    }
  
    return node;
}

ExprNode* parseTokens(const std::list<ExprNode*>& tokens)
{
    return parseTokensR(tokens.begin(), std::prev(tokens.end()));
}

}