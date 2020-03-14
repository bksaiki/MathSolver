#include <iterator>
#include "expression.h"

namespace MathSolver
{

const char* OPERATOR_CHARS = "+-*/%^!,=><|";

const size_t FLATTENABLE_OP_COUNT = 3;
const std::string FLATTENABLE_OPS[FLATTENABLE_OP_COUNT] =
{
	"+",
	"-",
	"*"
};

const size_t PREDEF_FUNC_COUNT = 5;
const std::string PREDEF_FUNCTIONS[PREDEF_FUNC_COUNT] = 
{
	"exp",
	"ln",
	"sin",
	"cos",
	"tan"
};

ExpressionNode::ExpressionNode()
{
	mParent = nullptr;
	mType = VARIABLE;
	mPrecedence = 0;
	mInexact = 0.0;
}

void flattenExpr(ExpressionNode* node)
{
	bool hasGrandChildren = false;
	for (ExpressionNode* child : node->mChildren) // recursively flatten
	{
		if (child->mChildren.size() != 0)
		{
			flattenExpr(child);
			hasGrandChildren = true;
		}
	}

	if (hasGrandChildren)
	{
		for (size_t i = 0; i < FLATTENABLE_OP_COUNT; ++i)
		{
			if (node->mStr == FLATTENABLE_OPS[i])
			{	
				for (auto child = node->mChildren.begin(); child != node->mChildren.end(); ++child)
				{
					if ((*child)->mStr == FLATTENABLE_OPS[i])
					{
						node->mChildren.insert(child, (*child)->mChildren.begin(), (*child)->mChildren.end());
						delete *child;
						child = node->mChildren.erase(child--);
					}	
				}
			}
		}
	}
}

void freeExpression(ExpressionNode* node)
{
	for (ExpressionNode* child : node->mChildren)
		freeExpression(child);
	delete node;	
}

std::string toInfixString(ExpressionNode* node)
{
	if (node->mType == ExpressionNode::FUNCTION)
	{
		std::string expr = node->mStr + "(";
		if (!node->mChildren.empty())
		{
			expr += toInfixString(node->mChildren.front());
			for (std::list<ExpressionNode*>::iterator it = std::next(node->mChildren.begin()); 
				 it != node->mChildren.end(); ++it)
				expr += (", " + toInfixString(*it));
		}

		return expr + ")";
	}
	else if (node->mType == ExpressionNode::OPERATOR)
	{
		if (node->mStr == "!")
		{
			if (node->mChildren.front()->mStr == "!")
				return "(" + toInfixString(node->mChildren.front()) + ")!";
			else
				return toInfixString(node->mChildren.front()) + "!";
		}
		else if (node->mStr == "^")
		{
			return toInfixString(node->mChildren.front()) + "^" + toInfixString(node->mChildren.back());
		}
		else
		{
			std::string expr = toInfixString(node->mChildren.front());
			std::string op = (node->mStr == "**") ? "" : node->mStr;
			for (std::list<ExpressionNode*>::iterator it = std::next(node->mChildren.begin()); 
				 it != node->mChildren.end(); ++it)
				expr += (op + toInfixString(*it));
			if (node->mParent != nullptr && node->mParent->mPrecedence < node->mPrecedence)
				return "(" + expr + ")";
			else
				return expr;
		}
		
	}
	else
	{
		return node->mStr;
	}
}

std::string toPrefixString(ExpressionNode* node)
{
	if (node == nullptr)
	{
		return "<null>";
	}
	else
	{
		if (!node->mChildren.empty())
		{
			std::string expr = "(" + node->mStr;
			for (auto e : node->mChildren)
				expr += (" " + toPrefixString(e));
			return expr + ")";
		}
		else
		{
			return node->mStr;
		}
	}
}

//
//	Language constructs
//

bool isFunction(const std::string& func)
{
	for (size_t i = 0; i < PREDEF_FUNC_COUNT; ++i)
	{
		if (func == PREDEF_FUNCTIONS[i])
			return true;
	}

	return false;
}

bool isOperatorChar(char c)
{
	for (size_t i = 0; OPERATOR_CHARS[i]; ++i)
	{
		if (c == OPERATOR_CHARS[i])
			return true;
	}

	return false;
}

bool isBracket(char c)
{
	return (c == '(' || c == '{' || c == '[' ||
			c == ')' || c == '}' || c == ']');
}

int operatorPrec(const std::string& op)
{
	if (op == "^")										return 2;
	else if (op == "!")									return 3;
	else if (op == "**")								return 4;
	else if (op == "*" || op == "/" || op == "%")		return 5;
	else if (op == "+" || op == "-")					return 6;
	else if (op == ">" || op == ">=" || op == "=" ||
			 op == "<" || op == "<=")					return 7;
	else												return 0;
}

}