#include "expression.h"

namespace MathSolver
{

const char* OPERATOR_CHARS = "+-*/%^!,=><|";

ExpressionNode::ExpressionNode()
{
	mParent = nullptr;
	mType = VARIABLE;
	mPrecedence = 0;
	mInexact = 0.0;
}

bool isOperator(char c)
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
	else if (op == "*" || op == "/" || op == "%")		return 5;
	else if (op == "+" || op == "-")					return 6;
	else if (op == ">" || op == ">=" || op == "=" ||
			 op == "<" || op == "<=")					return 7;
	else												return 0;
}

}