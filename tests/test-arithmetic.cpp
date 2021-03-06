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

	//
	// Single operations
	//

	TestModule tests("- (negate)", verbose);
	{
		const size_t COUNT = 10;
		const std::string exprs[COUNT * 2] = 
		{ 
			"-x",		"-x",
			"-(x)",		"-x",
			"x+-y",		"x-y",
			"x--y",		"x+y",
			"-x+y",     "y-x",
			"-3",		"-3",
			"-(5)",		"-5",
			"2+-6",		"-4",
			"5--9",		"14",
			"-2+6",		"4"
		};

		status &= evalExpr(tests, exprs, COUNT);
	}

	tests.reset("+");
	{
		const size_t COUNT = 6;
		const std::string exprs[COUNT * 2] = 
		{ 
			"2a+a",				"3a",
			"2x+3x+10",			"5x+10",
			"100+1000",			"1100",
			"500+600+200",		"1300",
			"5z+2z+9+6",		"7z+15",
			"2x+y+3x",			"5x+y"
		};

		status &= evalExpr(tests, exprs, COUNT);
	}

	tests.reset("-");
	{
		const size_t COUNT = 6;
		const std::string exprs[COUNT * 2] = 
		{ 
			"5a-2a",			"3a",
			"3x-2x-10",			"x-10",
			"100-1000",			"-900",
			"500-600-200",		"-300",
			"5z-2z-9-6",		"3z-15",
			"2x-y-3x",			"-x-y"
		};

		status &= evalExpr(tests, exprs, COUNT);
	}

	tests.reset("*");
	{
		const size_t COUNT = 9;
		const std::string exprs[COUNT * 2] = 
		{ 
			"100*1000",		"100000",
			"3*(a*b)",		"3*a*b",
			"1*a*1",		"a",
			"0*x*y",		"0",
			"-1*x^2",		"-x^2",
			"(a/b)*(c/d)",	"ac/bd",
			"a*(1/d)",		"a/d",
			"2*a+2*b",		"2(a+b)",
			"2a+a(b)",		"(b+2)a"
		};

		status &= evalExpr(tests, exprs, COUNT);
	}

	tests.reset("/");
	{
		const size_t COUNT = 10;
		const std::string exprs[COUNT * 2] = 
		{ 
			"5/0",				"undef",
			"a/0",				"undef",
			"(a/b)/(c/d)",		"ad/bc",
			"(a/b)/c",	 		"a/bc",
			"a/(b/c)",			"ac/b",
			"(a*b)/(b*c)",		"a/c",
			"a/(a*b*c)",		"1/bc",
			"(a*b*c)/a",		"bc",
			"(a*b)/(a*c*d)",	"b/cd",
			"(a*b*c)/(a*d)",	"bc/d"
		};

		status &= evalExpr(tests, exprs, COUNT);
	}

	tests.reset("Mixed 4-function");
	{
		const size_t COUNT = 4;
		const std::string exprs[COUNT * 2] = 
		{ 
			"a+b/c*d",			"a+bd/c",
			"-a+b*c+d", 		"b*c-a+d",
			"(3a+5a)/5a",		"8/5",
			"2a/(15*a*b)",		"2/15b"
		};

		status &= evalExpr(tests, exprs, COUNT);
	}

	tests.reset("%");
	{
		const size_t COUNT = 10;
		const std::string exprs[COUNT * 2] = 
		{ 
			"10%3",				"1",
			"10%-3",			"1",
			"-10%3",			"-1",
			"-10%-3", 			"-1",
			"(a%n)%n",			"a%n",
			"(n^2)%n",			"0",
			"((a%n)+(b%n))%n",	"(a+b)%n",
			"((a%n)(b%n))%n",	"ab%n",
			"11 mod 3",			"2",
			"a mod n",			"a mod n"
		};

		status &= evalExpr(tests, exprs, COUNT);
	}

	tests.reset("^");
	{
		const size_t COUNT = 5;
		const std::string exprs[COUNT * 2] = 
		{ 
			"2^5",		"32",
			"3^4",		"81",
			"2^-5",		"1/32",
			"3^-4",		"1/81",
			"(a^b)^c",	"a^(bc)"
		};

		status &= evalExpr(tests, exprs, COUNT);
	}

	tests.reset("*,^ mixed");
	{
		const size_t COUNT = 8;
		const std::string exprs[COUNT * 2] = 
		{ 
			"x*x",		"x^2",
			"x*x^2",	"x^3",
			"x^2*x",	"x^3",
			"x^2*x^2",	"x^4",
			"x^5/x",	"x^4",
			"x/x^5",	"1/x^4",
			"x^5/x^3",	"x^2",
			"x^3/x^5", 	"x^-2"
		};

		status &= evalExpr(tests, exprs, COUNT);
	}

	tests.reset("!");
	{
		const size_t COUNT = 4;
		const std::string exprs[COUNT * 2] = 
		{ 
			"10!",		"3628800",
			"4!",		"24",
			"7!",		"5040",
			"5!",		"120"
		};

		status &= evalExpr(tests, exprs, COUNT);
	}

	return (int)!status;
}
