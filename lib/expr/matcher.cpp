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

ExprNode* MatchDict::get(const std::string& id) const
{
    auto it = mDict.find(id);
    if (it != mDict.end())  return it->second;
    else                    return nullptr;
}

//
// MatchExpr
//

ExprNode* MatchExpr::create(const MatchDict& dict) const
{
    return createSubexpr(mTop, dict);
}

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
        if (tokens.front() != "(" || tokens.back() != ")")
        {
            gErrorManager.log("Expected a subexpression inside parenthesis!", ErrorManager::FATAL);
            return { "null", std::vector<node>(), nullptr, node::SINGLE, 0 };
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
                    return { "null", std::vector<node>(), nullptr, node::SINGLE, 0 };
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

        auto pred = [](const node& lhs, const node& rhs) { return lhs.depth < rhs.depth; };
        auto it = std::min_element(top.children.begin(), top.children.end(), pred);
        if (it != top.children.end())
        {
            top.depth = it->depth + 1; // min(children) + 1
            return top;
        }
        else
        {
            gErrorManager.log("Malformed match expression", ErrorManager::FATAL);
            return { "null", std::vector<node>(), nullptr, node::SINGLE, 0 };
        }
        
    }
}

ExprNode* MatchExpr::createLeaf(const node& match, const MatchDict& dict) const
{
    ExprNode* inDict = dict.get(match.name);
    if (inDict != nullptr)
    {
        if (match.name.front() == '?')
            return ((inDict->toString() == "...") ? inDict : copyOf(inDict));
        else
            return copyNode(inDict);
    }

    std::list<ExprNode*> tokens = tokenizeStr(match.name);
    return tokens.front();
}

ExprNode* MatchExpr::createSubexpr(const node& match, const MatchDict& dict) const
{
    if (match.children.empty()) // create leaf
        return createLeaf(match, dict);

    // create subexpr layer
    ExprNode* op = createLeaf(match, dict);
    const node& last = match.children.back();
    bool ellipse = (last.type == node::REL_ELLIPSE);
    op->setParent(nullptr);
    
    if (ellipse)
    {
        ExprNode* ell = dict.get(last.name);
        for (auto e : ell->children().front()->children())
        {
            ExprNode* t = copyOf(e);
            t->setParent(op);
            op->children().push_back(t);
        }
    } 

    size_t lim = (ellipse ? (match.children.size() - 1) : match.children.size());
    for (size_t i = 0; i < lim; ++i)
    {
        ExprNode* sexpr = createSubexpr(match.children[i], dict);
        if (sexpr->toString() == "...")
        {
            for (auto e : sexpr->children())
            {
                ExprNode* t = copyOf(e);
                t->setParent(op);
                op->children().push_back(t);
            }
        }
        else
        {
            sexpr->setParent(op);
            op->children().push_back(sexpr);
        }
    }

    if (ellipse)
    {
        ExprNode* ell = dict.get(last.name);
        for (auto e : ell->children().back()->children())
        {
            ExprNode* t = copyOf(e);
            t->setParent(op);
            op->children().push_back(t);
        }
    } 

    return op;
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
                    before->children().push_back(*it);
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

std::vector<std::string> MatchExpr::tokenizeMatchString(const std::string& match)
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

// ** Unique Transform Matcher ** //

UniqueExprTransformer::UniqueExprTransformer()   // default constructor
{
    mSuccess = false;
}

void UniqueExprTransformer::add(const std::string& input, const std::string& output)
{
    mTransforms.push_back({ MatchExpr(input), MatchExpr(output) });
}

void UniqueExprTransformer::clear()
{
    mTransforms.clear();
    mSuccess = false;
}

ExprNode* UniqueExprTransformer::transform(ExprNode* expr)
{
    MatchDict dict;
    bool loop = true;
    mSuccess = false;

    while (loop)
    {
        loop = false;
        for (auto e : mTransforms)
        {
            if (e.first.match(expr, dict))
            {
                ExprNode* trans = e.second.create(dict);
                dict.clear();
                freeExpression(expr);

                expr = trans;
                mSuccess = true;
                loop = true;
            }
        }
    }

    return expr;
}

ExprNode* UniqueExprMatcher::get(const std::string& key) const
{
    ExprNode* inDict = mSubexprs.get(key);
    if (inDict == nullptr)
    {
        gErrorManager.log("Could not find match subexpression key \"" + key + "\"",
                          ErrorManager::FATAL);
    }

    return inDict;
}

bool UniqueExprMatcher::match(ExprNode* expr, const std::string& match)
{
    MatchExpr mexpr(match);
    return mexpr.match(expr, mSubexprs);
}

}