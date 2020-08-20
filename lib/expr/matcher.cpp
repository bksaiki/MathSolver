#include <list>
#include <map>
#include "matcher.h"
#include "parser.h"

namespace MathSolver
{

std::list<std::string> tokenizeMatchString(const std::string& match)
{
    std::list<std::string> tokens;
    size_t len = match.length();
    for (size_t i = 0; i < len; ++i)
    {
        while (isspace(match[i])) ++i;

        if (match[i] == '(' || match[i] == ')')
        {
            tokens.push_back(std::string(1, match[i]));
        }
        else
        {  
            size_t j = i;
            while (!isspace(match[j]) && match[i] != '(' && match[j] != ')') ++j;
            tokens.push_back(match.substr(i, j - i));
            i = j - 1;
        }   
    }

    return tokens;
}

bool isMatchString(const std::string& match)
{
    std::list<std::string> tokens = tokenizeMatchString(match);
    std::map<int, int> subexprCount;
    int parenCount = 0;

    subexprCount[0] = 0;
    for (auto e : tokens)
    {
        if (e == "(")
        {
            // increment subexpr count of this subexpr layer
            if (subexprCount.find(parenCount) != subexprCount.end())    ++subexprCount[parenCount];
            else                                                        subexprCount[parenCount] = 1;
            
            // set the next layer to 0
            subexprCount[++parenCount] = 0;
        }
        else if (e == ")")
        {
            if (subexprCount[parenCount] == 0)      return false;
            subexprCount.erase(parenCount);
            --parenCount;
        }
        else
        {
            ++subexprCount[parenCount]; // assume this subexpr count has been set to 0
        }     
    }

    if (parenCount != 0)    return false; // unmatched parenthesis
    return true;
}

std::list<std::string> extractMatchSubexpr(const std::list<std::string>& match, std::list<std::string>::const_iterator& it)
{
    std::list<std::string> subexpr;
    if (*it == "(")
    {
        auto it2 = std::next(it);
        int parenCount = 1;
        for (; it2 != match.end() && parenCount > 0; ++it2)
        {
            if (*it2 == "(")        ++parenCount;
            else if (*it2 == ")")   --parenCount;
        }

        subexpr.insert(subexpr.begin(), it, it2);
    }
    else
    {
        subexpr.push_back(*it);
    }

    return subexpr;
}

bool matchExprToken(const std::string& token, ExprNode* expr)
{
    if (token.front() != '?')   return token == expr->toString();
    else                        return true;
}

bool matchExprHelper(const std::list<std::string> tokens, ExprNode* expr)
{
    if (tokens.size() == 1)
    {
        return matchExprToken(tokens.front(), expr);
    }
    else
    {
        std::list<std::string> sexpr;
        auto it = std::next(tokens.begin());
        auto end = std::prev(tokens.end());
        size_t argc = 0;

        sexpr = extractMatchSubexpr(tokens, it);
        if (sexpr.size() != 1 || !matchExprToken(sexpr.front(), expr)) 
            return false;

        it = std::next(it, sexpr.size());
        for (auto it2 = expr->children().begin(); it != end; ++argc, ++it2, it = std::next(it, sexpr.size()))
        {
            if (it2 == expr->children().end()) // too many match arguments
                return false;

            sexpr = extractMatchSubexpr(tokens, it);    
            if (!matchExprHelper(sexpr, *it2))
                return false;
        }

        if (argc != expr->children().size()) // too few match arguments
            return false;

        return true;
    }
}

bool matchExpr(const std::string& match, ExprNode* expr)
{
    std::list<std::string> tokens = tokenizeMatchString(match);
    return matchExprHelper(tokens, expr);
} 

void loadMatchDict(const std::list<std::string> tokens, ExprNode* expr, std::map<std::string, ExprNode*>& matchDict)
{
    if (tokens.size() == 1)
    {
        matchDict[tokens.front()] = expr;
    }
    else
    {
        std::list<std::string> sexpr;
        auto it = std::next(tokens.begin());
        auto end = std::prev(tokens.end());

        sexpr = extractMatchSubexpr(tokens, it);    
        loadMatchDict(sexpr, expr, matchDict);
        it = std::next(it, sexpr.size());

        for (auto it2 = expr->children().begin(); it != end; ++it2, it = std::next(it, sexpr.size()))
        {
            sexpr = extractMatchSubexpr(tokens, it);    
            loadMatchDict(sexpr, *it2, matchDict);
        }
    }
}

ExprNode* buildTree(const std::list<std::string> tokens, const std::map<std::string, ExprNode*>& matchDict)
{
    if (tokens.size() == 1)
    {
        if (matchDict.find(tokens.front()) != matchDict.end())
            return copyNodeNoTree(matchDict.at(tokens.front()));
        
        std::list<ExprNode*> exprTokens = tokenizeStr(tokens.front());
        return exprTokens.front();
    }
    else
    {
        std::list<std::string> sexpr;
        auto it = std::next(tokens.begin());
        auto end = std::prev(tokens.end());

        sexpr = extractMatchSubexpr(tokens, it);    
        ExprNode* op = buildTree(sexpr, matchDict);

        for (it = std::next(it, sexpr.size()); it != end; it = std::next(it, sexpr.size()))
        {
            sexpr = extractMatchSubexpr(tokens, it);    
            ExprNode* sub = buildTree(sexpr, matchDict);
            sub->setParent(op);
            op->children().push_back(sub);
        }
        
        return op;
    }
}

ExprNode* applyMatchTransform(const std::string& input, const std::string& output, ExprNode* expr)
{
    std::list<std::string> itokens = tokenizeMatchString(input);
    std::list<std::string> otokens = tokenizeMatchString(output);
    std::map<std::string, ExprNode*> matchDict;
    ExprNode* ret;

    loadMatchDict(itokens, expr, matchDict);
    ret = buildTree(otokens, matchDict);
    freeExpression(expr);
    return ret;
}

// ** Unique Transform Matcher ** //

UniqueTransformMatcher::UniqueTransformMatcher()   // default constructor
{
    successful = false;
}

void UniqueTransformMatcher::add(const std::string& input, const std::string& output)
{
    if (!isMatchString(input) || !isMatchString(output))
    {
        gErrorManager.log("Expected a match transform. Got " + input + " and " + output,
                          ErrorManager::FATAL);
    }
    else if (mTransforms.find(input) != mTransforms.end())
    {
        gErrorManager.log("Overwriting transform " + input + " -> " + mTransforms.at(input) + " with " + output,
                          ErrorManager::WARNING);
    }
    else
    {
        mTransforms[input] = output;
    }
}

ExprNode* UniqueTransformMatcher::transform(ExprNode* expr)
{
    for (auto e : mTransforms)
    {
        if (matchExpr(e.first, expr))   // if match, return transform
        {
            successful = true;
            return applyMatchTransform(e.first, e.second, expr);
        }
    }

    successful = false;
    return expr; // else, return unaltered
}

ExprNode* UniqueExprMatcher::get(const std::string& key) const
{
    if (mSubexprs.find(key) == mSubexprs.end())
    {
        gErrorManager.log("Could not find match subexpression key \"" + key + "\"",
                          ErrorManager::FATAL);
        return nullptr;
    }

    return mSubexprs.at(key);
}

bool UniqueExprMatcher::match(ExprNode* expr, const std::string& match)
{
    if (!isMatchString(match))
    {
        gErrorManager.log("Expected a match transform. Got " + match + " instead",
                          ErrorManager::FATAL);
        return false;
    }

    std::list<std::string> tokens = tokenizeMatchString(match);
    if (!matchExprHelper(tokens, expr))
        return false;

    if (!mSubexprs.empty())
        mSubexprs.clear();

    loadMatchDict(tokens, expr, mSubexprs);
    return true;
}

}