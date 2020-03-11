#include <iostream>
#include <iterator>
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
        else if ((isdigit(expr[itr])) ||        // <digit> OR <neg><digit> OR <dot><digit> OR <neg><dot><digit>
                (itr < (len - 1) && expr[itr] == '.' && isdigit(expr[itr + 1])) ||
                (expr[itr] == '-' && ((itr < (len - 1) && isdigit(expr[itr + 1])) ||
                                      (itr < (len - 2) && expr[itr + 1] == '.' && isdigit(expr[itr + 2])))))
        {
            ExpressionNode* num = new ExpressionNode();
            size_t i = itr + 1;
            for (; i != len && (isdigit(expr[i]) || expr[i] == '.'); ++i);
            num->mStr = expr.substr(itr, i - itr);
            num->mType = ExpressionNode::NUMBER;
            tokens.push_back(num);
            itr = i;
        }
        else if (isalpha(expr[itr]))
        {
            size_t i = itr + 1;
            for (; i != len && isalpha(expr[i]); ++i);
            ExpressionNode* str = new ExpressionNode();
            str->mStr = expr.substr(itr, i - itr);
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
                std::string match = brackets.top();
                brackets.pop();
                if ((match == "(" && expr[itr] != ')') || (match == "{" && expr[itr] != '}') || (match != "[" && expr[itr] == ']'))
                {
                    ExpressionNode* rest = new ExpressionNode();
                    rest->mStr = expr.substr(itr, len - itr);
                    rest->mType = ExpressionNode::VARIABLE;
                    tokens.push_back(rest);
                    std::cout << "Wrong closing bracket "; // TODO: error - expected different parentheses
                    return tokens;
                }
            }
            
            ExpressionNode* bracket = new ExpressionNode();
            bracket->mStr = std::string(1, expr[itr]);
            bracket->mType = ExpressionNode::OPERATOR;
            tokens.push_back(bracket);
            ++itr;
        }
        else if (isOperator(expr[itr]))
        {
            size_t i = itr + 1;
            for (; i != len && isOperator(expr[i]); ++i);
            ExpressionNode* op = new ExpressionNode();
            op->mStr = expr.substr(itr, i - itr);
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

    return tokens;
}

// Token expansion

// Implicit multiplication
// <number><constant>           3pi
// <number><function>           3sin(x)
// <number><variable>           3x
// <number><bracket>            3(...)
// <constant><function>         bsin(x)
// <constant><variable>         bx
// <constant><bracket>          b(...)
// <variable><funcion>          xsin(x)
// <variable><bracket>          x(...)
// <factorial><not operator>    5!x, 5!2, 5!sin(x)
// <bracket><bracket>           (2)(3)

void expandTokens(std::list<ExpressionNode*>& tokens)
{
    for (std::list<ExpressionNode*>::iterator it = tokens.begin(); it != tokens.end(); ++it)
    {
        std::list<ExpressionNode*>::iterator next = std::next(it);
        if (next != tokens.end() &&
            (((*it)->mType == ExpressionNode::NUMBER && ((*next)->mType == ExpressionNode::VARIABLE || // number
                (*next)->mType == ExpressionNode::FUNCTION || (*next)->mType == ExpressionNode::CONSTANT ||
                (*next)->mStr == "(")) ||
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
            tokens.insert(next, mul);
        }
    }
}

// buildExpression(tokens) and related functions

bool notContained(std::list<ExpressionNode*>::const_iterator begin, std::list<ExpressionNode*>::const_iterator end, const std::string& str)
{
    for (std::list<ExpressionNode*>::const_iterator it = begin; it != end; ++it)
    {
        if ((*it)->mStr == str)
            return false;
    }

    return true;
}

ExpressionNode* buildExpressionR(std::list<ExpressionNode*>::const_iterator begin, std::list<ExpressionNode*>::const_iterator end)
{
    if (((*begin)->mStr == "(" && (*end)->mStr == ")" && notContained(std::next(begin), end, "(")) ||
        ((*begin)->mStr == "[" && (*end)->mStr == "]" && notContained(std::next(begin), end, "[")) ||
        ((*begin)->mStr == "{" && (*end)->mStr == "}" && notContained(std::next(begin), end, "{")))
    {
        return buildExpressionR(++begin, --end);
    }

    std::list<ExpressionNode*>::const_iterator split = end; // loop through tokens from end to beginning
    size_t bracketLevel = 0;
    for (std::list<ExpressionNode*>::const_iterator it = end; it != begin; --it)
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
            if ((*it)->mPrecedence > 2)
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
        // TODO: support for functions
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

            ExpressionNode* lhs = buildExpressionR(begin, prev);
            ExpressionNode* rhs = buildExpressionR(next, end);

            lhs->mParent = node;
            rhs->mParent = node;
            node->mChildren.push_back(lhs);
            node->mChildren.push_back(rhs);  
        }
        else if (node->mStr == "!")
        {
            ExpressionNode* lhs = buildExpressionR(begin, prev);
            if (lhs != nullptr)     node->mChildren.push_back(lhs);
        }
    }
  
    return node;
}

ExpressionNode* buildExpression(const std::list<ExpressionNode*>& tokens)
{
    return buildExpressionR(tokens.begin(), std::prev(tokens.end()));
}

}