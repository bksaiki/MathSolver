#include <string>
#include "../lib/mathsolver.h"
#include "../lib/test/test-common.h"

using namespace MathSolver;

int main()
{
    const size_t INT_ARR_SIZE = 10;
    Integer cints[INT_ARR_SIZE];
    std::string strs[INT_ARR_SIZE] = {
        "8975123573246880",
        "-134930869899202",
        "31359086913123",
        "-6498981925330",
        "2234623451",
        "-5090920131",
        "107247",
        "-86247",
        "341",
        "-234"
    };

    bool status = true;

    for (size_t i = 0; i < INT_ARR_SIZE; ++i)
        cints[i].fromString(strs[i]);

    TestModule tests("abs");
    {
        Integer a = strs[0];
        Integer b = strs[1];

        tests.runTest(abs(a).toString(), strs[0]);
        tests.runTest(abs(b).toString(), strs[1].substr(1));

        std::cout << tests.result() << std::endl;
		status &= tests.status();
    }

    {
        tests.reset("gcd");
        Integer r01 = gcd(cints[0], cints[1]);
        Integer r23 = gcd(cints[2], cints[3]);
        Integer r45 = gcd(cints[4], cints[5]);
        Integer r67 = gcd(cints[6], cints[7]);
        Integer r89 = gcd(cints[8], cints[9]);

        tests.runTest(r01.toString(), "2");
        tests.runTest(r23.toString(), "1");
        tests.runTest(r45.toString(), "1");
        tests.runTest(r67.toString(), "21");
        tests.runTest(r89.toString(), "1");

        std::cout << tests.result() << std::endl;
		status &= tests.status();
    }

    return (int)!status;
}