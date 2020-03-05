#include "expression.h"

namespace MathSolver
{

const char* OPERATOR_CHARS = "+-*/%^!,=><|";

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

}