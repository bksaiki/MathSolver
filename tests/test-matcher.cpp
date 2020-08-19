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

int main()
{
    {
        const size_t COUNT = 3;
        std::string match[COUNT] =
        {
            "(+ ?a ?b)",
            "(+ (* ?a ?b) ?c)",
            "(+ (* PI E) ?a)"
        };
        
        for (size_t i = 0; i < COUNT; ++i)
        {
            std::list<std::string> tokens = tokenizeMatchString(match[i]);
            for (auto e : tokens) std::cout << e << " ";
            std::cout << std::endl;
        }
    }

    {
        const size_t COUNT = 6;
        std::string match[COUNT] =
        {
            "(+ ?a ?b)",
            "(+ (* ?a ?b) ?c)",
            "(+ (* PI E) ?a)",
            "()",
            "(+ ?a ())",
            "(+ (* ?a ?b)"
        };
        
        std::cout << "isMatchString?" << std::endl;
        for (auto e : match)
            std::cout << bool_to_string(isMatchString(e)) << std::endl;
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
        
        std::cout << "Single token match?" << std::endl;
        for (size_t i = 0; i < EXPR_COUNT; ++i)
        {
            for (size_t j = 0; j < MATCH_COUNT; ++j)
            {
                ExprNode* expr = parseString(exprs[i]);
                std::cout << exprs[i] << " satisfies? " << match[j] << " : " <<
                    bool_to_string(matchExpr(match[j], expr)) << std::endl;
                freeExpression(expr);
            }
        }
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
            "(+ (* PI E) ?a)",
        };
        
        std::cout << "Multi token match?" << std::endl;
        for (size_t i = 0; i < EXPR_COUNT; ++i)
        {
            for (size_t j = 0; j < MATCH_COUNT; ++j)
            {
                ExprNode* expr = parseString(exprs[i]);
                bool res = matchExpr(match[j], expr);
                std::cout << exprs[i] << " satisfies? " << match[j] << " : " << bool_to_string(res) << std::endl;
                freeExpression(expr);
            }
        }
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
            "(* ?a 0)",     "undef"
        };
        
        std::cout << "Match and replace" << std::endl;
        for (size_t i = 0; i < COUNT; ++i)
        {
            ExprNode* expr = parseString(exprs[i]);
            expr = applyMatchTransform(match[2 * i], match[2 * i + 1], expr);
            std::cout << toPrefixString(expr) << std::endl;
            freeExpression(expr);
        }
    }

    return 0;

}