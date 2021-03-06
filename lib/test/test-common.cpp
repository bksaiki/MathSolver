#include "test-common.h"

namespace MathSolver
{

std::string TestModule::result() const
{
    std::string results;
    std::string totalStr = std::to_string(mResults.size());
    size_t i = 1;
    for (const std::string& s : mResults)
    {
        if (mVerbose || s.find("FAIL") != std::string::npos)
            results += ("\n    " + std::to_string(i) + "\t" + s);
        ++i;
    }

    return std::to_string(mPassed) + "/" + totalStr + " \"" + mName + "\"" + results;
}

void TestModule::runTest(const std::string& test, const std::string& expected)
{
    std::string result = std::string((test == expected) ? "PASS" : "FAIL") + "\tExpected: " + expected + "\tActual: " + test;
    if (test == expected)    ++mPassed;
    if (gErrorManager.hasAny())
    {
        result += ("\n" + gErrorManager.toString());
        gErrorManager.clear();
    }

    mResults.push_back(result);
}

void TestModule::reset(const std::string& name)
{
    mName = name;
    mPassed = 0;
    mResults.clear();
}

} // END MathSolver namespace