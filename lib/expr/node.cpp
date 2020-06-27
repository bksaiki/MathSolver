#include "node.h"

namespace MathSolver
{

ExprNode::~ExprNode()
{
    // do nothing
}

SyntaxNode::SyntaxNode(const std::string& data, ExprNode* parent)
{
    mData = data;
    mParent = parent;
    mPrec = 0;
}

OpNode::OpNode(const std::string& data, ExprNode* parent)
{
    mData = data;
    mParent = parent;
    mPrec = opPrec(mData);
}

void OpNode::setName(const std::string& str)
{
    mData = str;
    mPrec = opPrec(str);
}

FuncNode::FuncNode(const std::string& data, ExprNode* parent)
{
    mData = data;
    mParent = parent;
    mPrec = 1;
}

VarNode::VarNode(const std::string& data, ExprNode* parent)
{
    mData = data;
    mParent = parent;
    mPrec = 0;
}

ConstNode::ConstNode(const std::string& data, ExprNode* parent)
{
    mData = data;
    mParent = parent;
    mPrec = 0;
}

IntNode::IntNode(const Integer& data, ExprNode* parent)
{
    mData = data;
    mParent = parent;
    mPrec = 0;
}

FloatNode::FloatNode(const Float& data, ExprNode* parent)
{
    mData = data;
    mParent = parent;
    mPrec = 0;
}

FloatNode::FloatNode(Float&& data, ExprNode* parent)
{
    mData = data;
    mParent = parent;
    mPrec = 0;
}

RangeNode::RangeNode(const Range& data, ExprNode* parent)
{
    mData = data;
    mParent = parent;
    mPrec = 0;
}
    
RangeNode::RangeNode(Range&& data, ExprNode* parent)
{
    mData = data;
    mParent = parent;
    mPrec = 0;
}

BoolNode::BoolNode(bool data, ExprNode* parent)
{
    mData = data;
    mParent = parent;
    mPrec = 0;
}

bool isZeroNode(ExprNode* expr)
{
    return ((expr->type() == ExprNode::INTEGER && ((IntNode*)expr)->value().isZero()) || 
            (expr->type() == ExprNode::FLOAT && ((FloatNode*)expr)->value().isZero()));
}

bool isIdentityNode(ExprNode* expr)
{
    return ((expr->type() == ExprNode::INTEGER && ((IntNode*)expr)->value() == Integer(1)) || 
            (expr->type() == ExprNode::FLOAT && ((FloatNode*)expr)->value() == Float("1.0")));
}


const char* OPERATOR_CHARS = "+-*/%^!=><|";
const char* SYNTAX_CHARS = "(){}[]|,";

const size_t PREDEF_FUNC_COUNT = 5;
const std::string PREDEF_FUNCTIONS[PREDEF_FUNC_COUNT] = 
{
	"exp", "log",
	"sin", "cos", "tan"
};

const size_t OPERATOR_COUNT = 17;
const std::string OPERATORS[OPERATOR_COUNT] = 
{
	"+", "-", "*", "/", "%", "mod",
    "-*", "**",
	"^", "!",
	">", "<", ">=", "<=", "=",
    "or", "and"
};

bool isOperator(char c)
{
	for (size_t i = 0; OPERATOR_CHARS[i]; ++i)
	{
		if (c == OPERATOR_CHARS[i])
			return true;
	}

	return false;
}

bool isSyntax(char c)
{
	for (size_t i = 0; SYNTAX_CHARS[i]; ++i)
	{
		if (c == SYNTAX_CHARS[i])
			return true;
	}

	return false;
}

bool isBracket(char c)
{
	return (c == '(' || c == '{' || c == '[' ||
			c == ')' || c == '}' || c == ']');
}

bool isClosingBracket(const std::string& str)
{
    return (str == ")" || str == "}" || str == "]");
}

bool isOpeningBracket(const std::string& str)
{
    return (str == "(" || str == "{" || str == "[");
}

bool isFunction(const std::string& func)
{
	for (size_t i = 0; i < PREDEF_FUNC_COUNT; ++i)
	{
		if (func == PREDEF_FUNCTIONS[i])
			return true;
	}

	return false;
}

bool isOperator(const std::string& op)
{
    for (size_t i = 0; i < OPERATOR_COUNT; ++i)
    {
        if (op == OPERATORS[i])
            return true;
    }

    return false;
}

int opPrec(const std::string& str)
{
    if (str == "-*")										return 2;
	else if (str == "^")									return 3;
	else if (str == "!")									return 4;
	else if (str == "**")								    return 5;
	else if (str == "*" || str == "/" || str == "%")		return 6;
    else if (str == "mod")								    return 6;
	else if (str == "+" || str == "-")					    return 7;
	else if (str == ">" || str == ">=" || str == "=" ||
			 str == "<" || str == "<=")					    return 8;
    else if (str == "or" || str == "and")                   return 9;
	else												    return 0;
}

ExprNode* moveNode(ExprNode* dest, ExprNode* src)
{
    if (dest != nullptr && src != nullptr && dest != src)
	{
        if (dest->parent() != nullptr)
        {
            for (auto e = dest->parent()->children().begin(); e != dest->parent()->children().end(); ++e)
            {
                if (*e == dest)
                {
                    e = dest->parent()->children().insert(e, src);
                    dest->parent()->children().erase(e);
                    break;
                }
            }
        }

		src->setParent(dest->parent()); 
        delete dest;
	}

    return src;
}

std::list<ExprNode*>::iterator replaceChild(ExprNode* parent, ExprNode* src, std::list<ExprNode*>::iterator pos, bool remove)
{
    if (parent != nullptr && src != *pos)
    {       
        if (remove) delete *pos;
        auto it = parent->children().insert(pos, src);      
        parent->children().erase(pos);
        src->setParent(parent);
        return it;
    }

    return pos;
}

void releaseChild(ExprNode* node)
{
    if (node->parent() != nullptr)
    {
        auto e = node->parent()->children().begin();
        while (e != node->parent()->children().end())
        {
            if (*e == node) 
            {
                e = node->parent()->children().erase(e);
                break;
            }
            else           
            {
                ++e;
            }
        }
    }
}

}