#include "range.h"

namespace MathSolver
{

bool isValidInterval(const interval_t& ival)
{
    if (ival.lower.isNaN() || ival.upper.isNaN())                           return false;   // NaN
    if ((ival.lower.isInf() && ival.lowerClosed) || 
        (ival.upper.isInf() && ival.upperClosed))                           return false;   // closed inf
    if (ival.lower > ival.upper)                                            return false;   // low > high
    if (ival.lower == ival.upper && ival.lowerClosed != ival.upperClosed)   return false;   // low == high, but closed and open

    return true;
}

bool contains(const interval_t& ival, const Float& val)
{
    if (val < ival.lower || val > ival.upper)           return false;
    if ((val == ival.lower && !ival.lowerClosed) ||
        (val == ival.upper && !ival.upperClosed))       return false;
    return true;
}

bool contains(const interval_t& ival, const interval_t& test)
{
    if (test.lower < ival.lower || test.upper > ival.upper)                         return false;
    if ((test.lower == ival.lower && !(test.lowerClosed && ival.lowerClosed)) ||
        (test.upper == ival.upper && !(test.upperClosed && ival.upperClosed)))      return false;
    return true;
}

bool isIntersecting(const interval_t& lhs, const interval_t& rhs)
{
    if (lhs.upper < rhs.lower || lhs.lower > rhs.upper)                         return false;
    if ((lhs.upper == rhs.lower && !(lhs.upperClosed && rhs.lowerClosed)) ||
        (lhs.lower == rhs.upper && !(lhs.upperClosed && rhs.lowerClosed)))      return false;
    return true;
}

std::string toString(const interval_t& ival)
{
    return std::string((ival.lowerClosed ? "[" : "(")) + ival.lower.toString() + ", " + ival.upper.toString() + std::string((ival.upperClosed ? "]" : ")"));
}

const interval_t INTERVAL_REALS = { "-inf", "inf", false, false };

// 
//  Range type
// 

Range::Range()
{
    mIntervals.push_back(INTERVAL_REALS);
}

Range::Range(const Float& lower, const Float& upper, bool lclosed, bool uclosed)
{
    interval_t ival;
    ival.lower = lower;
    ival.upper = upper;
    ival.lowerClosed = lclosed;
    ival.upperClosed = uclosed;

    if (isValidInterval(ival))  mIntervals.push_back(ival);
    else                        mIntervals.push_back(INTERVAL_REALS);
}

Range::Range(const Range& other)
{
    for (auto e : other.mIntervals)
        mIntervals.push_back(e);
}

Range::Range(Range&& other)
{
    std::move(other.mIntervals.begin(), other.mIntervals.end(), mIntervals.begin());
}

Range& Range::operator=(const Range& other)
{
    if (mIntervals.empty() && !other.mIntervals.empty())
    {
        for (auto e : other.mIntervals)
            mIntervals.push_back(e);
    }
    return *this;
}

Range& Range::operator=(Range&& other)
{
    if (mIntervals.empty() && !other.mIntervals.empty())
        std::move(other.mIntervals.begin(), other.mIntervals.end(), mIntervals.begin());
    return *this;
}

Range& Range::operator=(const interval_t& ival)
{
    mIntervals.push_back(ival);
    return *this;
}

Range Range::conjoin(const Range& other) const
{
    return Range();
}

bool Range::contains(const Float& val) const
{
    for (auto e : mIntervals)
    {
        if (MathSolver::contains(e, val))
            return true;
    }

    return false;
}

Range Range::disjoin(const Range& other) const
{
    return Range(); // TODO
}

bool Range::intersectsWith(const Range& other) const
{
    for (auto i : mIntervals)
    {
        for (auto j : other.mIntervals)
        {
            if (isIntersecting(i, j))
                return true;
        }
    }

    return false;
}

std::string Range::toString() const
{
    if (mIntervals.empty()) return "{}";
    
    std::string ret = MathSolver::toString(mIntervals.front());
    for (auto i = std::next(mIntervals.begin()); i != mIntervals.end(); ++i)
        ret += MathSolver::toString(*i) + " ∪ ";
    return ret;
}

}