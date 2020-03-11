#include <iostream>
#include <list>
#include <string>
#include "../src/test/test-common.h"
#include "../src/expression/parser.h"

using namespace MathSolver;


void printExpressionTree(ExpressionNode* tree)
{
	if (tree == nullptr)
	{
		std::cout << "<invalid expression>";
	}
	else
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
}

void printTokenList(const std::list<ExpressionNode*>& list)
{
	for (auto e : list)
		std::cout << e->mStr << " ";
	std::cout << std::endl;
}

int main()
{
	const size_t EXPR_COUNT = 7;
	std::string exprs[EXPR_COUNT] =
	{
		"5(2)(2)(2)",
		"PI * 5 + 2",
		"(2 + 5) * (b ^ 6 - 2 * a)",
		"3/2 <= x < 9*6",
		"3^2!5!",
		"3xy",
		"y=m*x+b"
	};

	TestModule tester("Parser");
	for (size_t i = 0; i < EXPR_COUNT; ++i)
	{
		std::cout << "Expr: " << exprs[i] << std::endl << "Tokens: ";
		std::list<ExpressionNode*> tokens = tokenizeStr(exprs[i]);
		expandTokens(tokens);
		printTokenList(tokens);
		ExpressionNode* node = buildExpression(tokens);
		std::cout << "Prefix: ";
		printExpressionTree(node);
		std::cout << std::endl << std::endl;
	}

	return 0;
}
