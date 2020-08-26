#include <list>
#include <map>
#include "matcher.h"
#include "parser.h"

namespace MathSolver
{

std::vector<std::string> tokenizeMatchString(const std::string& match)
{
    std::vector<std::string> tokens;
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
            while (j < len && !isspace(match[j]) && match[i] != '(' && match[j] != ')')
                ++j;
            tokens.push_back(match.substr(i, j - i));
            i = j - 1;
        }   
    }

    return tokens;
}

bool isMatchString(const std::string& match)
{
    std::vector<std::string> tokens = tokenizeMatchString(match);
    std::map<int, int> subexprCount;
    std::map<int, bool> ellipseUsed;
    int parenCount = 0;

    for (size_t i = 0; i < tokens.size(); ++i)
    {
        if (tokens[i] == "(")
        {
            // increment subexpr count of this subexpr layer
            if (subexprCount.find(parenCount) != subexprCount.end())    ++subexprCount[parenCount];
            else                                                        subexprCount[parenCount] = 1;
            
            // set the next layer to 0
            ++parenCount;
            subexprCount[parenCount] = 0;
            ellipseUsed[parenCount] = false;
        }
        else if (tokens[i] == ")")
        {
            if (subexprCount[parenCount] == 0)      return false;
            subexprCount.erase(parenCount);
            ellipseUsed.erase(parenCount);
            --parenCount;
        }
        else if (tokens[i] == "...")
        {
            if (ellipseUsed[parenCount])       return false;
            ellipseUsed[parenCount] = true;
        }
        else if (tokens[i] == "...?")
        {
            if (tokens[i + 1] != ")")   
                return false; // ellipses may only appear in the tail
        }
        else
        {
            ++subexprCount[parenCount]; // assume this subexpr count has been set to 0
        }     
    }

    if (parenCount != 0)    return false; // unmatched parenthesis
    return true;
}

std::vector<std::string> extractMatchSubexpr(const std::vector<std::string>& match,
                                             std::vector<std::string>::const_iterator& it)
{
    std::vector<std::string> subexpr;
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

bool matchExprHelper(const std::vector<std::string> tokens, ExprNode* expr)
{
    if (tokens.size() == 1)
    {
        return matchExprToken(tokens.front(), expr);
    }
    else
    {
        std::vector<std::vector<std::string>> sexprs;
        auto it = std::next(tokens.begin());
        auto end = std::prev(tokens.end());
        
       while (it != end)
       {
           std::vector<std::string> s = extractMatchSubexpr(tokens, it);
           it = std::next(it, s.size());
           sexprs.push_back(s);
       }

       if (sexprs[0].size() != 1 || !matchExprToken(sexprs[0][0], expr))
            return false;   // operator did not match
        
        auto it2 = expr->children().begin();
        size_t argc = 0;
        if (sexprs.back().front() == "...?")
        {
            size_t len = expr->children().size();
            size_t needed = sexprs.size() - 2;
            size_t check = len - needed;
            size_t idx = 1;
            for (; argc <= check + idx && argc < len; ++argc, ++it2)
            {
                if (matchExprHelper(sexprs[idx], *it2))
                {
                    if (idx == needed)  return true;
                    ++idx;
                }
            }

            return false;
        }
        else
        {
            for (size_t i = 1; i < sexprs.size(); ++i, ++it2, ++argc)
            {
                if (it2 == expr->children().end()) // too many match arguments
                    return false;
                
                if (i + 1 < sexprs.size() && sexprs[i + 1].front() == "...")
                {
                    size_t after = (sexprs.size() - 2) - i;
                    size_t inlist = expr->children().size() - argc - after;
                    argc += (inlist - 1);
                    it2 = std::next(it2, (inlist - 1));
                    ++i;
                }
                else
                {
                    if (!matchExprHelper(sexprs[i], *it2))
                        return false;
                }
            }

            if (argc != expr->children().size()) // too few match arguments
                return false;
        }

        return true;
    }
}

bool matchExpr(const std::string& match, ExprNode* expr)
{
    std::vector<std::string> tokens = tokenizeMatchString(match);
    return matchExprHelper(tokens, expr);
} 

void loadMatchDict(const std::vector<std::string> tokens, ExprNode* expr,
                   std::map<std::string, ExprNode*>& matchDict)
{
    if (tokens.size() == 1)
    {
        matchDict[tokens.front()] = expr;
    }
    else
    {
        std::vector<std::vector<std::string>> sexprs;
        auto it = std::next(tokens.begin());
        auto end = std::prev(tokens.end());
        
       while (it != end)
       {
           std::vector<std::string> s = extractMatchSubexpr(tokens, it);
           it = std::next(it, s.size());
           sexprs.push_back(s);
       }

       loadMatchDict(sexprs[0], expr, matchDict);
        
        auto it2 = expr->children().begin();
        size_t argc = 0;
        if (sexprs.back()[0] == "...?")
        {
            ExprNode* ell = new SyntaxNode("...?");
            ExprNode* before = new SyntaxNode("before", ell);
            ExprNode* after = new SyntaxNode("after", ell);
            ell->children().push_back(before);
            ell->children().push_back(after);

            size_t len = expr->children().size();
            size_t needed = sexprs.size() - 2;
            size_t check = len - needed;
            size_t idx = 1;
            bool matched = false;
            for (; argc <= check + idx && argc < len; ++argc, ++it2)
            {
                if (matchExprHelper(sexprs[idx], *it2))
                {
                   loadMatchDict(sexprs[idx], *it2, matchDict);
                    ++idx;
                    matched = true;
                }
                else
                {
                    if (matched)    after->children().push_back(*it2);
                    else            before->children().push_back(*it2);
                }  
            }

            matchDict["...?"] = ell;
        }
        else
        {
            for (size_t i = 1; i < sexprs.size(); ++i, ++it2, ++argc)
            {
                if (i + 1 < sexprs.size() && sexprs[i + 1].front() == "...")
                {
                    size_t after = (sexprs.size() - 2) - i;
                    size_t inlist = expr->children().size() - argc - after;

                    ExprNode* ell = new SyntaxNode("...");
                    for (size_t j = 0; j < inlist; ++j, ++it2)
                    {
                        ExprNode* t = *it2;
                        t->setParent(ell);
                        ell->children().push_back(t);
                    }

                    matchDict[sexprs[i].front()] = ell;
                    ++i;
                    --it2;
                    argc += (inlist - 1);
                }
                else
                {
                    loadMatchDict(sexprs[i], *it2, matchDict);
                }
            }
        }
    }
}

ExprNode* buildTree(const std::vector<std::string> tokens,
                    const std::map<std::string, ExprNode*>& matchDict)
{
    if (tokens.size() == 1)
    {
        if (matchDict.find(tokens.front()) != matchDict.end())
        {
            if (tokens.front().front() == '?')
            {
                ExprNode* t = matchDict.at(tokens.front());
                return ((t->toString() == "...") ? t : copyOf(t));
            }
           
           return copyNode(matchDict.at(tokens.front()));
        }
        
        std::list<ExprNode*> exprTokens = tokenizeStr(tokens.front());
        return exprTokens.front();
    }
    else
    {
        std::vector<std::vector<std::string>> sexprs;
        auto it = std::next(tokens.begin());
        auto end = std::prev(tokens.end());

        while (it != end)
        {
            std::vector<std::string> s = extractMatchSubexpr(tokens, it);
            it = std::next(it, s.size());
            sexprs.push_back(s);
        }
        
        ExprNode* op = buildTree(sexprs[0], matchDict);
        bool ellipse = sexprs.back().front() == "...?";
        
        if (ellipse)
        {
            ExprNode* ell = matchDict.at("...?");
            for (auto e : ell->children().front()->children())
            {
                ExprNode* t = copyOf(e);
                t->setParent(op);
                op->children().push_back(t);
            }
        }

        size_t lim = (ellipse ? (sexprs.size() - 1) : sexprs.size());
        for (size_t i = 1; i < lim; ++i)
        {
            ExprNode* sub = buildTree(sexprs[i], matchDict);
            if (sub->toString() == "...") // ellipse match
            {
                for (auto e : sub->children())
                {
                    ExprNode* t = copyOf(e);
                    t->setParent(op);
                    op->children().push_back(t);
                }

                ++i; // skip ellipse
            }
            else
            {
                sub->setParent(op);
                op->children().push_back(sub);
            }
        }

        if (ellipse)
        {
            ExprNode* ell = matchDict.at("...?");
            for (auto e : ell->children().back()->children())
            {
                ExprNode* t = copyOf(e);
                t->setParent(op);
                op->children().push_back(t);
            }
        }

        return op;
    }
}

ExprNode* applyMatchTransform(const std::string& input, const std::string& output, ExprNode* expr)
{
    std::vector<std::string> itokens = tokenizeMatchString(input);
    std::vector<std::string> otokens = tokenizeMatchString(output);
    std::map<std::string, ExprNode*> matchDict;
    ExprNode* ret;

    loadMatchDict(itokens, expr, matchDict);
    ret = buildTree(otokens, matchDict);

    for (auto e : matchDict)
    {
        if (e.first == "...?")
        {
            delete e.second->children().front();
            delete e.second->children().back();
            delete e.second;
        }
        else if (e.second->toString() == "...")
        {
            delete e.second;
        }
    }

    freeExpression(expr);
    return ret;
}

// ** Unique Transform Matcher ** //

UniqueTransformMatcher::UniqueTransformMatcher()   // default constructor
{
    mSuccess = false;
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

void UniqueTransformMatcher::clear()
{
    mTransforms.clear();
    mSuccess = false;
}

ExprNode* UniqueTransformMatcher::transform(ExprNode* expr)
{
    for (auto e : mTransforms)
    {
        if (matchExpr(e.first, expr))   // if match, return transform
        {
            mSuccess = true;
            return applyMatchTransform(e.first, e.second, expr);
        }
    }

    mSuccess = false;
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

    std::vector<std::string> tokens = tokenizeMatchString(match);
    if (!matchExprHelper(tokens, expr))
        return false;

    if (!mSubexprs.empty())
        mSubexprs.clear();

    loadMatchDict(tokens, expr, mSubexprs);
    return true;
}

}