#include "range.h"

namespace MathSolver
{

const interval_t INTERVAL_REALS = { "-inf", "inf", false, false };
const interval_t INTERVAL_EMPTY = { "0", "0", false, false };

bool cmpIntervals(const interval_t& lhs, const interval_t& rhs)
{
    if (!isIntersecting(lhs, rhs))      return (lhs.upper < rhs.upper);
    if (lhs.lower < rhs.lower)          return true;
    if (lhs.upper > rhs.upper)          return false;
    return true;
}

bool isValidInterval(const interval_t& ival)
{
    if (ival.lower.isNaN() || ival.upper.isNaN())                           return false;   // NaN
    if ((ival.lower.isInf() && ival.lowerClosed) || 
        (ival.upper.isInf() && ival.upperClosed))                           return false;   // closed inf
    if (ival.lower > ival.upper)                                            return false;   // low > high
    if (ival.lower == ival.upper && ival.lowerClosed != ival.upperClosed)   return false;   // low == high, but closed and open

    return true;
}

interval_t conjoin(const interval_t& lhs, const interval_t& rhs)
{
    if (!isIntersecting(lhs, rhs))                          return INTERVAL_EMPTY;
    if (contains(lhs, rhs))                                 return rhs;
    if (contains(rhs, lhs))                                 return lhs;
    if (rhs.lower > lhs.lower && rhs.lower < lhs.upper)     return { rhs.lower, lhs.upper, rhs.lowerClosed, lhs.upperClosed };
    if (lhs.lower > rhs.lower && lhs.lower < rhs.upper)     return { lhs.lower, rhs.upper, lhs.lowerClosed, rhs.upperClosed };
    else                                                    return { lhs.lower, lhs.upper, lhs.lowerClosed && rhs.lowerClosed, lhs.upperClosed && rhs.upperClosed };
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
    if ((test.lower == ival.lower && test.lowerClosed && !ival.lowerClosed) ||
        (test.upper == ival.upper && test.upperClosed && !ival.upperClosed))      return false;
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
    if (ival.lower == "0.0" && ival.upper == "0.0" && !ival.lowerClosed && !ival.upperClosed)   return "{}";
    return std::string((ival.lowerClosed ? "[" : "(")) + ival.lower.toString() + ", " + ival.upper.toString() + std::string((ival.upperClosed ? "]" : ")"));
}

// 
//  Range type
// 

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

Range::Range(const std::initializer_list<interval_t>& ivals)
{
    for (auto e : ivals)
        mIntervals.push_back(e);
    mIntervals.sort(cmpIntervals);
}

Range::Range(const Range& other)
{
    for (auto e : other.mIntervals)
    {
        interval_t tmp = e;
        mIntervals.push_back(tmp);
    }
}

Range::Range(Range&& other)
{
    while (!other.mIntervals.empty())
    {
        mIntervals.push_back(other.mIntervals.front());
        other.mIntervals.pop_front();
    }
}

Range& Range::operator=(const Range& other)
{
    if (this != &other)
    {
        mIntervals.clear();
        for (auto e : other.mIntervals)
        {
            interval_t tmp = e;
            mIntervals.push_back(tmp);
        }
    }

    return *this;
}

Range& Range::operator=(Range&& other)
{
    if (this != &other)
    {
        mIntervals.clear();
        while (!other.mIntervals.empty())
        {
            mIntervals.push_back(other.mIntervals.front());
            other.mIntervals.pop_front();
        }
    }

    return *this;
}

Range& Range::operator=(const interval_t& ival)
{
    mIntervals.push_back(ival);
    return *this;
}

Range& Range::operator=(const std::initializer_list<interval_t>& ivals)
{
    for (auto e : ivals)
        mIntervals.push_back(e);
    mIntervals.sort(cmpIntervals);
    return *this;
}

Range Range::conjoin(const Range& other) const
{
    Range tmp = *this;
    Range tmp2 = other;
    Range ret;

    auto it = tmp.mIntervals.begin();
    while (it != tmp.mIntervals.end())
    {    
        for (auto it2 = tmp2.mIntervals.begin(); it2 != tmp2.mIntervals.end(); ++it2)
        {
            interval_t& lhs = *it;
            interval_t& rhs = *it2;
            if (isIntersecting(lhs, rhs))
            {   
                interval_t inter = MathSolver::conjoin(lhs, rhs);
                Range sub = (Range(rhs)).subtract(Range(inter));
                
                it2 = tmp2.mIntervals.erase(it2);
                if (!sub.mIntervals.empty())
                    tmp2.mIntervals.insert(it2, sub.mIntervals.begin(), sub.mIntervals.end());
                ret.mIntervals.push_back(inter);
                break;
            }        
        }

        it = ret.mIntervals.erase(it);
    }

    return ret;
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
    Range ret = *this;
    for (auto e : other.mIntervals) ret.mIntervals.push_back(e);
    ret.mIntervals.sort(cmpIntervals);

    for (auto itr = ret.mIntervals.begin(); itr != ret.mIntervals.end() && itr != std::prev(ret.mIntervals.end()); ++itr)
    {
        auto itr2 = std::next(itr);
        interval_t& lhs = *itr;
        interval_t& rhs = *itr2;
        if (isIntersecting(lhs, rhs) || (lhs.lower == rhs.upper && lhs.lowerClosed != rhs.upperClosed) || (lhs.upper == rhs.lower && lhs.upperClosed != rhs.lowerClosed))
        {
            interval_t tmp;
            if (MathSolver::contains(lhs, rhs))  tmp = lhs;
            else if (MathSolver::contains(rhs, lhs))  tmp = rhs;
            else if (rhs.lower > lhs.lower && rhs.lower <= lhs.upper)  tmp = { lhs.lower, rhs.upper, lhs.lowerClosed, rhs.upperClosed };
            else /*(lhs.lower > rhs.lower && lhs.lower <= rhs.upper)*/ tmp = { rhs.lower, lhs.upper, rhs.lowerClosed, lhs.upperClosed };

            itr = ret.mIntervals.erase(itr);
            itr = ret.mIntervals.erase(itr);
            itr = ret.mIntervals.insert(itr, tmp);
        }
    }

    return ret;
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

Range Range::subtract(const Range& other) const
{
    Range ret = *this;
    for (auto rhs : other.mIntervals)
    {      
        auto itr = ret.mIntervals.begin();
        while (itr != ret.mIntervals.end())
        {
            interval_t& lhs = *itr;
            bool removed = false;
            if (MathSolver::contains(rhs, lhs))         
            {   
                removed = true;
            }
            else if (MathSolver::contains(lhs, rhs))
            {
                interval_t low = { lhs.lower, rhs.lower, lhs.lowerClosed, !rhs.lowerClosed };
                interval_t high = { rhs.upper, lhs.upper, !rhs.upperClosed, lhs.upperClosed };
                if (isValidInterval(low)) ret.mIntervals.insert(itr, low);
                if (isValidInterval(high)) ret.mIntervals.insert(itr, high);
                removed = true;
            }
            else
            {
                interval_t inter = MathSolver::conjoin(lhs, rhs);
                if (lhs.lower == inter.lower && lhs.upper == inter.upper)
                {
                    interval_t low = { lhs.lower, lhs.lower, true, true };
                    interval_t high = { lhs.upper, lhs.upper, true, true };
                    ret.mIntervals.insert(itr, { low, high });
                    removed = true;
                }
                else if (lhs.lower == inter.lower)
                {
                    interval_t sub = { inter.upper, lhs.upper, !inter.upperClosed, lhs.upperClosed };
                    ret.mIntervals.insert(itr, sub);
                    removed = true;
                }
                else if (lhs.upper == inter.upper)
                {
                    interval_t sub = { lhs.lower, inter.lower, lhs.lowerClosed, !rhs.lowerClosed };
                    ret.mIntervals.insert(itr, sub);
                    removed = true;
                }
            }

            if (removed)    itr = ret.mIntervals.erase(itr);
            else            ++itr;
        }
    }

    return ret;
}

std::string Range::toString() const
{
    if (mIntervals.empty()) return "{}";
    
    std::string ret = MathSolver::toString(mIntervals.front());
    for (auto i = std::next(mIntervals.begin()); i != mIntervals.end(); ++i)
        ret += (" U " + MathSolver::toString(*i));
    return ret;
}

}