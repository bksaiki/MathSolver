#ifndef _MATHSOLVER_RANGE_H_
#define _MATHSOLVER_RANGE_H_

#include <initializer_list>
#include <list>
#include <string>
#include "../common/base.h"
#include "float.h"

namespace MathSolver
{

// Represents a single interval low < x < high
struct interval_t
{  
    Float lower;
    Float upper;
    bool lowerClosed;
    bool upperClosed;
};

// Useful constants
extern const interval_t INTERVAL_REALS;
extern const interval_t INTERVAL_EMPTY;

// Comparator predicate. Order from negative to positive, then size of the intervals, finally returning
// 'lhs' to break ties.
bool cmpIntervals(const interval_t& lhs, const interval_t& rhs);

// Returns the intersection of these intervals.
interval_t conjoin(const interval_t& lhs, const interval_t& rhs);

// Returns true if 'val' is contained within the interval. Assumes the range is valid.
bool contains(const interval_t& ival, const Float& val);

// Returns true if 'test' is completely contained within the interval
bool contains(const interval_t& ival, const interval_t& test);

// Returns true if the interval is valid.
bool isValidInterval(const interval_t& ival);

// Returns true if there exists an intersection between the two intervals.
bool isIntersecting(const interval_t& lhs, const interval_t& rhs);

// Returns the difference between the two invervals
interval_t subtract(const interval_t& lhs, const interval_t& rhs);

// Returns a std::string representation of the interval.
std::string toString(const interval_t& ival);

// Represents a single continuous range or multiple discontinous ranges
class Range
{
public:

    // Default constructor
    inline Range() {}

    // Single interval
    Range(const Float& lower, const Float& upper, bool lclosed, bool uclosed); 

    // Single interval (interval_t)
    inline Range(const interval_t& ival) { mIntervals.push_back(ival); }

    // Multiple intervals (interval_t)
    Range(const std::initializer_list<interval_t>& ivals);

    // Copy constructor
    Range(const Range& other);

    // Move constructor
    Range(Range&& other);

    // Copy assignment
    Range& operator=(const Range& other);

    // Move assignment
    Range& operator=(Range&& other);

    // Assignment from interval_t.
    Range& operator=(const interval_t& ival);

    // Assignment from multiple interval_t.
    Range& operator=(const std::initializer_list<interval_t>& ivals);

    // Returns the conjunction (intersection) of this Range and another.
    Range conjoin(const Range& other) const;

    // Returns true if the value is contained within this Range.
    bool contains(const Float& val) const;

    // Returns a reference to the underlying.
    inline std::list<interval_t>& data() { return mIntervals; }
    inline const std::list<interval_t>& data() const { return mIntervals; }

    // Returns the disjunction (union) of this Range and another.
    Range disjoin(const Range& other) const;

    // Returns true if there exists an intersection between this Range with another.
    bool intersectsWith(const Range& other) const;

    // Returns the difference between this Range and another (set subtraction).
    Range subtract(const Range& other) const;

    // Returns a string representation of this Range.
    std::string toString() const;

private:
    std::list<interval_t> mIntervals;
};

}

#endif