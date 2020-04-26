#ifndef _MATHSOLVER_TEST_COMMON_H_
#define _MATHSOLVER_TEST_COMMON_H_

#include <stddef.h>
#include <string>
#include <vector>

namespace MathSolver
{

class TestModule
{
public:

    // Default constructor
    TestModule(const std::string& name, bool verbose = false) : mName(name), mPassed(0), mVerbose(verbose) {}

    // No copying, assigning
    TestModule(const TestModule&) = delete;
    TestModule& operator=(const TestModule&) = delete;

    // Returns the result of the tests as a string.
    std::string result() const;

    // Compares test output and the expected output. Logs a failure if they are not equal.
    void runTest(const std::string& test, const std::string& expected);

    // Sets the name and verbosity of the module and resets the statistics.
    void reset(const std::string& name);

    // Returns true if all tests have passed.
    inline bool status() const { return (mPassed == mResults.size()); }

private:

    std::vector<std::string> mResults;
    std::string mName;
    size_t mPassed;
    bool mVerbose;
};

} // END MathSolver namespace

#endif