#include <iostream>
#include <list>
#include <string>
#include "../lib/mathsolver.h"
#include "../lib/expr/matcher.h"
#include "../lib/test/test-common.h"

using namespace MathSolver;

std::string bool_to_string(bool x)
{
	return std::string((x ? "true" : "false"));
}

std::string list_to_string(const std::list<std::string>& li)
{
    if (li.empty())     return "";

    std::string s = li.front();
    for (auto it = std::next(li.begin()); it != li.end(); ++it)
        s += (" " + *it);
    return s;
}

int main()
{
    bool status = true;
	bool verbose = false;

    {
        const size_t COUNT = 3;
        std::string match[COUNT * 2] =
        {
            "(+ ?a ?b)",            "( + ?a ?b )",
            "(+ (* ?a ?b) ?c)",     "( + ( * ?a ?b ) ?c )",
            "(+ (* PI E) ?a)",      "( + ( * PI E ) ?a )"
        };

        TestModule tests("Tokenize", verbose);
        
        for (size_t i = 0; i < COUNT; ++i)
            tests.runTest(list_to_string(tokenizeMatchString(match[2 * i])), match[2 * i + 1]);
        std::cout << tests.result() << std::endl;
        status &= tests.status();
    }

    {
        const size_t COUNT = 6;
        std::string match[COUNT * 2] =
        {
            "(+ ?a ?b)",            "true",
            "(+ (* ?a ?b) ?c)",     "true",
            "(+ (* PI E) ?a)",      "true",
            "()",                   "false",
            "(+ ?a ())",            "false",
            "(+ (* ?a ?b)",         "false"
        };
        
        TestModule tests("Verify", verbose);
        for (size_t i = 0; i < COUNT; ++i)
            tests.runTest(bool_to_string(isMatchString(match[2 * i])), match[2 * i + 1]);
        std::cout << tests.result() << std::endl;
        status &= tests.status();
    }

    {
        const size_t EXPR_COUNT = 2;
        std::string exprs[EXPR_COUNT] =
        {
            "x",
            "3.14"
        };

        const size_t MATCH_COUNT = 2;
        std::string match[MATCH_COUNT] =
        {
            "x",
            "?a"
        };

        std::string expect[EXPR_COUNT * MATCH_COUNT] =
        {
            "true", "true",
            "false", "true"
        };
        
        TestModule tests("Single token matching", verbose);
        for (size_t i = 0; i < EXPR_COUNT; ++i)
        {
            for (size_t j = 0; j < MATCH_COUNT; ++j)
            {
                ExprNode* expr = parseString(exprs[i]);
                tests.runTest(bool_to_string(matchExpr(match[j], expr)), expect[i * EXPR_COUNT + j]);
                freeExpression(expr);
            }
        }

        std::cout << tests.result() << std::endl;
        status &= tests.status();
    }

    {
        const size_t EXPR_COUNT = 3;
        std::string exprs[EXPR_COUNT] =
        {
            "5 * 3 + 2",
            "x + y",
            "PI * E + 10.5"
        };

        const size_t MATCH_COUNT = 3;
        std::string match[MATCH_COUNT] =
        {
            "(+ ?a ?b)",
            "(+ (* ?a ?b) ?c)",
            "(+ (* PI E) ?a)"
        };

        std::string expect[EXPR_COUNT * MATCH_COUNT] =
        {
            "false", "true", "false",
            "true", "false", "false",
            "false", "true", "true"
        };
        
        TestModule tests("Multi token matching", verbose);
        for (size_t i = 0; i < EXPR_COUNT; ++i)
        {
            for (size_t j = 0; j < MATCH_COUNT; ++j)
            {
                ExprNode* expr = parseString(exprs[i]);
                tests.runTest(bool_to_string(matchExpr(match[j], expr)), expect[i * EXPR_COUNT + j]);
                freeExpression(expr);
            }
        }

        std::cout << tests.result() << std::endl;
        status &= tests.status();
    }

    {
        const size_t COUNT = 4;
        std::string exprs[COUNT] =
        {
            "x + x",
            "x * 0",
            "x * x",
            "x / 0"
        };

        std::string match[COUNT * 2] =
        {
            "(+ ?a ?a)",    "(* 2 ?a)",
            "(* ?a 0)",     "0",
            "(* ?a ?a)",    "(^ ?a 2)",
            "(/ ?a 0)",     "undef"
        };

        std::string expect[COUNT] =
        {
            "(* 2 x)",
            "0",
            "(^ x 2)",
            "undef"
        };
        
        TestModule tests("Match and replace", verbose);
        for (size_t i = 0; i < COUNT; ++i)
        {
            ExprNode* expr = parseString(exprs[i]);
            expr = applyMatchTransform(match[2 * i], match[2 * i + 1], expr);
            tests.runTest(toPrefixString(expr), expect[i]);
            freeExpression(expr);
        }

        std::cout << tests.result() << std::endl;
        status &= tests.status();
    }

    {
        UniqueTransformMatcher utm;
        utm.add("(+ ?a ?a)", "(* 2 ?a)");
        utm.add("(* ?a ?a)", "(^ ?a 2)");
        utm.add("(* ?a 0)", "0");
        utm.add("(/ ?a 0)", "undef");

        const size_t COUNT = 4;
        std::string exprs[COUNT] =
        {
            "x + x",
            "x * 0",
            "x * x",
            "x / 0"
        };

        std::string expect[COUNT] =
        {
            "(* 2 x)",
            "0",
            "(^ x 2)",
            "undef"
        };

        TestModule tests("Unique transform matcher", verbose);
        for (size_t i = 0; i < COUNT; ++i)
        {
            ExprNode* expr = parseString(exprs[i]);
            expr = utm.transform(expr);
            tests.runTest(toPrefixString(expr), expect[i]);
            freeExpression(expr);
        }

        std::cout << tests.result() << std::endl;
        status &= tests.status();
    }

    return (int)(!status);

}