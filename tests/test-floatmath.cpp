#include <string>
#include "../lib/mathsolver.h"
#include "../lib/test/test-common.h"

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

    return (bool)(!status);
}