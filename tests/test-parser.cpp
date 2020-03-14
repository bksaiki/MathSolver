#include <iostream>
#include <list>
#include <string>
#include "../src/test/test-common.h"
#include "../src/expression/parser.h"

using namespace MathSolver;

void printTokenList(const std::list<ExpressionNode*>& list)
{
	for (auto e : list)
		std::cout << e->mStr << " ";
	std::cout << std::endl;
}

int main()
{
	const size_t EXPR_COUNT = 4;
	std::string exprs[EXPR_COUNT] =
	{
		"(3 + 2)(5 + 6)(8 + 9)",
		"2^(3 + 2^(2 * 2))",
		"(2!)!5!",
		"tan(5, 3)"
	};

	TestModule tester("Parser");
	for (size_t i = 0; i < EXPR_COUNT; ++i)
	{
		std::cout << "Expr: " << exprs[i] << std::endl << "Tokens: ";
		std::list<ExpressionNode*> tokens = tokenizeStr(exprs[i]);
		expandTokens(tokens);
		printTokenList(tokens);
		ExpressionNode* node = parseTokens(tokens);
		flattenExpr(node);
		std::cout << "Prefix: " << toPrefixString(node) << std::endl;
		std::cout << "Infix: " << toInfixString(node) << "\n" << std::endl;
		freeExpression(node);
	}

	return 0;
}
