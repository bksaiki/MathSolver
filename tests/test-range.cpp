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
        tests.runTest(r1.toString(), "(-inf, inf)");
        tests.runTest(r2.toString(), "[0, 1]");
        tests.runTest(r3.toString(), "(0, 100]");
        tests.runTest(r4.toString(), "[0, 1]");
        tests.runTest(r5.toString(), "(-inf, inf)");

        std::cout << tests.result() << std::endl;
        status &= tests.status();
    }

    {
        Range r1;
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
        tests.runTest(bool_to_string(contains(ival1, ival4)), "false");
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

    return (int)(!status);
}