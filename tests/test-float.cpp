#include <iostream>
#include "../lib/mathsolver.h"
#include "../lib/test/test-common.h"

using namespace MathSolver;

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
            "1e-20",        "1.00000000000000000003e-20"
        };

        TestModule tests("Constructor (string)", verbosity);
        for (size_t i = 0; i < TEST_COUNT; ++i)
        {
            tests.runTest(Float(strs[2 * i]).toString(), strs[2 * i + 1]);
        }

        std::cout << tests.result() << std::endl;
        status &= tests.status();
    }

    return (int)(!status);
}