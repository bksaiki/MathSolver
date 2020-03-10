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
        else if (expr[itr] == '-' || isdigit(expr[itr]) || expr[itr] == '.')
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

    std::list<ExpressionNode*>::const_iterator split = end;
    for (std::list<ExpressionNode*>::const_iterator it = end; it != begin; --it)
    {
        if ((*it)->mPrecedence > 3)
        {
            if((*it)->mPrecedence >= (*split)->mPrecedence)
                split = it;
        }
        else if ((*it)->mPrecedence > 0)
        {
            if((*it)->mPrecedence > (*split)->mPrecedence)
                split = it;
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
        if (node->mStr == "+" || node->mStr == "-" || node->mStr == "*" || node->mStr == "/" ||
            node->mStr == "%" || node->mStr == "^" || node->mStr == "<" || node->mStr == ">" ||
            node->mStr == "<=" || node->mStr == ">=" || node->mStr == "=") 
        {         
            ExpressionNode* lhs = buildExpressionR(begin, prev);
            ExpressionNode* rhs = buildExpressionR(next, end);

            if (lhs != nullptr)     node->mChildren.push_back(lhs);
            if (rhs != nullptr)     node->mChildren.push_back(rhs);   
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