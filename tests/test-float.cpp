#include <iostream>
#include <string>
#include "../lib/mathsolver.h"
#include "../lib/test/test-common.h"

using namespace MathSolver;

std::string bool_to_string(bool x)
{
	return std::string((x ? "true" : "false"));
}

int main()
{
    //
	//	Tests
	//

    bool status = true;
	bool verbosity = false;

    {
        const int TEST_COUNT = 10;
        std::string strs[TEST_COUNT * 2] = 
        {   
            "1.0",          "1",
            "0.0",          "0",
            "-1.0",         "-1",
            "inf",          "inf",
            "nan",          "nan",
            "-inf",         "-inf",
            "1e5",          "100000",
            "5e-1",         "0.5",
            "1e20",         "1e+20",
            "1e-20",        "1e-20"
        };

        TestModule tests("Constructor (string)", verbosity);
        for (size_t i = 0; i < TEST_COUNT; ++i)
        {
            tests.runTest(Float(strs[2 * i]).toString(), strs[2 * i + 1]);
        }

        std::cout << tests.result() << std::endl;
        status &= tests.status();
    }

    {
        const int TEST_COUNT = 10;
        std::string strs[TEST_COUNT * 2] = 
        {   
            "1.0",          "1",
            "0.0",          "0",
            "-1.0",         "-1",
            "inf",          "inf",
            "nan",          "nan",
            "-inf",         "-inf",
            "1e5",          "100000",
            "5e-1",         "0.5",
            "1e20",         "1e+20",
            "1e-20",        "1e-20"
        };

        TestModule tests("Assignment (string)", verbosity);
        for (size_t i = 0; i < TEST_COUNT; ++i)
        {
            Float f = strs[2* i];
            tests.runTest(f.toString(), strs[2 * i + 1]);
        }

        std::cout << tests.result() << std::endl;
        status &= tests.status();
    }

    {
        Float f1 = "1.0";
        Float f2 = "-1.0";
        Float f3 = "1e5";
        Float f4 = "1e20";
        Float f5 = "1e-20";

        TestModule tests("+", verbosity);
        tests.runTest((f1 + f2).toString(), "0");
        tests.runTest((f2 + f3).toString(), "99999");
        tests.runTest((f3 + f4).toString(), "1.000000000000001e+20");
        tests.runTest((f4 + f5).toString(), "1e+20");
        tests.runTest((f1 + f3).toString(), "100001");
        tests.runTest((f2 + f4).toString(), "1e+20");
        tests.runTest((f3 + f5).toString(), "100000");
        tests.runTest((f1 + f4).toString(), "1e+20");
        tests.runTest((f2 + f5).toString(), "-1");       
        tests.runTest((f1 + f5).toString(), "1");

        std::cout << tests.result() << std::endl;
        status &= tests.status();
    }

    {
        Float f1 = "1.0";
        Float f2 = "-1.0";
        Float f3 = "1e5";
        Float f4 = "1e20";
        Float f5 = "1e-20";

        TestModule tests("-", verbosity);
        tests.runTest((f1 - f2).toString(), "2");
        tests.runTest((f2 - f3).toString(), "-100001");
        tests.runTest((f3 - f4).toString(), "-9.99999999999999e+19");
        tests.runTest((f4 - f5).toString(), "1e+20");
        tests.runTest((f1 - f3).toString(), "-99999");
        tests.runTest((f2 - f4).toString(), "-1e+20");
        tests.runTest((f3 - f5).toString(), "100000");
        tests.runTest((f1 - f4).toString(), "-1e+20");
        tests.runTest((f2 - f5).toString(), "-1");       
        tests.runTest((f1 - f5).toString(), "1");

        std::cout << tests.result() << std::endl;
        status &= tests.status();
    }

    {
        Float f1 = "1.0";
        Float f2 = "-1.0";
        Float f3 = "1e5";
        Float f4 = "1e20";
        Float f5 = "1e-20";

        TestModule tests("*", verbosity);
        tests.runTest((f1 * f2).toString(), "-1");
        tests.runTest((f2 * f3).toString(), "-100000");
        tests.runTest((f3 * f4).toString(), "1e+25");
        tests.runTest((f4 * f5).toString(), "1");
        tests.runTest((f1 * f3).toString(), "100000");
        tests.runTest((f2 * f4).toString(), "-1e+20");
        tests.runTest((f3 * f5).toString(), "1e-15");
        tests.runTest((f1 * f4).toString(), "1e+20");
        tests.runTest((f2 * f5).toString(), "-1e-20");       
        tests.runTest((f1 * f5).toString(), "1e-20");

        std::cout << tests.result() << std::endl;
        status &= tests.status();
    }

    {
        Float f1 = "1.0";
        Float f2 = "-1.0";
        Float f3 = "1e5";
        Float f4 = "1e20";
        Float f5 = "1e-20";

        TestModule tests("/", verbosity);
        tests.runTest((f1 / f2).toString(), "-1");
        tests.runTest((f2 / f3).toString(), "-1e-05");
        tests.runTest((f3 / f4).toString(), "1e-15");
        tests.runTest((f4 / f5).toString(), "1e+40");
        tests.runTest((f1 / f3).toString(), "1e-05");
        tests.runTest((f2 / f4).toString(), "-1e-20");
        tests.runTest((f3 / f5).toString(), "1e+25");
        tests.runTest((f1 / f4).toString(), "1e-20");
        tests.runTest((f2 / f5).toString(), "-1e+20");       
        tests.runTest((f1 / f5).toString(), "1e+20");

        std::cout << tests.result() << std::endl;
        status &= tests.status();
    }

    {
        Float f1 = "1.0";
        Float f2 = "-1.0";

        TestModule tests("- (unary)", verbosity);
        tests.runTest((-f1).toString(), "-1");
        tests.runTest((-f2).toString(), "1"); 

        std::cout << tests.result() << std::endl;
        status &= tests.status();
    }

    {
        Float f1 = "1.0";
        Float f2 = "-1.0";

        TestModule tests("==", verbosity);
        tests.runTest(bool_to_string(f1 == f1), "true");
        tests.runTest(bool_to_string(f1 == f2), "false");

        std::cout << tests.result() << std::endl;
        status &= tests.status();
    }

    {
        Float f1 = "1.0";
        Float f2 = "-1.0";

        TestModule tests("!=", verbosity);
        tests.runTest(bool_to_string(f1 != f1), "false");
        tests.runTest(bool_to_string(f1 != f2), "true");

        std::cout << tests.result() << std::endl;
        status &= tests.status();
    }

    {
        Float f1 = "1.0";
        Float f2 = "-1.0";

        TestModule tests(">", verbosity);
        tests.runTest(bool_to_string(f1 > f2), "true");
        tests.runTest(bool_to_string(f1 > f1), "false");
        tests.runTest(bool_to_string(f2 > f1), "false");

        std::cout << tests.result() << std::endl;
        status &= tests.status();
    }

    {
        Float f1 = "1.0";
        Float f2 = "-1.0";

        TestModule tests("<", verbosity);
        tests.runTest(bool_to_string(f1 < f2), "false");
        tests.runTest(bool_to_string(f1 < f1), "false");
        tests.runTest(bool_to_string(f2 < f1), "true");

        std::cout << tests.result() << std::endl;
        status &= tests.status();
    }

    {
        Float f1 = "1.0";
        Float f2 = "-1.0";

        TestModule tests(">=", verbosity);
        tests.runTest(bool_to_string(f1 >= f2), "true");
        tests.runTest(bool_to_string(f1 >= f1), "true");
        tests.runTest(bool_to_string(f2 >= f1), "false");

        std::cout << tests.result() << std::endl;
        status &= tests.status();
    }

    {
        Float f1 = "1.0";
        Float f2 = "-1.0";

        TestModule tests("<=", verbosity);
        tests.runTest(bool_to_string(f1 <= f2), "false");
        tests.runTest(bool_to_string(f1 <= f1), "true");
        tests.runTest(bool_to_string(f2 <= f1), "true");

        std::cout << tests.result() << std::endl;
        status &= tests.status();
    }

    return (int)(!status);
}