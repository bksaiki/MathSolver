#include <iostream>
#include <list>
#include <string>
#include "../lib/test/test-common.h"
#include "../lib/expr/parser.h"

using namespace MathSolver;

bool parseExpr(TestModule& tester, const std::string exprs[], size_t count)
{
	for (size_t i = 0; i < count; ++i)
	{
		ExprNode* expr = parseString(exprs[2 * i]);
		flattenExpr(expr);
		tester.runTest(toPrefixString(expr), exprs[2 * i + 1]);
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

	{
		const size_t COUNT = 4;
		const std::string exprs[COUNT * 2] = 
		{ 
			"-x",		"(-* x)",
			"-(x)",		"(-* x)",
			"x+-y",		"(+ x (-* y))",
			"x--y",		"(- x (-* y))"
		};

		TestModule tests("Parser - (negate)", verbose);
		status &= parseExpr(tests, exprs, COUNT);
	}

	{
		const size_t COUNT = 4;
		const std::string exprs[COUNT * 2] = 
		{ 
			"x+y",		"(+ x y)",
			"x+y+z",	"(+ x y z)",
			"x-y",		"(- x y)",
			"x-y-z",	"(- x y z)"
		};

		TestModule tests("Parser +,-", verbose);
		status &= parseExpr(tests, exprs, COUNT);
	}

	{
		const size_t COUNT = 6;
		const std::string exprs[COUNT * 2] = 
		{ 
			"x*y",		"(* x y)",
			"x*y*z",	"(* x y z)",
			"x(y)",		"(** x y)",
			"x(y)(z)",	"(** x y z)",
			"x/y", 		"(/ x y)",
			"x%y", 		"(% x y)"
		};

		TestModule tests("Parser *,/,%", verbose);
		status &= parseExpr(tests, exprs, COUNT);
	}

	{
		const size_t COUNT = 4;
		const std::string exprs[COUNT * 2] = 
		{ 
			"x^y", 		"(^ x y)",
			"x^y^z",	"(^ x (^ y z))",
			"x!", 		"(! x)",
			"(x!)!",	"(! (! x))"
		};

		TestModule tests("Parser ^,!", verbose);
		status &= parseExpr(tests, exprs, COUNT);
	}

	{
		const size_t COUNT = 5;
		const std::string exprs[COUNT * 2] = 
		{ 
			"x=y", 		"(= x y)",
			"x>y", 		"(> x y)",
			"x<y", 		"(< x y)",
			"x>=y", 	"(>= x y)",
			"x<=y", 	"(<= x y)"
		};

		TestModule tests("Parser =,>,<,>=,<=", verbose);
		status &= parseExpr(tests, exprs, COUNT);
	}

	{
		const size_t COUNT = 5;
		const std::string exprs[COUNT * 2] = 
		{ 
			"exp x",		"(exp x)",
			"exp(x)",		"(exp x)",
			"log x",		"(log x)",
			"log(x)",		"(log x)",
			"log(b, x)",	"(log b x)"
		};

		TestModule tests("Parser exp, log", verbose);
		status &= parseExpr(tests, exprs, COUNT);
	}

	{
		const size_t COUNT = 7;
		const std::string exprs[COUNT * 2] = 
		{ 
			"sin x",		"(sin x)",
			"sin(x)",		"(sin x)",
			"cos x",		"(cos x)",
			"cos(x)",		"(cos x)",
			"tan x",		"(tan x)",
			"tan(x)",		"(tan x)",
			"tan(x, y)",	"(tan x y)"
		};

		TestModule tests("Parser sin, cos, tan", verbose);
		status &= parseExpr(tests, exprs, COUNT);
	}

	//
	//	Multiple operations
	//

	{
		const size_t COUNT = 10;
		const std::string exprs[COUNT * 2] = 
		{ 
			"m*x+b",					"(+ (* m x) b)",
			"a*x^2+b*x+c",				"(+ (* a (^ x 2)) (* b x) c)",
			"(a+b)*(x+y)",				"(* (+ a b) (+ x y))",
			"(a/b)*(c/d)",				"(* (/ a b) (/ c d))",
			"(a-b)!*c!",				"(* (! (- a b)) (! c))",
			"-(a+b)^2*(c!/d-e)",		"(* (^ (-* (+ a b)) 2) (- (/ (! c) d) e))",
			"sin(cos(tan x))",			"(sin (cos (tan x)))",
			"exp(log(x + 1))",			"(exp (log (+ x 1)))",
			"tan(x+1,y/2)",				"(tan (+ x 1) (/ y 2))",
			"log(a%b,tan(x,exp y))", 	"(log (% a b) (tan x (exp y)))"
		};

		TestModule tests("Parser +,-,*,/,^,!,exp,log,sin,cos,tan", verbose);
		status &= parseExpr(tests, exprs, COUNT);
	}

	{
		const size_t COUNT = 4;
		const std::string exprs[COUNT * 2] = 
		{ 
			"(0, 1)",	"(0, 1)",
			"(0, 1]",	"(0, 1]",
			"[0, 1)",	"[0, 1)",
			"[0, 1]",	"[0, 1]"
		};

		TestModule tests("Parser range", verbose);
		status &= parseExpr(tests, exprs, COUNT);
	}

	return (int)!status;
}
