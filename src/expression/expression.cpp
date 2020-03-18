#include <iterator>
#include "expression.h"

namespace MathSolver
{

const char* OPERATOR_CHARS = "+-*/%^!,=><|";

const size_t FLATTENABLE_OP_COUNT = 4;
const std::string FLATTENABLE_OPS[FLATTENABLE_OP_COUNT] =
{
	"+",
	"-",
	"*",
	"**"
};

const size_t PREDEF_FUNC_COUNT = 5;
const std::string PREDEF_FUNCTIONS[PREDEF_FUNC_COUNT] = 
{
	"exp",
	"log",
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

void assignExprNode(ExpressionNode* dest, ExpressionNode* src)
{
	if (dest != nullptr && src != nullptr)
	{
		dest->mChildren = src->mChildren;
		dest->mType = src->mType;
		dest->mPrecedence = src->mPrecedence;
		dest->mStr = src->mStr;
		dest->mExact = src->mExact;
		dest->mInexact = src->mInexact;
	}
}

void flattenExpr(ExpressionNode* expr)
{
	bool hasGrandChildren = false;
	for (ExpressionNode* child : expr->mChildren) // recursively flatten
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
			if (expr->mStr == FLATTENABLE_OPS[i])
			{	
				for (auto child = expr->mChildren.begin(); child != expr->mChildren.end(); ++child)
				{
					if ((*child)->mStr == FLATTENABLE_OPS[i])
					{
						expr->mChildren.insert(child, (*child)->mChildren.begin(), (*child)->mChildren.end());
						delete *child;
						child = expr->mChildren.erase(child--);
					}	
				}
			}
		}
	}
}

void freeExpression(ExpressionNode* expr)
{
	for (ExpressionNode* child : expr->mChildren)
		freeExpression(child);
	delete expr;	
}

bool isNumerical(ExpressionNode* expr)
{
	if (expr->mType == ExpressionNode::FUNCTION || expr->mType == ExpressionNode::OPERATOR)
	{
		for (ExpressionNode* child : expr->mChildren)
		{
			if (!isNumber(child))
				return false;
		}

		return true;
	}

	return isNumber(expr);
}

bool isNumber(ExpressionNode* node)
{
	return (node->mType == ExpressionNode::INTEGER || node->mType == ExpressionNode::FLOAT);
}

bool isValue(ExpressionNode* node)
{
	return (node->mType == ExpressionNode::INTEGER || node->mType == ExpressionNode::FLOAT ||
			node->mType == ExpressionNode::CONSTANT || node->mType == ExpressionNode::VARIABLE);
}

std::string toInfixString(ExpressionNode* expr)
{
	if (expr->mType == ExpressionNode::FUNCTION)
	{
		std::string sub = expr->mStr + "(";
		if (!expr->mChildren.empty())
		{
			sub += toInfixString(expr->mChildren.front());
			for (std::list<ExpressionNode*>::iterator it = std::next(expr->mChildren.begin()); 
				 it != expr->mChildren.end(); ++it)
				sub += (", " + toInfixString(*it));
		}

		return sub + ")";
	}
	else if (expr->mType == ExpressionNode::OPERATOR)
	{
		if (expr->mStr == "!")
		{
			if (expr->mChildren.front()->mStr == "!")
				return "(" + toInfixString(expr->mChildren.front()) + ")!";
			else
				return toInfixString(expr->mChildren.front()) + "!";
		}
		else if (expr->mStr == "^")
		{
			return toInfixString(expr->mChildren.front()) + "^" + toInfixString(expr->mChildren.back());
		}
		else
		{
			std::string sub = toInfixString(expr->mChildren.front());
			std::string op = (expr->mStr == "**") ? "" : expr->mStr;
			for (std::list<ExpressionNode*>::iterator it = std::next(expr->mChildren.begin()); 
				 it != expr->mChildren.end(); ++it)
				sub += (op + toInfixString(*it));
			if (expr->mParent != nullptr && expr->mParent->mPrecedence < expr->mPrecedence)
				return "(" + sub + ")";
			else
				return sub;
		}
		
	}
	else if (expr->mType == ExpressionNode::INTEGER)
	{
		return expr->mExact.toString();
	}
	else if (expr->mType == ExpressionNode::FLOAT)
	{
		return std::to_string(expr->mInexact);
	}
	else // constant, variable
	{
		return expr->mStr;
	}
}

std::string toPrefixString(ExpressionNode* expr)
{
	if (expr == nullptr)
	{
		return "<null>";
	}
	else
	{
		if (!expr->mChildren.empty())
		{
			std::string sub = "(" + expr->mStr;
			for (auto e : expr->mChildren)
				sub += (" " + toPrefixString(e));
			return sub + ")";
		}
		else
		{
			if (expr->mType == ExpressionNode::INTEGER) 	return expr->mExact.toString();
			else if (expr->mType == ExpressionNode::FLOAT)	return std::to_string(expr->mInexact);
			else 											return expr->mStr;
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
	if (op == "-*")										return 2;
	else if (op == "^")									return 3;
	else if (op == "!")									return 4;
	else if (op == "**")								return 5;
	else if (op == "*" || op == "/" || op == "%")		return 6;
	else if (op == "+" || op == "-")					return 7;
	else if (op == ">" || op == ">=" || op == "=" ||
			 op == "<" || op == "<=")					return 8;
	else												return 0;
}

}