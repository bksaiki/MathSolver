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
			std::cout << "( " << tree->mStr << " ";
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
	const size_t EXPR_COUNT = 2;
	std::string exprs[EXPR_COUNT] =
	{
		"y=a*x^2+b*x+c",
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
