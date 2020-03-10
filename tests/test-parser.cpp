#include <iostream>
#include <list>
#include <string>
#include "../src/test/test-common.h"
#include "../src/expression/parser.h"

using namespace MathSolver;


void printExpressionTree(ExpressionNode* tree)
{
	
	if (!tree->mChildren.empty())
	{
		std::cout << "(" << tree->mStr << " ";
		for (auto e : tree->mChildren)
			printExpressionTree(e);
		std::cout << ") ";
	}
	else
	{
		std::cout << tree->mStr << " ";
	}
}

int main()
{
	const size_t EXPR_COUNT = 5;
	std::string exprs[EXPR_COUNT] =
	{
		"-3.52 + 5.1",
		"PI * 5 + 2",
		"(2 * 5) + (9 ^ 6)",
		"3/2 <= x < 9*6",
		"3^2!5!"
	};

	TestModule tester("Parser");
	for (size_t i = 0; i < EXPR_COUNT; ++i)
	{
		std::cout << "Expr: " << exprs[i] << " -> ";
		std::list<ExpressionNode*> tokens = tokenizeStr(exprs[i]);
		ExpressionNode* node = buildExpression(tokens);
		printExpressionTree(node);
		std::cout << std::endl;
	}

	return 0;
}
