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
        TestModule tests(">", verbose);
		const size_t COUNT = 8;
		const std::string exprs[COUNT * 2] = 
		{ 
			"2>1",      "true",
            "1>2",      "false",
            "1>1",      "false",
            "x>2",      "x>2",
            "3>2>1",    "true",
            "3>1>2",    "false",
            "3>x>2",    "2<x<3",
            "2>x>3",    "false"
		};

		status &= evalExpr(tests, exprs, COUNT);
	}

    {
        TestModule tests("<", verbose);
		const size_t COUNT = 8;
		const std::string exprs[COUNT * 2] = 
		{ 
			"1<2",      "true",
            "2<1",      "false",
            "1<1",      "false",
            "x<2",      "x<2",
            "1<2<3",    "true",
            "1<3<2",    "false",
            "2<x<3",    "2<x<3",
            "3<x<2",    "false"
		};

		status &= evalExpr(tests, exprs, COUNT);
	}

    {
        TestModule tests(">=", verbose);
		const size_t COUNT = 8;
		const std::string exprs[COUNT * 2] = 
		{ 
			"2>=1",      "true",
            "1>=2",      "false",
            "1>=1",      "true",
            "x>=2",      "x>=2",
            "3>=2>=1",   "true",
            "3>=1>=2",   "false",
            "3>=x>=2",   "2<=x<=3",
            "2>=x>=3",   "false"
		};

		status &= evalExpr(tests, exprs, COUNT);
	}

    {
        TestModule tests("<=", verbose);
		const size_t COUNT = 8;
		const std::string exprs[COUNT * 2] = 
		{ 
			"1<=2",      "true",
            "2<=1",      "false",
            "1<=1",      "true",
            "x<=2",      "x<=2",
            "1<=2<=3",   "true",
            "1<=3<=2",   "false",
            "2<=x<=3",   "2<=x<=3",
            "3<=x<=2",   "false"
		};

		status &= evalExpr(tests, exprs, COUNT);
	}

    {
        TestModule tests("!=", verbose);
		const size_t COUNT = 4;
		const std::string exprs[COUNT * 2] = 
		{ 
			"1!=2",     "true",
            "1!=1",     "false",
            "x!=x",     "false",
            "x!=y",     "x!=y"
		};

		status &= evalExpr(tests, exprs, COUNT);
	}

	{
		TestModule tests("Mixed intervals", verbose);
		const size_t COUNT = 2;
		const std::string exprs[COUNT * 2] = 
		{ 
			"1<=x<2",		"1<=x<2",
			"1<=x<y<=2",	"1<=x and x<y and y<=2"
		};

		status &= evalExpr(tests, exprs, COUNT);
	}

    return (int)(!status);
}