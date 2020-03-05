#include <iostream>
#include <stack>
#include "parser.h"

namespace MathSolver
{

// See header for description.
std::list<std::string> tokenizeStr(const std::string& expr)
{
    std::list<std::string> tokens;
    std::stack<std::string> brackets;
    size_t len = expr.length();
    size_t itr = 0;

    while (itr != len)
    {
        if (isspace(expr[itr]))
        {
            ++itr;
        }
        else if (isdigit(expr[itr]) || expr[itr] == '.')
        {
            size_t i = itr + 1;
            for (; i != len && (isdigit(expr[i]) || expr[i] == '.'); ++i);
            tokens.push_back(expr.substr(itr, i - itr));
            itr = i;
        }
        else if (isalpha(expr[itr]))
        {
            size_t i = itr + 1;
            for (; i != len && isalpha(expr[i]); ++i);
            tokens.push_back(expr.substr(itr, i - itr));
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
                    tokens.push_back(expr.substr(itr, len - itr));
                    std::cout << "Wrong closing bracket "; // TODO: error - expected different parentheses
                    return tokens;
                }
            }
            
            tokens.push_back(std::string(1, expr[itr]));
            ++itr;
        }
        else if (isOperator(expr[itr]))
        {
            size_t i = itr + 1;
            for (; i != len && isOperator(expr[i]); ++i);
            tokens.push_back(expr.substr(itr, i - itr));
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

bool reorderTokens(const std::list<std::string>& tokens)
{
    return true;
}

}