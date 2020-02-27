#include "test-common.h"

namespace Mathsolver
{

std::string TestModule::result() const
{
    std::string results;
    std::string totalStr = std::to_string(mResults.size());
    size_t i = 1;
    for (const std::string& s : mResults)
    {
        if (mVerbose || s.find("FAIL") != std::string::npos)
        {
            results += ("\n  " + std::to_string(i) + + "\t" + s);
        }
        ++i;
    }

    return std::to_string(mPassed) + "/" + totalStr + " tests passed\t\"" + mName + "\"" + results;
}

void TestModule::runTest(const std::string& test, const std::string& expected)
{
    std::string result;
    if (test == expected)
    {
        result += "PASS";
        ++mPassed;
    }
    else
    {
        result += "FAIL";
    }

    mResults.push_back(result + "\tExpected: " + expected + "\tActual: " + test);
}

void TestModule::reset(const std::string& name)
{
    mName = name;
    mPassed = 0;
    mResults.clear();
}

} // END MathSolver namespace