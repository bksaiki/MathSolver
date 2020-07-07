#include <iostream>
#include <list>
#include <string>
#include "../lib/mathsolver.h"
#include "../lib/test/test-common.h"

using namespace MathSolver;

bool evalExpr(TestModule& tester, const std::string exprs[], size_t count)
{
	for (size_t i = 0; i < count; ++i)
	{
		ExprNode* expr = parseString(exprs[2 * i]);
		flattenExpr(expr);
		expr = evaluateExpr(expr);
		tester.runTest(toInfixString(expr), exprs[2 * i + 1]);
		freeExpression(expr);
	}

	std::cout << tester.result() << std::endl;
	return tester.status();
}

int main()
{
	bool status = true;
	bool verbose = false;

	{
        TestModule tests("not", verbose);
		const size_t COUNT = 2;
		const std::string exprs[COUNT * 2] = 
		{ 
			"not false",		"true",
			"not true",			"false"
		};

		status &= evalExpr(tests, exprs, COUNT);
	}

	{
        TestModule tests("or", verbose);
		const size_t COUNT = 4;
		const std::string exprs[COUNT * 2] = 
		{ 
			"false or false",			"false",
			"true or false",			"true",
			"false or true",			"true",
			"true or true",				"true"
		};

		status &= evalExpr(tests, exprs, COUNT);
	}

	{
        TestModule tests("xor", verbose);
		const size_t COUNT = 4;
		const std::string exprs[COUNT * 2] = 
		{ 
			"false xor false",			"false",
			"true xor false",			"true",
			"false xor true",			"true",
			"true xor true",			"false"
		};

		status &= evalExpr(tests, exprs, COUNT);
	}

	{
        TestModule tests("and", verbose);
		const size_t COUNT = 4;
		const std::string exprs[COUNT * 2] = 
		{ 	
			"false and false",			"false",
			"true and false",			"false",
			"false and true",			"false",
			"true and true",			"true"
		};

		status &= evalExpr(tests, exprs, COUNT);
	}

    return (int)(!status);
}