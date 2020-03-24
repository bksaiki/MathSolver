#include <iostream>
#include <list>
#include <string>
#include "../src/test/test-common.h"
#include "../src/expression/evaluator.h"
#include "../src/expression/parser.h"

using namespace MathSolver;

void printTokenList(const std::list<ExpressionNode*>& list)
{
	for (auto e : list)
	{
		if (e->mType == ExpressionNode::INTEGER) 	std::cout << e->mExact.toString();
		else if (e->mType == ExpressionNode::FLOAT)	std::cout << std::to_string(e->mInexact);
		else											std::cout << e->mStr;
		std::cout << " ";
	}
	std::cout << std::endl;
}

int main()
{
	bool status = true;
	TestModule tests("Parser - (negate)");
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
			ExpressionNode* expr = parseTokens(tokenizeStr(exprs[2 * i]));
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
			ExpressionNode* expr = parseTokens(tokenizeStr(exprs[2 * i]));
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
			ExpressionNode* expr = parseTokens(tokenizeStr(exprs[2 * i]));
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
			ExpressionNode* expr = parseTokens(tokenizeStr(exprs[2 * i]));
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
			ExpressionNode* expr = parseTokens(tokenizeStr(exprs[2 * i]));
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
			ExpressionNode* expr = parseTokens(tokenizeStr(exprs[2 * i]));
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
			ExpressionNode* expr = parseTokens(tokenizeStr(exprs[2 * i]));
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
			ExpressionNode* expr = parseTokens(tokenizeStr(exprs[2 * i]));
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
			ExpressionNode* expr = parseTokens(tokenizeStr(exprs[2 * i]));
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
			ExpressionNode* expr = parseTokens(tokenizeStr(exprs[2 * i]));
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
			ExpressionNode* expr = parseTokens(tokenizeStr(exprs[2 * i]));
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
			ExpressionNode* expr = parseTokens(tokenizeStr(exprs[2 * i]));
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
			ExpressionNode* expr = parseTokens(tokenizeStr(exprs[2 * i]));
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
			ExpressionNode* expr = parseTokens(tokenizeStr(exprs[2 * i]));
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
			ExpressionNode* expr = parseTokens(tokenizeStr(exprs[2 * i]));
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
			ExpressionNode* expr = parseTokens(tokenizeStr(exprs[2 * i]));
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
			ExpressionNode* expr = parseTokens(tokenizeStr(exprs[2 * i]));
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
			ExpressionNode* expr = parseTokens(tokenizeStr(exprs[2 * i]));
			flattenExpr(expr);
			tests.runTest(toPrefixString(expr), exprs[2 * i + 1]);
			freeExpression(expr);
		}
		std::cout << tests.result() << std::endl;
		status &= tests.status();
	}

	return (int)!status;
}
