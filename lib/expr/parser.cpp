#include <iostream>
#include "parser.h"

namespace MathSolver
{

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

void consumeFrom(ExprNode* expr, std::list<ExprNode*>& tokens)
{
    for (auto e : expr->children())
        consumeFrom(e, tokens);
    
    for (auto itr = tokens.begin(); itr != tokens.end(); ++itr)
    {
        if ((*itr) == expr)
        {
            tokens.erase(itr);
            break;
        }
    }
}

ExprNode* parseString(const std::string& expr)
{
    std::list<ExprNode*> tokens = tokenizeStr(expr);
    if (gErrorManager.hasError())
    {
        for (auto e : tokens) 
            delete e;
        tokens.clear();
        return nullptr;
    }

    ExprNode* exprTree = parseTokens(tokens);
    if (exprTree != nullptr) consumeFrom(exprTree, tokens);
    for (auto e : tokens) delete e;
    tokens.clear();

    if (gErrorManager.hasError())
        return nullptr;
    return exprTree;
}

//
// Parse Tokens
//

bool bracketedExpr(std::list<ExprNode*>::const_iterator begin, std::list<ExprNode*>::const_iterator end)
{
    if (!(*begin)->isSyntax() || !(*end)->isSyntax() ||
        !(((SyntaxNode*)*begin)->name() == "(" || ((SyntaxNode*)*begin)->name() == "[" || ((SyntaxNode*)*begin)->name() == "{") ||
        !(((SyntaxNode*)*end)->name() == ")" || ((SyntaxNode*)*end)->name() == "]" || ((SyntaxNode*)*end)->name() == "}"))
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

ExprNode* captureDataType(std::list<ExprNode*>::const_iterator begin, std::list<ExprNode*>::const_iterator end)
{
    std::list<ExprNode*> tokens;
    tokens.insert(tokens.begin(), begin, std::next(end));
    gErrorManager.log("Unknown type: '{" + toString(tokens) + "}'", ErrorManager::ERROR);
    return nullptr;
}

ExprNode* parseTokensR(std::list<ExprNode*>::iterator begin, std::list<ExprNode*>::iterator end)
{
    if (bracketedExpr(begin, end))
    {
        if (((SyntaxNode*)*begin)->name() == "{" && ((SyntaxNode*)*end)->name() == "}")   // '{ ... } implies a specific data type is contained within
            return captureDataType(std::next(begin), std::prev(end));
        
        if (std::distance(begin, end) &&
            (*std::next(begin))->isNumber() && (*std::prev(end))->isNumber() &&
            (*std::next(begin, 2))->isSyntax() && ((SyntaxNode*)*std::next(begin, 2))->name() == ",")
        {
            Range range = { (*std::next(begin))->toString(), (*std::prev(end))->toString(), (((SyntaxNode*)*begin)->name() == "["), (((SyntaxNode*)*end)->name() == "]") };
            return new RangeNode(range);
        }
        
        return parseTokensR(std::next(begin),std::prev(end));
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
                it = it2; 
            }
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

ExprNode* parseTokens(std::list<ExprNode*>& tokens)
{
    if (gErrorManager.hasError()) // Don't try to parse if there's an error =
        return nullptr;

    ExprNode* expr = parseTokensR(tokens.begin(), std::prev(tokens.end()));
    if (expr != nullptr) consumeFrom(expr, tokens);
    for (auto e : tokens) delete e;
    tokens.clear();
    return expr;
}

//
// Tokenize string
//

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
            if (str.find('.') == std::string::npos)     node = new IntNode(str);
            else                                        node = new FloatNode(str);
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
                if (((expr[itr] != '}') && (match == "{")) || ((expr[itr] == '}') && (match != "{")))
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

std::string toString(const std::list<ExprNode*>& list)
{
    std::string ret;
	for (auto e : list) ret += e->toString();
    return ret;
}

}