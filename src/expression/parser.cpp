#include <iostream>
#include <stack>
#include "parser.h"

namespace MathSolver
{

// See header for description.
std::list<ExpressionNode*> tokenizeStr(const std::string& expr)
{
    std::list<ExpressionNode*> tokens;
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
            ExpressionNode* num = new ExpressionNode();
            size_t i = itr + 1;
            for (; i != len && (isdigit(expr[i]) || expr[i] == '.'); ++i);

            std::string str = expr.substr(itr, i - itr);
            if (str.find('.') == std::string::npos)
            {
                num->mExact = str;
                num->mType = ExpressionNode::INTEGER;
            }
            else
            {
                num->mInexact = std::stod(str);
                num->mType = ExpressionNode::FLOAT;
            }

            tokens.push_back(num);
            itr = i;
        }
        else if (isalpha(expr[itr]))
        {
            size_t i = itr + 1;
            for (; i != len && isalpha(expr[i]); ++i);
            ExpressionNode* str = new ExpressionNode();
            str->mStr = expr.substr(itr, i - itr);

            if (isFunction(str->mStr))
            {
                str->mType = ExpressionNode::FUNCTION;
                str->mPrecedence = 1;
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
                    ExpressionNode* rest = new ExpressionNode();
                    rest->mStr = expr.substr(itr, len - itr);
                    rest->mType = ExpressionNode::VARIABLE;
                    tokens.push_back(rest);
                    std::cout << "Unexpected bracket: \"" << expr[itr] << "\" Rest=\"" << rest->mStr << "\"" << std::endl; // TODO: error - expected different parentheses
                    return tokens;
                }

                std::string match = brackets.top();
                brackets.pop();
                if ((expr[itr] == ')' && match != "(") || (expr[itr] == '}' && match != "{") || (expr[itr] == ']' && match != "["))
                {
                    ExpressionNode* rest = new ExpressionNode();
                    rest->mStr = expr.substr(itr, len - itr);
                    rest->mType = ExpressionNode::VARIABLE;
                    tokens.push_back(rest);
                    std::cout << "Wrong closing bracket: \"" << expr[itr] << "\" Rest=\"" << rest->mStr << "\"" << std::endl; // TODO: error - expected different parentheses
                    return tokens;
                }
            }
            
            ExpressionNode* bracket = new ExpressionNode();
            bracket->mStr = std::string(1, expr[itr]);
            bracket->mType = ExpressionNode::OPERATOR;
            tokens.push_back(bracket);
            ++itr;
        }
        else if (isOperatorChar(expr[itr]))
        {
            ExpressionNode* op = new ExpressionNode();
            size_t i = itr;
      
            do
            {
                ++i;
                op->mStr = expr.substr(itr, i - itr); 
            } while (i != len && isOperator(op->mStr + expr[i]));

            op->mType = ExpressionNode::OPERATOR;
            op->mPrecedence = operatorPrec(op->mStr);
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

void expandTokens(std::list<ExpressionNode*>& tokens)
{
    for (std::list<ExpressionNode*>::iterator it = tokens.begin(); it != tokens.end(); ++it)
    {
        std::list<ExpressionNode*>::iterator next = std::next(it);
        if (it == tokens.begin() && next != tokens.end() && (*it)->mStr == "-" &&  // beginning negative
            ((*next)->mType != ExpressionNode::OPERATOR || (*next)->mStr == "("))
        {
            (*it)->mStr = "-*";
            (*it)->mPrecedence = operatorPrec((*it)->mStr);
        }
        else if (next != tokens.end() && (*it)->mType == ExpressionNode::OPERATOR && (*next)->mStr == "-")
        {
            (*next)->mStr = "-*";
            (*next)->mPrecedence = operatorPrec((*next)->mStr);
        }
        else if (next != tokens.end() && // implicit multiplication
            ((((*it)->mType == ExpressionNode::INTEGER || (*it)->mType == ExpressionNode::FLOAT) &&
                ((*next)->mType == ExpressionNode::VARIABLE || (*next)->mType == ExpressionNode::FUNCTION ||
                (*next)->mType == ExpressionNode::CONSTANT || (*next)->mStr == "(")) ||
            ((*it)->mType == ExpressionNode::CONSTANT && ((*next)->mType == ExpressionNode::VARIABLE || // constant
                (*next)->mType == ExpressionNode::FUNCTION || (*next)->mStr == "(")) ||
            ((*it)->mType == ExpressionNode::VARIABLE && ((*next)->mType == ExpressionNode::FUNCTION || // variable
                (*next)->mStr == "(")) ||
            ((*it)->mStr == "!" && (*next)->mType != ExpressionNode::OPERATOR) || // factorial
            ((*it)->mStr == ")" && (*next)->mStr == "("))) // factorial
        {
            ExpressionNode* mul = new ExpressionNode();
            mul->mStr = "**";
            mul->mType = ExpressionNode::OPERATOR;
            mul->mPrecedence = operatorPrec(mul->mStr);
            it = tokens.insert(next, mul);
        }
    }
}

// parseTokens(tokens) and related functions

bool bracketedExpr(std::list<ExpressionNode*>::const_iterator begin, std::list<ExpressionNode*>::const_iterator end)
{
    if (!((*begin)->mStr == "(" && (*end)->mStr == ")") && !((*begin)->mStr == "[" && (*end)->mStr == "]") &&
        !((*begin)->mStr == "{" && (*end)->mStr == "}"))
        return false;

    size_t bracketLevel = 0;
    for (std::list<ExpressionNode*>::const_iterator it = begin; it != end; ++it)
    {
        if ((*it)->mStr == "(" || (*it)->mStr == "[" || (*it)->mStr == "{")
        {
            ++bracketLevel;
        }
        else if ((*it)->mStr == ")" || (*it)->mStr == "]" || (*it)->mStr == "}")
        {
            if (--bracketLevel == 0 && it != end)
                return false;
        }
    }

    return true;
}

ExpressionNode* parseTokensR(std::list<ExpressionNode*>::const_iterator begin, std::list<ExpressionNode*>::const_iterator end)
{
    if (bracketedExpr(begin, end))
    {
        ExpressionNode* ret = parseTokensR(std::next(begin),std::prev(end));
        delete *begin;
        delete *end;
        return ret;
    }

    std::list<ExpressionNode*>::const_iterator split = end; // loop through tokens from end to beginning
    std::list<ExpressionNode*>::const_iterator rend = std::prev(begin);
    size_t bracketLevel = 0;
    for (std::list<ExpressionNode*>::const_iterator it = end; it != rend; --it)
    {
        if ((*it)->mStr == ")" || (*it)->mStr == "]" || (*it)->mStr == "}")
        {
            ++bracketLevel;
        }
        else if ((*it)->mStr == "(" || (*it)->mStr == "[" || (*it)->mStr == "{")
        {
            --bracketLevel;
        }
        else if (bracketLevel == 0) // find lowest precision when not within the bracket
        {
            if ((*it)->mPrecedence > operatorPrec("^"))
            {
                if((*it)->mPrecedence > (*split)->mPrecedence)
                    split = it;
            }
            else if ((*it)->mPrecedence > 0)
            {
                if((*it)->mPrecedence >= (*split)->mPrecedence)
                    split = it;
            }
        }
    }

    ExpressionNode* node = *split;
    std::list<ExpressionNode*>::const_iterator next = std::next(split);
    std::list<ExpressionNode*>::const_iterator prev = std::prev(split);
    if (node->mType == ExpressionNode::FUNCTION)
    {
        if (split == end) // TODO: arity match
        {
            std::cout << "Expected " << node->mStr << " <args>...";
            return nullptr;
        }

        if ((*next)->mStr != "(") // func <arg>"
        {
            ExpressionNode* arg = parseTokensR(next, end);
            arg->mParent = node;
            node->mChildren.push_back(arg); 
        }
        else // func (<arg>, <arg>, ...)
        {
            std::list<ExpressionNode*>::const_iterator it = next;
            while ((*it)->mStr != ")") // iterate forward now
            {
                std::list<ExpressionNode*>::const_iterator it2 = std::next(it);
                bracketLevel = 0;
                while (it2 != end && (*it2)->mStr != "," && ((*it2)->mStr != ")" || bracketLevel > 0))
                {
                    if ((*it2)->mStr == "(")         ++bracketLevel;
                    else if ((*it2)->mStr == ")")    --bracketLevel;
                    ++it2; // split arg vector
                }

                ExpressionNode* arg = parseTokensR(std::next(it), std::prev(it2));
                arg->mParent = node;
                node->mChildren.push_back(arg);
                delete *it;
                it = it2; 
            }
            delete *it;
        }        
    }
    else if (node->mType == ExpressionNode::OPERATOR)
    {
        if (node->mStr == "+" || node->mStr == "-" || node->mStr == "**" || node->mStr == "*" ||
            node->mStr == "/" || node->mStr == "%" || node->mStr == "^" || node->mStr == "<" ||
            node->mStr == ">" || node->mStr == "<=" || node->mStr == ">=" || node->mStr == "=") 
        {         
            if (split == begin || split == end) // TODO: arity match
            {
                std::cout << "Expected <lhs> " << node->mStr << " <rhs>.";
                return nullptr;
            }

            ExpressionNode* lhs = parseTokensR(begin, prev);
            ExpressionNode* rhs = parseTokensR(next, end);

            lhs->mParent = node;
            rhs->mParent = node;
            node->mChildren.push_back(lhs);
            node->mChildren.push_back(rhs);  
        }
        else if (node->mStr == "-*")
        {
            if (split == end) // TODO: arity match
            {
                std::cout << "Expected " << node->mStr << " <arg>.";
                return nullptr;
            }

            ExpressionNode* arg = parseTokensR(next, end);
            arg->mParent = node;
            node->mChildren.push_back(arg);
        }
        else if (node->mStr == "!")
        {
            if (split == begin) // TODO: arity match
            {
                std::cout << "Expected <arg> " << node->mStr << ".";
                return nullptr;
            }

            ExpressionNode* arg = parseTokensR(begin, prev);
            arg->mParent = node;
            node->mChildren.push_back(arg);
        }
    }
  
    return node;
}

ExpressionNode* parseTokens(const std::list<ExpressionNode*>& tokens)
{
    return parseTokensR(tokens.begin(), std::prev(tokens.end()));
}

}