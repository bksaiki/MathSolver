#include <iostream>
#include <list>
#include <string>
#include "../lib/test/test-common.h"
#include "../lib/expr/parser.h"

using namespace MathSolver;

int main()
{
	bool status = true;
	bool verbose = false;

	//
	// Single operations
	//

	TestModule tests("Parser - (negate)", verbose);
	{
		const size_t COUNT = 4;
		const std::string exprs[COUNT * 2] = 
		{ 
			"-x",		"(-* x)",
			"-(x)",		"(-* x)",
			"x+-y",		"(+ x (-* y))",
			"x--y",		"(- x (-* y))"
		};

		for (size_t i = 0; i < COUNT; ++i)
		{
			ExprNode* expr = parseTokens(tokenizeStr(exprs[2 * i]));
			flattenExpr(expr);
			tests.runTest(toPrefixString(expr), exprs[2 * i + 1]);
			freeExpression(expr);
		}
		std::cout << tests.result() << std::endl;
		status &= tests.status();
	}

	tests.reset("Parser +");
	{
		const size_t COUNT = 2;
		const std::string exprs[COUNT * 2] = 
		{ 
			"x+y",		"(+ x y)",
			"x+y+z",	"(+ x y z)"
		};

		for (size_t i = 0; i < COUNT; ++i)
		{
			ExprNode* expr = parseTokens(tokenizeStr(exprs[2 * i]));
			flattenExpr(expr);
			tests.runTest(toPrefixString(expr), exprs[2 * i + 1]);
			freeExpression(expr);
		}
		std::cout << tests.result() << std::endl;
		status &= tests.status();
	}

	tests.reset("Parser -");
	{
		const size_t COUNT = 2;
		const std::string exprs[COUNT * 2] = 
		{ 
			"x-y",		"(- x y)",
			"x-y-z",	"(- x y z)"
		};

		for (size_t i = 0; i < COUNT; ++i)
		{
			ExprNode* expr = parseTokens(tokenizeStr(exprs[2 * i]));
			flattenExpr(expr);
			tests.runTest(toPrefixString(expr), exprs[2 * i + 1]);
			freeExpression(expr);
		}
		std::cout << tests.result() << std::endl;
		status &= tests.status();
	}

	tests.reset("Parser *");
	{
		const size_t COUNT = 4;
		const std::string exprs[COUNT * 2] = 
		{ 
			"x*y",		"(* x y)",
			"x*y*z",	"(* x y z)",
			"x(y)",		"(** x y)",
			"x(y)(z)",	"(** x y z)"
		};

		for (size_t i = 0; i < COUNT; ++i)
		{
			ExprNode* expr = parseTokens(tokenizeStr(exprs[2 * i]));
			flattenExpr(expr);
			tests.runTest(toPrefixString(expr), exprs[2 * i + 1]);
			freeExpression(expr);
		}
		std::cout << tests.result() << std::endl;
		status &= tests.status();
	}

	tests.reset("Parser /");
	{
		const size_t COUNT = 1;
		const std::string exprs[COUNT * 2] = 
		{ 
			"x/y", 		"(/ x y)"
		};

		for (size_t i = 0; i < COUNT; ++i)
		{
			ExprNode* expr = parseTokens(tokenizeStr(exprs[2 * i]));
			flattenExpr(expr);
			tests.runTest(toPrefixString(expr), exprs[2 * i + 1]);
			freeExpression(expr);
		}
		std::cout << tests.result() << std::endl;
		status &= tests.status();
	}

	tests.reset("Parser %");
	{
		const size_t COUNT = 1;
		const std::string exprs[COUNT * 2] = 
		{ 
			"x%y", 		"(% x y)"
		};

		for (size_t i = 0; i < COUNT; ++i)
		{
			ExprNode* expr = parseTokens(tokenizeStr(exprs[2 * i]));
			flattenExpr(expr);
			tests.runTest(toPrefixString(expr), exprs[2 * i + 1]);
			freeExpression(expr);
		}
		std::cout << tests.result() << std::endl;
		status &= tests.status();
	}

	tests.reset("Parser ^");
	{
		const size_t COUNT = 2;
		const std::string exprs[COUNT * 2] = 
		{ 
			"x^y", 		"(^ x y)",
			"x^y^z",	"(^ x (^ y z))"
		};

		for (size_t i = 0; i < COUNT; ++i)
		{
			ExprNode* expr = parseTokens(tokenizeStr(exprs[2 * i]));
			flattenExpr(expr);
			tests.runTest(toPrefixString(expr), exprs[2 * i + 1]);
			freeExpression(expr);
		}
		std::cout << tests.result() << std::endl;
		status &= tests.status();
	}

	tests.reset("Parser !");
	{
		const size_t COUNT = 2;
		const std::string exprs[COUNT * 2] = 
		{ 
			"x!", 		"(! x)",
			"(x!)!",	"(! (! x))"
		};

		for (size_t i = 0; i < COUNT; ++i)
		{
			ExprNode* expr = parseTokens(tokenizeStr(exprs[2 * i]));
			flattenExpr(expr);
			tests.runTest(toPrefixString(expr), exprs[2 * i + 1]);
			freeExpression(expr);
		}
		std::cout << tests.result() << std::endl;
		status &= tests.status();
	}

	tests.reset("Parser =");
	{
		const size_t COUNT = 1;
		const std::string exprs[COUNT * 2] = 
		{ 
			"x=y", 		"(= x y)",
		};

		for (size_t i = 0; i < COUNT; ++i)
		{
			ExprNode* expr = parseTokens(tokenizeStr(exprs[2 * i]));
			flattenExpr(expr);
			tests.runTest(toPrefixString(expr), exprs[2 * i + 1]);
			freeExpression(expr);
		}
		std::cout << tests.result() << std::endl;
		status &= tests.status();
	}

	tests.reset("Parser >");
	{
		const size_t COUNT = 1;
		const std::string exprs[COUNT * 2] = 
		{ 
			"x>y", 		"(> x y)",
		};

		for (size_t i = 0; i < COUNT; ++i)
		{
			ExprNode* expr = parseTokens(tokenizeStr(exprs[2 * i]));
			flattenExpr(expr);
			tests.runTest(toPrefixString(expr), exprs[2 * i + 1]);
			freeExpression(expr);
		}
		std::cout << tests.result() << std::endl;
		status &= tests.status();
	}

	tests.reset("Parser <");
	{
		const size_t COUNT = 1;
		const std::string exprs[COUNT * 2] = 
		{ 
			"x<y", 		"(< x y)",
		};

		for (size_t i = 0; i < COUNT; ++i)
		{
			ExprNode* expr = parseTokens(tokenizeStr(exprs[2 * i]));
			flattenExpr(expr);
			tests.runTest(toPrefixString(expr), exprs[2 * i + 1]);
			freeExpression(expr);
		}
		std::cout << tests.result() << std::endl;
		status &= tests.status();
	}

	tests.reset("Parser >=");
	{
		const size_t COUNT = 1;
		const std::string exprs[COUNT * 2] = 
		{ 
			"x>=y", 	"(>= x y)",
		};

		for (size_t i = 0; i < COUNT; ++i)
		{
			ExprNode* expr = parseTokens(tokenizeStr(exprs[2 * i]));
			flattenExpr(expr);
			tests.runTest(toPrefixString(expr), exprs[2 * i + 1]);
			freeExpression(expr);
		}
		std::cout << tests.result() << std::endl;
		status &= tests.status();
	}

	tests.reset("Parser <=");
	{
		const size_t COUNT = 1;
		const std::string exprs[COUNT * 2] = 
		{ 
			"x<=y", 		"(<= x y)",
		};

		for (size_t i = 0; i < COUNT; ++i)
		{
			ExprNode* expr = parseTokens(tokenizeStr(exprs[2 * i]));
			flattenExpr(expr);
			tests.runTest(toPrefixString(expr), exprs[2 * i + 1]);
			freeExpression(expr);
		}
		std::cout << tests.result() << std::endl;
		status &= tests.status();
	}

	tests.reset("Parser exp");
	{
		const size_t COUNT = 2;
		const std::string exprs[COUNT * 2] = 
		{ 
			"exp x",		"(exp x)",
			"exp(x)",		"(exp x)"
		};

		for (size_t i = 0; i < COUNT; ++i)
		{
			ExprNode* expr = parseTokens(tokenizeStr(exprs[2 * i]));
			flattenExpr(expr);
			tests.runTest(toPrefixString(expr), exprs[2 * i + 1]);
			freeExpression(expr);
		}
		std::cout << tests.result() << std::endl;
		status &= tests.status();
	}

	tests.reset("Parser log");
	{
		const size_t COUNT = 3;
		const std::string exprs[COUNT * 2] = 
		{ 
			"log x",		"(log x)",
			"log(x)",		"(log x)",
			"log(b, x)",	"(log b x)"
		};

		for (size_t i = 0; i < COUNT; ++i)
		{
			ExprNode* expr = parseTokens(tokenizeStr(exprs[2 * i]));
			flattenExpr(expr);
			tests.runTest(toPrefixString(expr), exprs[2 * i + 1]);
			freeExpression(expr);
		}
		std::cout << tests.result() << std::endl;
		status &= tests.status();
	}

	tests.reset("Parser sin");
	{
		const size_t COUNT = 2;
		const std::string exprs[COUNT * 2] = 
		{ 
			"sin x",		"(sin x)",
			"sin(x)",		"(sin x)"
		};

		for (size_t i = 0; i < COUNT; ++i)
		{
			ExprNode* expr = parseTokens(tokenizeStr(exprs[2 * i]));
			flattenExpr(expr);
			tests.runTest(toPrefixString(expr), exprs[2 * i + 1]);
			freeExpression(expr);
		}
		std::cout << tests.result() << std::endl;
		status &= tests.status();
	}

	tests.reset("Parser cos");
	{
		const size_t COUNT = 2;
		const std::string exprs[COUNT * 2] = 
		{ 
			"cos x",		"(cos x)",
			"cos(x)",		"(cos x)"
		};

		for (size_t i = 0; i < COUNT; ++i)
		{
			ExprNode* expr = parseTokens(tokenizeStr(exprs[2 * i]));
			flattenExpr(expr);
			tests.runTest(toPrefixString(expr), exprs[2 * i + 1]);
			freeExpression(expr);
		}
		std::cout << tests.result() << std::endl;
		status &= tests.status();
	}

	tests.reset("Parser tan");
	{
		const size_t COUNT = 3;
		const std::string exprs[COUNT * 2] = 
		{ 
			"tan x",		"(tan x)",
			"tan(x)",		"(tan x)",
			"tan(x, y)",	"(tan x y)"
		};

		for (size_t i = 0; i < COUNT; ++i)
		{
			ExprNode* expr = parseTokens(tokenizeStr(exprs[2 * i]));
			flattenExpr(expr);
			tests.runTest(toPrefixString(expr), exprs[2 * i + 1]);
			freeExpression(expr);
		}
		std::cout << tests.result() << std::endl;
		status &= tests.status();
	}

	//
	//	Multiple operations
	//

	tests.reset("Parser +,-,*,/,^,!,exp,log,sin,cos,tan");
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

		for (size_t i = 0; i < COUNT; ++i)
		{
			ExprNode* expr = parseTokens(tokenizeStr(exprs[2 * i]));
			flattenExpr(expr);
			tests.runTest(toPrefixString(expr), exprs[2 * i + 1]);
			freeExpression(expr);
		}
		std::cout << tests.result() << std::endl;
		status &= tests.status();
	}

	return (int)!status;
}
