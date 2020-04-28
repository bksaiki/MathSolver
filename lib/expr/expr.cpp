#include "expr.h"

namespace MathSolver
{

const char* OPERATOR_CHARS = "+-*/%^!,=><|";

const size_t FLATTENABLE_OP_COUNT = 4;
const std::string FLATTENABLE_OPS[FLATTENABLE_OP_COUNT] = { "+", "-", "*", "**" };

const size_t PREDEF_FUNC_COUNT = 5;
const std::string PREDEF_FUNCTIONS[PREDEF_FUNC_COUNT] = 
{
	"exp", "log",
	"sin", "cos", "tan"
};

const size_t OPERATOR_COUNT = 14;
const std::string OPERATORS[OPERATOR_COUNT] = 
{
	"+", "-", "*", "/", "%",
    "-*", "**"
	"^", "!",
	">", "<", ">=", "<=", "="
};

ExprNode::ExprNode()
{
	parent = nullptr;
	type = VARIABLE;
	prec = 0;
	inexact = 0.0;
}

void assignExprNode(ExprNode* dest, ExprNode* src)
{
	if (dest != nullptr && src != nullptr)
	{		
		dest->children = src->children;
		dest->type = src->type;
		dest->prec = src->prec;
		dest->str = src->str;
		dest->exact = src->exact;
		dest->inexact = src->inexact;

		for (ExprNode* child : dest->children)
			child->parent = dest;
	}
}

ExprNode* copyOf(ExprNode* expr)
{
	ExprNode* cp = new ExprNode();
	cp->type = expr->type;
	cp->prec = expr->prec;
	cp->str = expr->str;
	cp->exact = expr->exact;
	cp->inexact = expr->inexact;

	for (ExprNode* child : expr->children)
		cp->children.push_back(copyOf(child));
	return cp;
}

bool equivExpression(ExprNode* a, ExprNode* b)
{
	if (a->type == b->type)
	{
		if (a->type == ExprNode::INTEGER)
			return (a->exact == b->exact);
		else if (a->type == ExprNode::FLOAT)
			return (a->inexact == b->inexact);
		else if (a->type == ExprNode::CONSTANT ||
				 a->type == ExprNode::CONSTANT)	
			return (a->str == b->str);
		// Operator, function
		if (a->str == b->str && a->children.size() == b->children.size())
		{
			auto a_it = a->children.begin();
			auto b_it = b->children.begin();
			for (; a_it != a->children.end(); ++a_it, ++b_it)
			{
				if (!equivExpression(*a_it, *b_it))
					return false;
			}

			return true;
		}
	}

	return false;
}

void flattenExpr(ExprNode* expr)
{
	bool hasGrandChildren = false;
	for (ExprNode* child : expr->children) // recursively flatten
	{
		if (child->children.size() != 0)
		{
			flattenExpr(child);
			hasGrandChildren = true;
		}
	}

	if (hasGrandChildren)
	{
		for (size_t i = 0; i < FLATTENABLE_OP_COUNT; ++i)
		{
			if (expr->str == FLATTENABLE_OPS[i])
			{	
				for (auto child = expr->children.begin(); child != expr->children.end(); ++child)
				{
					if ((*child)->str == FLATTENABLE_OPS[i])
					{
						expr->children.insert(child, (*child)->children.begin(), (*child)->children.end());
						delete *child;
						child = expr->children.erase(child--);
					}	
				}
			}
		}
	}
}

void freeExpression(ExprNode* expr)
{
	for (ExprNode* child : expr->children)
		freeExpression(child);
	delete expr;	
}

bool isNumerical(ExprNode* expr)
{
	if (expr->type == ExprNode::FUNCTION || expr->type == ExprNode::OPERATOR)
	{
		for (ExprNode* child : expr->children)
		{
			if (!isNumber(child))
				return false;
		}

		return true;
	}

	return isNumber(expr);
}

bool isNumber(ExprNode* node)
{
	return (node->type == ExprNode::INTEGER || node->type == ExprNode::FLOAT);
}

bool isValue(ExprNode* node)
{
	return (node->type == ExprNode::INTEGER || node->type == ExprNode::FLOAT ||
			node->type == ExprNode::CONSTANT || node->type == ExprNode::VARIABLE);
}

std::string toInfixString(ExprNode* expr)
{
	if (expr->type == ExprNode::FUNCTION)
	{
		std::string sub = expr->str + "(";
		if (!expr->children.empty())
		{
			sub += toInfixString(expr->children.front());
			for (auto it = std::next(expr->children.begin()); it != expr->children.end(); ++it)
				sub += (", " + toInfixString(*it));
		}

		return sub + ")";
	}
	else if (expr->type == ExprNode::OPERATOR)
	{
		if (expr->str == "!")
		{
			if (expr->children.front()->str == "!")
				return "(" + toInfixString(expr->children.front()) + ")!";
			else
				return toInfixString(expr->children.front()) + "!";
		}
		else if (expr->str == "^")
		{
			return toInfixString(expr->children.front()) + "^" + toInfixString(expr->children.back());
		}
		else if (expr->str == "-*" && expr->children.size() == 1)
		{
			return "-" + toInfixString(expr->children.front());
		}
		else
		{
			std::string sub = toInfixString(expr->children.front());
			std::string op = (expr->str == "**") ? "" : (expr->str == "-*"? "-" : expr->str) ;
			for (auto it = std::next(expr->children.begin()); it != expr->children.end(); ++it)
				sub += (op + toInfixString(*it));
			if (expr->parent != nullptr && expr->parent->prec < expr->prec)
				return "(" + sub + ")";
			else
				return sub;
		}
		
	}
	else if (expr->type == ExprNode::INTEGER)
	{
		return expr->exact.toString();
	}
	else if (expr->type == ExprNode::FLOAT)
	{
		return std::to_string(expr->inexact);
	}
	else // constant, variable
	{
		return expr->str;
	}
}

std::string toPrefixString(ExprNode* expr)
{
	if (expr == nullptr)
	{
		return "<null>";
	}
	else
	{
		if (!expr->children.empty())
		{
			std::string sub = "(" + expr->str;
			for (auto e : expr->children)
				sub += (" " + toPrefixString(e));
			return sub + ")";
		}
		else
		{
			if (expr->type == ExprNode::INTEGER) 	return expr->exact.toString();
			else if (expr->type == ExprNode::FLOAT)	return std::to_string(expr->inexact);
			else 											return expr->str;
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

bool isOperator(const std::string& op)
{
    for (size_t i = 0; i < OPERATOR_COUNT; ++i)
    {
        if (op == OPERATORS[i])
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

size_t nodeCount(ExprNode* expr)
{
	size_t c = 1;
	if (expr->type == ExprNode::OPERATOR || expr->type == ExprNode::FUNCTION)
	{
		for (ExprNode* child : expr->children)
			c += nodeCount(child);	
	}

	return c;
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