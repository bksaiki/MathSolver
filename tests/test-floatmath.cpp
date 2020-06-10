#include <iostream>
#include <string>
#include "../lib/test/test-common.h"
#include "../lib/math/float-math.h"

using namespace MathSolver;

int main()
{
    bool status = true;
    bool verbose = false;

    {
        TestModule tests("abs", verbose);
        Float a = "1.0";
        Float b = "-1.0";

        tests.runTest(abs(a).toString(), "1");
        tests.runTest(abs(b).toString(), "1");

        std::cout << tests.result() << std::endl;
		status &= tests.status();
    }

    {
        TestModule tests("fma", verbose);
        Float f1 = "1.0";
        Float f2 = "2.0";
        Float f3 = "3.0";

        tests.runTest(fma(f1, f2, f3).toString(), "5");
        tests.runTest(fma(f1, f3, f2).toString(), "5");
        tests.runTest(fma(f2, f3, f1).toString(), "7");

        std::cout << tests.result() << std::endl;
		status &= tests.status();
    }

    {
        TestModule tests("fms", verbose);
        Float f1 = "1.0";
        Float f2 = "2.0";
        Float f3 = "3.0";

        tests.runTest(fms(f1, f2, f3).toString(), "-1");
        tests.runTest(fms(f1, f3, f2).toString(), "1");
        tests.runTest(fms(f2, f3, f1).toString(), "5");

        std::cout << tests.result() << std::endl;
		status &= tests.status();
    }

    {
        TestModule tests("mod", verbose);
        Float f1 = "10.0";
        Float f2 = "3.0";

        tests.runTest(mod(f1, f2).toString(), "1");
        tests.runTest(mod(f1, -f2).toString(), "1");
        tests.runTest(mod(-f1, f2).toString(), "-1");
        tests.runTest(mod(-f1, -f2).toString(), "-1");

        std::cout << tests.result() << std::endl;
		status &= tests.status();
    }

    {
        TestModule tests("sqrt", verbose);
        Float f1 = "4.0";
        Float f2 = "36.0";
        Float f3 = "144.0";

        tests.runTest(sqrt(f1).toString(), "2");
        tests.runTest(sqrt(f2).toString(), "6");
        tests.runTest(sqrt(f3).toString(), "12");

        std::cout << tests.result() << std::endl;
		status &= tests.status();
    }

    {
        TestModule tests("cbrt", verbose);
        Float f1 = "8.0";
        Float f2 = "27.0";
        Float f3 = "64";

        tests.runTest(cbrt(f1).toString(), "2");
        tests.runTest(cbrt(f2).toString(), "3");
        tests.runTest(cbrt(f3).toString(), "4");

        std::cout << tests.result() << std::endl;
		status &= tests.status();
    }

    {
        TestModule tests("exp", verbose);
        Float f1 = "0.0";
        Float f2 = "1.0";
        Float f3 = "-1.0";

        tests.runTest(exp(f1).toString(), "1");
        tests.runTest(exp(f2).toString(), "2.718281828459045");
        tests.runTest(exp(f3).toString(), "0.3678794411714423");

        std::cout << tests.result() << std::endl;
		status &= tests.status();
    }

    {
        TestModule tests("exp2", verbose);
        Float f1 = "0.0";
        Float f2 = "1.0";
        Float f3 = "-1.0";

        tests.runTest(exp2(f1).toString(), "1");
        tests.runTest(exp2(f2).toString(), "2");
        tests.runTest(exp2(f3).toString(), "0.5");

        std::cout << tests.result() << std::endl;
		status &= tests.status();
    }

    {
        TestModule tests("exp10", verbose);
        Float f1 = "0.0";
        Float f2 = "1.0";
        Float f3 = "-1.0";

        tests.runTest(exp10(f1).toString(), "1");
        tests.runTest(exp10(f2).toString(), "10");
        tests.runTest(exp10(f3).toString(), "0.1");

        std::cout << tests.result() << std::endl;
		status &= tests.status();
    }

    {
        TestModule tests("pow", verbose);
        Float f1 = "2.0";
        Float f2 = "3.0";
        Float f3 = "4.0";

        tests.runTest(pow(f1, f2).toString(), "8");
        tests.runTest(pow(f2, f3).toString(), "81");
        tests.runTest(pow(f3, f1).toString(), "16");

        std::cout << tests.result() << std::endl;
		status &= tests.status();
    }

    {
        TestModule tests("ln", verbose);
        Float f1 = "1.0";
        Float f2 = "10.0";
        Float f3 = "100.0";

        tests.runTest(log(f1).toString(), "0");
        tests.runTest(log(f2).toString(), "2.302585092994046");
        tests.runTest(log(f3).toString(), "4.605170185988091");

        std::cout << tests.result() << std::endl;
		status &= tests.status();
    }

    {
        TestModule tests("log2", verbose);
        Float f1 = "1.0";
        Float f2 = "2.0";
        Float f3 = "4.0";

        tests.runTest(log2(f1).toString(), "0");
        tests.runTest(log2(f2).toString(), "1");
        tests.runTest(log2(f3).toString(), "2");

        std::cout << tests.result() << std::endl;
		status &= tests.status();
    }

    {
        TestModule tests("log10", verbose);
        Float f1 = "1.0";
        Float f2 = "10.0";
        Float f3 = "100.0";

        tests.runTest(log10(f1).toString(), "0");
        tests.runTest(log10(f2).toString(), "1");
        tests.runTest(log10(f3).toString(), "2");

        std::cout << tests.result() << std::endl;
		status &= tests.status();
    }

    {
        TestModule tests("sin", verbose);
        Float f1 = "0.0";
        Float f2 = "1.0";
        Float f3 = "-1.0";

        tests.runTest(sin(f1).toString(), "0");
        tests.runTest(sin(f2).toString(), "0.8414709848078965");
        tests.runTest(sin(f3).toString(), "-0.8414709848078965");

        std::cout << tests.result() << std::endl;
		status &= tests.status();
    }

    {
        TestModule tests("cos", verbose);
        Float f1 = "0.0";
        Float f2 = "1.0";
        Float f3 = "-1.0";

        tests.runTest(cos(f1).toString(), "1");
        tests.runTest(cos(f2).toString(), "0.5403023058681397");
        tests.runTest(cos(f3).toString(), "0.5403023058681397");

        std::cout << tests.result() << std::endl;
		status &= tests.status();
    }

    {
        TestModule tests("tan", verbose);
        Float f1 = "0.0";
        Float f2 = "1.0";
        Float f3 = "-1.0";

        tests.runTest(tan(f1).toString(), "0");
        tests.runTest(tan(f2).toString(), "1.557407724654902");
        tests.runTest(tan(f3).toString(), "-1.557407724654902");

        std::cout << tests.result() << std::endl;
		status &= tests.status();
    }

    {
        TestModule tests("csc", verbose);
        Float f1 = "0.0";
        Float f2 = "1.0";
        Float f3 = "-1.0";

        tests.runTest(csc(f1).toString(), "inf");
        tests.runTest(csc(f2).toString(), "1.188395105778121");
        tests.runTest(csc(f3).toString(), "-1.188395105778121");

        std::cout << tests.result() << std::endl;
		status &= tests.status();
    }

    {
        TestModule tests("sec", verbose);
        Float f1 = "0.0";
        Float f2 = "1.0";
        Float f3 = "-1.0";

        tests.runTest(sec(f1).toString(), "1");
        tests.runTest(sec(f2).toString(), "1.850815717680926");
        tests.runTest(sec(f3).toString(), "1.850815717680926");

        std::cout << tests.result() << std::endl;
		status &= tests.status();
    }

    {
        TestModule tests("cot", verbose);
        Float f1 = "0.0";
        Float f2 = "1.0";
        Float f3 = "-1.0";

        tests.runTest(cot(f1).toString(), "inf");
        tests.runTest(cot(f2).toString(), "0.6420926159343307");
        tests.runTest(cot(f3).toString(), "-0.6420926159343307");

        std::cout << tests.result() << std::endl;
		status &= tests.status();
    }

    {
        TestModule tests("arcsin", verbose);
        Float f1 = "0.0";
        Float f2 = "1.0";
        Float f3 = "-1.0";

        tests.runTest(asin(f1).toString(), "0");
        tests.runTest(asin(f2).toString(), "1.570796326794897");
        tests.runTest(asin(f3).toString(), "-1.570796326794897");

        std::cout << tests.result() << std::endl;
		status &= tests.status();
    }

    {
        TestModule tests("arccos", verbose);
        Float f1 = "0.0";
        Float f2 = "1.0";
        Float f3 = "-1.0";

        tests.runTest(acos(f1).toString(), "1.570796326794897");
        tests.runTest(acos(f2).toString(), "0");
        tests.runTest(acos(f3).toString(), "3.141592653589793");

        std::cout << tests.result() << std::endl;
		status &= tests.status();
    }

    {
        TestModule tests("arctan", verbose);
        Float f1 = "0.0";
        Float f2 = "1.0";
        Float f3 = "-1.0";

        tests.runTest(atan(f1).toString(), "0");
        tests.runTest(atan(f2).toString(), "0.7853981633974483");
        tests.runTest(atan(f3).toString(), "-0.7853981633974483");

        std::cout << tests.result() << std::endl;
		status &= tests.status();
    }

    {
        TestModule tests("arctan2", verbose);
        Float f1 = "0.0";
        Float f2 = "1.0";
        Float f3 = "-1.0";

        tests.runTest(atan2(f1, f2).toString(), "0");
        tests.runTest(atan2(f2, f3).toString(), "2.356194490192345");
        tests.runTest(atan2(f3, f1).toString(), "-1.570796326794897");

        std::cout << tests.result() << std::endl;
		status &= tests.status();
    }

    mpfr_free_cache();
    return (bool)(!status);
}