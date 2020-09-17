#include <algorithm>
#include <list>
#include <map>
#include "matcher.h"
#include "parser.h"

namespace MathSolver
{

// 
// MatchDict
//

void MatchDict::add(const std::string& id, ExprNode* expr)
{
    if (mDict.find(id) != mDict.end())
    {
        gErrorManager.log("MatchDict: overwriting key \"" + id + "\"",
                          ErrorManager::WARNING);
        return;
    }

    mDict[id] = expr;
}

void MatchDict::clear()
{
    for (auto e : mDict)
    {
        if (e.second->toString() == "...?") // (...? (before a b) (after c d))
        {
            delete e.second->children().front();
            delete e.second->children().back();
            delete e.second;
        }
        else if (e.second->toString() == "...") // (... a b)
        {
            delete e.second;
        }
    }

    mDict.clear();
    mEll.clear();
}

ExprNode* MatchDict::get(const std::string& id)
{
    auto it = mDict.find(id);
    if (it != mDict.end())  return it->second;
    else                    return nullptr;
}

//
// MatchExpr
//

bool MatchExpr::match(ExprNode* expr, MatchDict& dict) const
{
    bool res = matchSubexpr(mTop, expr, dict);

    if (!res) dict.clear();
    return res;
}

bool MatchExpr::match(ExprNode* expr) const
{
    MatchDict dict;
    return match(expr, dict);
}

void MatchExpr::set(const std::string& match)
{
    std::vector<std::string> tokens = tokenizeMatchString(match);
    mTop = buildMatchTree(tokens);
}

MatchExpr::node MatchExpr::buildMatchTree(const std::vector<std::string>& tokens) const
{
    if (tokens.size() == 1)
    {
        node leaf;
        leaf.name = tokens.front();
        leaf.expr = nullptr;
        leaf.type = ((tokens.front().front() == '?') ? node::VARIABLE : node::SINGLE);
        leaf.depth = 0;
        return leaf;
    }
    else
    {
        if (tokens.front() != "(" && tokens.front() != ")")
        {
            gErrorManager.log("Expected a subexpression inside parenthesis!", ErrorManager::FATAL);
            return node();
        }

        node top;
        top.name = tokens[1];
        top.expr = nullptr;
        top.type = node::SINGLE;

        size_t end = tokens.size() - 1; // end paren
        for (size_t i = 2; i < end; ++i)
        {
            if (tokens[i] == "(")
            {
                size_t paren = 1;
                size_t j = i + 1;
                for (; j != end && paren != 0; ++j)
                {
                    if (tokens[j] == "(")       ++paren;
                    else if (tokens[j] == ")")  --paren;
                }

                std::vector<std::string> sexpr(tokens.begin() + i, tokens.begin() + j);
                top.children.push_back(buildMatchTree(sexpr));
                i = j - 1;
            }
            else if (tokens[i] == "..." || tokens[i] == "...?")
            {
                if (i + 1 != end)
                {
                    gErrorManager.log("Ellipses can only appear at the end of a subexpression", ErrorManager::FATAL);
                    return node();
                }

                top.children.back().depth += 1;
                top.children.back().type = ((tokens[i] == "...") ? node::ELLIPSE : node::REL_ELLIPSE);
            }
            else
            {
                std::vector<std::string> sexpr = { tokens[i] };
                top.children.push_back(buildMatchTree(sexpr));
            }
        }

        auto cmp = [](node& lhs, node& rhs) { return lhs.depth < rhs.depth; };
        auto it = std::min_element(top.children.begin(), top.children.end(), cmp);
        top.depth = it->depth + 1; // min(children) + 1
        return top;
    }
}

bool MatchExpr::matchLeaf(const node& match, ExprNode* expr, MatchDict& dict) const
{
    const std::string& token = match.name;
    if (token.front() == '?')  // if a variable
    {
        auto inDict = dict.get(token);
        if (inDict != nullptr) return eqvExpr(inDict, expr); // check equivalence
        
        MatchDict::ell_dict_t& ellDict = dict.ellDict();
        dict.add(token, expr); // add to the main dictionary

        auto pred = [&](MatchDict::ell_dict_t::value_type pair) {
            return pair.first.first == token;
        };

        auto it = std::find_if(ellDict.begin(), ellDict.end(), pred);
        if (it != ellDict.end()) // clear from the ellipse dictionary
        {
            const std::string& rest = it->first.second;

            ExprNode* ell = new SyntaxNode("...");
            ExprNode* before = new SyntaxNode("before", ell);
            ExprNode* after = new SyntaxNode("after", ell);

            ell->children().push_back(before);
            ell->children().push_back(after);

            bool found = false;
            for (auto n : it->second->children())
            {
                if (eqvExpr(n, expr))   found = true;
                else if (found)         after->children().push_back(n);
                else                    before->children().push_back(n);
            }

            dict.add(rest, ell);    // add rest identifier
            it = ellDict.erase(it); // remove
        }
        
        return true;
    }

    if (dict.get(token) == nullptr) dict.add(token, expr);  // add if not found
    return (token == expr->toString()); // check if it's the same
}

bool MatchExpr::matchSubexpr(const node& match, ExprNode* expr, MatchDict& dict) const
{
    if (match.children.empty()) // leaf
        return matchLeaf(match, expr, dict);
    
    // match subtree
    if (!matchLeaf(match, expr, dict))
        return false;

    const std::vector<node>& sexprs = match.children;
    if (sexprs.back().type == node::REL_ELLIPSE)  // (+ 0 ?b ...?)
    {   
        // unknown relative ellipse match (+ ?a ?b ...?), but
        // '?b' is contained in the ellipse dict
        if (sexprs.size() == 2 && sexprs[0].name.front() == '?' &&
            dict.get(sexprs[1].name) == nullptr)
        {
            MatchDict::ell_dict_t& ellDict = dict.ellDict();
            const std::string& head = sexprs[0].name;
            const std::string& rest = sexprs[1].name;

            auto pred = [&](MatchDict::ell_dict_t::value_type pair) {
                return pair.first.first == head;
            };
            auto it = std::find_if(ellDict.begin(), ellDict.end(), pred);
            
            if (it != ellDict.end()) // in the ellipse dict
            {
                bool matchFound = false;
                for (auto i : expr->children())
                {
                    for (auto j : it->second->children())
                    {
                        if (eqvExpr(i, j))
                        {
                            // this subexpression
                            ExprNode* ell = new SyntaxNode("...?");
                            ExprNode* before = new SyntaxNode("before", ell);
                            ExprNode* after = new SyntaxNode("after", ell);
                            bool found = false;

                            for (auto n : expr->children())
                            {
                                if (eqvExpr(n, i))   found = true;
                                else if (found)      after->children().push_back(n);
                                else                 before->children().push_back(n);
                            }

                            ell->children().push_back(before);
                            ell->children().push_back(after);
                            dict.add(head, i);
                            dict.add(rest, ell);

                            // in ellipse subexpression
                            ell = new SyntaxNode("...?");
                            before = new SyntaxNode("before", ell);
                            after = new SyntaxNode("after", ell);
                            found = false;

                            for (auto n : it->second->children())
                            {
                                if (eqvExpr(n, i))  found = true;
                                else if (found)     after->children().push_back(n);
                                else                before->children().push_back(n);
                            }

                            ell->children().push_back(before);
                            ell->children().push_back(after);
                            dict.add(it->first.second, ell);

                            it = ellDict.erase(it);
                            matchFound = true;
                            break;
                        }
                    }

                    if (matchFound) break;
                }

                return matchFound;
            }
            else
            {
                std::pair<std::string, std::string> key = { head, rest };
                ellDict[key] = expr;
                return true;
            }
        }
        else
        {
            ExprNode* ell = new SyntaxNode("...?");
            ExprNode* before = new SyntaxNode("before", ell);
            ExprNode* after = new SyntaxNode("after", ell);

            ell->children().push_back(before);
            ell->children().push_back(after);

            std::vector<ExprNode*> possible;
            size_t len = sexprs.size();
            size_t idx = 0;
            for (auto it = expr->children().begin(); it != expr->children().end(); ++it)
            {
                if (matchSubexpr(sexprs[idx], *it, dict))
                {
                    if (idx == len - 2)
                    {
                        after->children().insert(after->children().begin(),
                                                 std::next(it), expr->children().end());
                        dict.add(sexprs[len - 1].name, ell);
                        return true;
                    }

                    possible.push_back(*it);
                    ++idx;
                }
                else
                {
                    before->children().insert(before->children().end(),
                                              possible.begin(), possible.end());
                    possible.clear();
                    idx = 0;
                }
            }

            return false;
        }
    }
    else if (sexprs.back().type == node::ELLIPSE) // (+ ?a ?b ...)
    {
        if (sexprs.size() > expr->children().size() + 1)
            return false;  // too many args

        auto it = expr->children().begin();
        size_t end = sexprs.size() - 1;
        for (size_t i = 0; i < end; ++i)
        {
            if (!matchSubexpr(sexprs[i], *it, dict))
                return false;
            ++it;
        }

        ExprNode* ell = new SyntaxNode("...");
        ell->children().insert(ell->children().begin(), it, expr->children().end());
        for (auto e : ell->children())  e->setParent(ell);  // not necessary, good for debugging
        dict.add(sexprs[end].name, ell);
    }
    else
    {
        if (sexprs.size() != expr->children().size())
            return false;  // argc does not match

        auto it = expr->children().begin();
        for (auto e : sexprs)
        {
            if (!matchSubexpr(e, *it, dict))
                return false;
            ++it;
        }
    }
    
    return true;
}

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

std::string MatchExpr::toString(const MatchExpr::node& tree) const
{
    if (tree.children.empty())    return tree.name;

    std::string accum = "(" + tree.name;
    for (auto e : tree.children)
    {
        accum += (" " + toString(e));
        if (e.type == node::REL_ELLIPSE)    accum += " ...?";
        else if (e.type == node::ELLIPSE)   accum += " ...";
            
    }

    return accum + ")";
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
            if (tokens[i + 1] != ")")          return false; // this token may only appear in the tail
            if (ellipseUsed[parenCount])       return false;
            ellipseUsed[parenCount] = true;
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

bool matchExprToken(const std::string& token, ExprNode* expr,
                    std::map<std::string, ExprNode*>& matchDict,
                    std::map<std::vector<std::string>, ExprNode*>& unknownDict)
{
    if (token.front() == '?')   
    {
        if (matchDict.find(token) == matchDict.end())
        {
            matchDict[token] = expr;
            auto it = unknownDict.begin(); 
            while (it != unknownDict.end())
            {
                const std::string& key = it->first[0];
                const std::string& rest = it->first[1];
                if (token == key)
                {
                    ExprNode* ell = new SyntaxNode("...?");
                    ExprNode* before = new SyntaxNode("before", ell);
                    ExprNode* after = new SyntaxNode("after", ell);

                    ell->children().push_back(before);
                    ell->children().push_back(after);

                    bool found = false;
                    for (auto n : it->second->children())
                    {
                        if (eqvExpr(n, expr))   found = true;
                        else if (found)         after->children().push_back(n);
                        else                    before->children().push_back(n);
                    }

                    matchDict[rest] = ell;
                    delete it->second;
                    it = unknownDict.erase(it);
                }
                else
                {
                    ++it;
                }    
            }

            return true;
        }

        return eqvExpr(matchDict.at(token), expr);   
    }

    if (matchDict.find(token) == matchDict.end())
        matchDict[token] = expr;
    return token == expr->toString();
}

bool matchExprHelper(const std::vector<std::string> tokens, ExprNode* expr,
                     std::map<std::string, ExprNode*>& matchDict,
                     std::map<std::vector<std::string>, ExprNode*>& unknownDict)
{
    if (tokens.size() == 1)
        return matchExprToken(tokens.front(), expr, matchDict, unknownDict);

    std::vector<std::vector<std::string>> sexprs;
    auto it = std::next(tokens.begin());
    auto end = std::prev(tokens.end());
    
    while (it != end)
    {
        std::vector<std::string> s = extractMatchSubexpr(tokens, it);
        it = std::next(it, s.size());
        sexprs.push_back(s);
    }

    if (sexprs[0].size() != 1 || !matchExprToken(sexprs[0][0], expr, matchDict, unknownDict))
        return false;   // operator did not match
    
    auto it2 = expr->children().begin();
    size_t argc = 0;
    if (sexprs.back().front() == "...?")
    {
        // unknown relative ellipse match (+ ?a ?b ...?)   
        // what is b?
        if (sexprs[1].front().front() == '?' && sexprs.size() == 4 &&
            matchDict.find(sexprs[1].front()) == matchDict.end())
        {
            bool isUnknown = false;
            const std::string& head = sexprs[1].front();
            const std::string& rest = sexprs[2].front();
            auto it = unknownDict.begin();
            
            while (it != unknownDict.end())
            {
                if (it->first[0] == sexprs[1].front())
                {
                    for (auto i : expr->children())
                    {
                        bool loop = true;
                        for (auto j : it->second->children())
                        {
                            if (eqvExpr(i, j))
                            {
                                // this subexpression
                                ExprNode* ell = new SyntaxNode("...?");
                                ExprNode* before = new SyntaxNode("before", ell);
                                ExprNode* after = new SyntaxNode("after", ell);
                                bool found = false;

                                for (auto n : expr->children())
                                {
                                    if (eqvExpr(n, i))   found = true;
                                    else if (found)      after->children().push_back(n);
                                    else                 before->children().push_back(n);
                                }

                                ell->children().push_back(before);
                                ell->children().push_back(after);
                                matchDict[head] = i;
                                matchDict[rest] = ell;

                                // unknown subexpression
                                ell = new SyntaxNode("...?");
                                before = new SyntaxNode("before", ell);
                                after = new SyntaxNode("after", ell);
                                found = false;

                                for (auto n : it->second->children())
                                {
                                    if (eqvExpr(n, i))  found = true;
                                    else if (found)     after->children().push_back(n);
                                    else                before->children().push_back(n);
                                }

                                ell->children().push_back(before);
                                ell->children().push_back(after);
                                matchDict[it->first[1]] = ell;

                                delete it->second;
                                it = unknownDict.erase(it);
                                loop = false;
                                break;
                            }
                        }

                        if (!loop) break;
                    }

                    isUnknown = true;
                    break;
                }
                else
                {
                    ++it;
                }    
            }
            
            if (!isUnknown)
            {
                std::vector<std::string> key;   // must be variable followed by an ellipse
                key.push_back(head);
                key.push_back(rest);

                ExprNode* ell = new SyntaxNode("...");
                ell->children().insert(ell->children().begin(), 
                                       expr->children().begin(),
                                       expr->children().end());
                unknownDict[key] = ell;
                // Any match expr w/ a relative ellipse and a subexpression variable
                // must be of the form (op ?x ?y ...?), assume true
                return true;
            }
        }
        else // regular match expressions
        {
            ExprNode* ell = new SyntaxNode("...?");
            ExprNode* before = new SyntaxNode("before", ell);
            ExprNode* after = new SyntaxNode("after", ell);

            ell->children().push_back(before);
            ell->children().push_back(after);

            size_t len = expr->children().size();
            size_t needed = sexprs.size() - 3;
            size_t check = len - needed;
            for (size_t idx = 1; argc < check && (idx - 1) < needed; ++argc, ++it2)
            {
                if (matchExprHelper(sexprs[idx], *it2, matchDict, unknownDict))
                {
                    if (idx == needed)
                    {
                        after->children().insert(after->children().begin(),
                                                 std::next(it2),
                                                 expr->children().end());
                        matchDict[sexprs[idx + 1].front()] = ell;
                        return true;
                    }

                    ++idx;
                }
                else
                {
                    before->children().push_back(*it2);
                }
            }

            before->children().clear();
            freeExpression(ell);
            return false;
        }
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

                ExprNode* ell = new SyntaxNode("...");
                for (size_t j = 0; j < inlist; ++j, ++it2)
                {
                    ExprNode* t = *it2;
                    t->setParent(ell);
                    ell->children().push_back(t);
                }

                matchDict[sexprs[i].front()] = ell;
                argc += (inlist - 1);
                it2 = std::next(it2, (inlist - 1));
                ++i;
            }
            else
            {
                if (!matchExprHelper(sexprs[i], *it2, matchDict, unknownDict))
                    return false;
            }
        }

        if (argc != expr->children().size()) // too few match arguments
            return false;
    }

    return true;
}

void clearMatchDict(std::map<std::string, ExprNode*>& matchDict)
{
    for (auto e : matchDict) // clear match dict
    {
        if (e.second->toString() == "...?")
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

    matchDict.clear();
}

bool matchExpr(const std::string& match, ExprNode* expr,
               std::map<std::string, ExprNode*>& matchDict)
{
    std::vector<std::string> tokens = tokenizeMatchString(match);
    std::map<std::vector<std::string>, ExprNode*> unknownDict;
    bool res = matchExprHelper(tokens, expr, matchDict, unknownDict);

    if (!unknownDict.empty())
        res = false;

    if (!res) // on failure
    {
        clearMatchDict(matchDict);
        for (auto e : unknownDict) // clear unknown dict
        {
            if (e.second->toString() == "...")
                delete e.second;
        }
    }

    return res;
}

bool matchExpr(const std::string& match, ExprNode* expr)
{
    std::map<std::string, ExprNode*> matchDict;
    bool res = matchExpr(match, expr, matchDict);
    clearMatchDict(matchDict);
    return res;
}

ExprNode* buildTreeHelper(const std::vector<std::string> tokens,
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

    std::vector<std::vector<std::string>> sexprs;
    auto it = std::next(tokens.begin());
    auto end = std::prev(tokens.end());

    while (it != end)
    {
        std::vector<std::string> s = extractMatchSubexpr(tokens, it);
        it = std::next(it, s.size());
        sexprs.push_back(s);
    }
    
    ExprNode* op = buildTreeHelper(sexprs[0], matchDict);
    op->setParent(nullptr);
    bool ellipse = sexprs.back().front() == "...?";
    
    if (ellipse)
    {
        ExprNode* ell = matchDict.at(sexprs[sexprs.size() - 2].front());
        for (auto e : ell->children().front()->children())
        {
            ExprNode* t = copyOf(e);
            t->setParent(op);
            op->children().push_back(t);
        }
    }

    size_t lim = (ellipse ? (sexprs.size() - 2) : sexprs.size());
    for (size_t i = 1; i < lim; ++i)
    {
        ExprNode* sub = buildTreeHelper(sexprs[i], matchDict);
        if (sub->toString() == "...") // ellipse match
        {
            if (i + 1 == sexprs.size() || sexprs[i + 1].front() != "...")
            {
                gErrorManager.log("Expected \"...\" after \"" + sexprs[i].front() + "\" in the match expression",
                                    ErrorManager::FATAL);
                return op;
            }

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
        ExprNode* ell = matchDict.at(sexprs[sexprs.size() - 2].front());
        for (auto e : ell->children().back()->children())
        {
            ExprNode* t = copyOf(e);
            t->setParent(op);
            op->children().push_back(t);
        }
    }

    return op;
}

ExprNode* buildTree(const std::string& match, const std::map<std::string, ExprNode*>& matchDict)
{
    std::vector<std::string> tokens = tokenizeMatchString(match);
    return buildTreeHelper(tokens, matchDict);
}

ExprNode* applyMatchTransform(const std::string& input, const std::string& output, ExprNode* expr)
{
    std::map<std::string, ExprNode*> matchDict;
    if (matchExpr(input, expr, matchDict))
    {
        ExprNode* ret = buildTree(output, matchDict);
        clearMatchDict(matchDict);
        freeExpression(expr);
        return ret;
    }
    else
    {
        clearMatchDict(matchDict);
    }

    return expr;
}

// ** Unique Transform Matcher ** //

UniqueExprTransformer::UniqueExprTransformer()   // default constructor
{
    mSuccess = false;
}

void UniqueExprTransformer::add(const std::string& input, const std::string& output)
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

void UniqueExprTransformer::clear()
{
    mTransforms.clear();
    mSuccess = false;
}

ExprNode* UniqueExprTransformer::transform(ExprNode* expr)
{
    std::map<std::string, ExprNode*> matchDict;
    bool loop = true;
    mSuccess = false;

    while (loop)
    {
        loop = false;
        for (auto e : mTransforms)
        {
            if (matchExpr(e.first, expr, matchDict))   // if match, return transform
            {
                ExprNode* trans = buildTree(e.second, matchDict);
                clearMatchDict(matchDict);
                freeExpression(expr);

                expr = trans;
                mSuccess = true;
                loop = true;
            }
            else
            {
                clearMatchDict(matchDict);
            }
        }
    }

    return expr;
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

    if (!mSubexprs.empty())
        mSubexprs.clear();

    return matchExpr(match, expr, mSubexprs);
}

}