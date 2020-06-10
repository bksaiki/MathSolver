#include <iostream>
#include <string>
#include "../lib/test/test-common.h"
#include "../lib/types/range.h"

using namespace MathSolver;

std::string bool_to_string(bool x)
{
	return std::string((x ? "true" : "false"));
}

Range makeRange() { return Range(); }

int main()
{
	bool status = true;
	bool verbose = false;

    {
        Range r1;
        Range r2("0.0", "1.0", true, true);
        Range r3((interval_t){ "0", "100", false, true });
        Range r4(r2);
        Range r5(makeRange());

        TestModule tests("Constructor", verbose);
        tests.runTest(r1.toString(), "{}");
        tests.runTest(r2.toString(), "[0, 1]");
        tests.runTest(r3.toString(), "(0, 100]");
        tests.runTest(r4.toString(), "[0, 1]");
        tests.runTest(r5.toString(), "{}");

        std::cout << tests.result() << std::endl;
        status &= tests.status();
    }

    {
        Range r1("-inf", "inf", true, true);
        Range r2("0.0", "1.0", true, true);
        Range r3("0", "100", false, true);
        Float f1 = "0.0";
        Float f3 = "-1.0";

        TestModule tests("contains (Range, Float)", verbose);
        tests.runTest(bool_to_string(r1.contains(f1)), "true");
        tests.runTest(bool_to_string(r2.contains(f1)), "true");
        tests.runTest(bool_to_string(r3.contains(f1)), "false");
        tests.runTest(bool_to_string(r2.contains(f3)), "false");

        std::cout << tests.result() << std::endl;
        status &= tests.status();
    }

    {
        interval_t ival1 = { "0.0", "1.0", true, true };
        interval_t ival2 = { "-2.0", "-1.0", true , true };
        interval_t ival3 = { "2.0", "3.0", false, true };
        interval_t ival4 = { "0.0", "1.0", false, false };
        interval_t ival5 = { "0.0", "1.0", true, true };
        interval_t ival6 = { "0.25", "0.75", false, false };

        TestModule tests("contains (interval_t, interval_t)", verbose);
        tests.runTest(bool_to_string(contains(ival1, ival2)), "false");
        tests.runTest(bool_to_string(contains(ival1, ival3)), "false");
        tests.runTest(bool_to_string(contains(ival1, ival4)), "true");
        tests.runTest(bool_to_string(contains(ival1, ival5)), "true");
        tests.runTest(bool_to_string(contains(ival1, ival6)), "true");

        std::cout << tests.result() << std::endl;
        status &= tests.status();
    }

    {
        interval_t ival1 = { "0.0", "1.0", true, true };
        interval_t ival2 = { "-2.0", "-1.0", true , true };
        interval_t ival3 = { "1.0", "2.0", false, true };
        interval_t ival4 = { "1.0", "2.0", true, true };
        interval_t ival5 = { "0.5", "1.5", false, false };
        interval_t ival6 = { "-1.0", "1.0", true, true };

        TestModule tests("intersecting (interval_t)", verbose);
        tests.runTest(bool_to_string(isIntersecting(ival1, ival2)), "false");
        tests.runTest(bool_to_string(isIntersecting(ival1, ival3)), "false");
        tests.runTest(bool_to_string(isIntersecting(ival1, ival4)), "true");
        tests.runTest(bool_to_string(isIntersecting(ival1, ival5)), "true");
        tests.runTest(bool_to_string(isIntersecting(ival1, ival6)), "true");

        std::cout << tests.result() << std::endl;
        status &= tests.status();
    }

    {
        interval_t ival1 = { "0.0", "1.0", true, true };
        interval_t ival2 = { "-2.0", "-1.0", true , true };
        interval_t ival3 = { "1.0", "2.0", false, true };
        interval_t ival4 = { "1.0", "2.0", true, true };
        interval_t ival5 = { "0.5", "1.5", false, false };
        interval_t ival6 = { "-1.0", "1.0", true, true };

        Range r1 = ival1;
        Range r2 = ival2;
        Range r3 = ival5;

        TestModule tests("intersecting (Range)", verbose);
        tests.runTest(bool_to_string(r1.intersectsWith(r2)), "false");
        tests.runTest(bool_to_string(r1.intersectsWith(r3)), "true");

        std::cout << tests.result() << std::endl;
        status &= tests.status();
    }

    {
        Range r1 = { "0.0", "1.0", false, true };
        Range r2 = { "2.0", "3.0", true , true };
        Range r3 = { "1.0", "2.0", false, true };
        Range r4 = { "-1.0", "0.0", true, false };
        Range r5 = { "0.5", "1.5", false, false };
        Range r6 = { "-0.5", "1.5", false, false };
        Range r7 = { "-1.0", "0.0", false, false };
        Range r8 = { "1.0", "2.0", false, false };

        TestModule tests("disjunction (Range)", verbose);
        tests.runTest(r1.disjoin(r2).toString(), "(0, 1] U [2, 3]");
        tests.runTest(r1.disjoin(r3).toString(), "(0, 2]");
        tests.runTest(r1.disjoin(r4).toString(), "[-1, 0) U (0, 1]");
        tests.runTest(r1.disjoin(r5).toString(), "(0, 1.5)");
        tests.runTest(r1.disjoin(r6).toString(), "(-0.5, 1.5)");
        tests.runTest(r1.disjoin(r7).toString(), "(-1, 0) U (0, 1]");
        tests.runTest(r1.disjoin(r8).toString(), "(0, 2)");

        std::cout << tests.result() << std::endl;
        status &= tests.status();
    }

    {
        interval_t ival1 = { "0.0", "1.0", true, true };
        interval_t ival2 = { "2.0", "3.0", true, true };
        interval_t ival3 = { "-1.0", "2.0", true, true };
        interval_t ival4 = { "0.25", "0.75", true, true };
        interval_t ival5 = { "0.5", "1.5", true, true };
        interval_t ival6 = { "0.0", "1.0", false, false };

        TestModule tests("conjoin (interval_t)", verbose);
        tests.runTest(toString(conjoin(ival1, ival2)), "{}");
        tests.runTest(toString(conjoin(ival1, ival3)), "[0, 1]");
        tests.runTest(toString(conjoin(ival1, ival4)), "[0.25, 0.75]");
        tests.runTest(toString(conjoin(ival1, ival5)), "[0.5, 1]");
        tests.runTest(toString(conjoin(ival1, ival6)), "(0, 1)");

        std::cout << tests.result() << std::endl;
        status &= tests.status();
    }

    {
        interval_t ival1 = { "0.0", "1.0", true, true };
        interval_t ival2 = { "2.0", "3.0", true, true };
        interval_t ival3 = { "-1.0", "2.0", true, true };
        interval_t ival4 = { "0.25", "0.75", true, true };
        interval_t ival5 = { "0.5", "1.5", true, true };
        interval_t ival6 = { "0.0", "1.0", false, false };
        interval_t ival7 = { "2.5", "3.5", true, true };
        interval_t ival8 = { "0.5", "2.5", true, true };

        Range r1 = ival1;
        Range r2 = ival2;
        Range r3 = ival3;
        Range r4 = ival4;
        Range r5 = ival5;
        Range r6 = ival6;
        Range r7 = { ival1, ival2 };
        Range r8 = { ival5, ival7 };
        Range r9 = ival8;

        TestModule tests("conjoin (Range)", verbose);
        tests.runTest(r1.conjoin(r2).toString(), "{}");
        tests.runTest(r1.conjoin(r3).toString(), "[0, 1]");
        tests.runTest(r1.conjoin(r4).toString(), "[0.25, 0.75]");
        tests.runTest(r1.conjoin(r5).toString(), "[0.5, 1]");
        tests.runTest(r1.conjoin(r6).toString(), "(0, 1)");
        tests.runTest(r7.conjoin(r8).toString(), "[0.5, 1] U [2.5, 3]");
        tests.runTest(r7.conjoin(r9).toString(), "[0.5, 1] U [2, 2.5]");

        std::cout << tests.result() << std::endl;
        status &= tests.status();
    }

    {
        interval_t ival1 = { "0.0", "1.0", true, true };
        interval_t ival2 = { "2.0", "3.0", true , true };
        interval_t ival3 = { "-1.0", "2.0", true, true };
        interval_t ival4 = { "-0.5", "0.5", true, true };
        interval_t ival5 = { "0.5", "1.5", true, true };
        interval_t ival6 = { "0.0", "1.0", false, false };

        Range r1 = ival1;
        Range r2 = ival2;
        Range r3 = ival3;
        Range r4 = ival4;
        Range r5 = ival5;
        Range r6 = ival6;

        Range r7 = { ival1, ival2 };
        Range r8 = { "0.5", "2.5", true, true };
        Range r9 = { "0.0", "4.0", true, true };
        Range r10 = { ival5, ival2 };

        TestModule tests("subtract", verbose);
        tests.runTest(r1.subtract(r2).toString(), "[0, 1]");
        tests.runTest(r1.subtract(r3).toString(), "{}");
        tests.runTest(r1.subtract(r4).toString(), "(0.5, 1]");
        tests.runTest(r1.subtract(r5).toString(), "[0, 0.5)");
        tests.runTest(r1.subtract(r6).toString(), "[0, 0] U [1, 1]");
        tests.runTest(r7.subtract(r8).toString(), "[0, 0.5) U (2.5, 3]");
        tests.runTest(r9.subtract(r10).toString(), "[0, 0.5) U (1.5, 2) U (3, 4]");

        std::cout << tests.result() << std::endl;
        status &= tests.status();
    }

    return (int)(!status);
}