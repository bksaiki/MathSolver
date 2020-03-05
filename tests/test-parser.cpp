#include <iostream>
#include <list>
#include <string>
#include "../src/test/test-common.h"
#include "../src/expression/parser.h"

using namespace MathSolver;

int main()
{
	const size_t EXPR_COUNT = 5;
	std::string exprs[EXPR_COUNT] =
	{
		"-3.52 + 5.1",
		"PI * 5 + 2",
		"Integral[1, 5](x)",
		"(2 * 5) + (9 ^ 6) * exp(5)",
		"{x | x >= 0}",
	};

	TestModule tester("Parser");
	for (size_t i = 0; i < EXPR_COUNT; ++i)
	{
		std::cout << "Expr: " << exprs[i] << " -> ";
		std::list<std::string> tokens = tokenizeStr(exprs[i]);

		for (auto t : tokens)
			std::cout << "\"" << t << "\" ";
		std::cout << std::endl;
	}

	return 0;
}
