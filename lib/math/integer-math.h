#ifndef _MATHSOLVER_INTEGERMATH_H_
#define _MATHSOLVER_INTEGERMATH_H_

#include "../types/integer.h"
#include "../common/base.h"

namespace MathSolver
{

// Returns the absolute value of x.
inline Integer abs(const Integer& x) { return x.sign() ? -x : x; }

// Returns the greatest common divisor (gcd) of two integers. The result
// is always positive.
Integer gcd(const Integer& a, const Integer& b);

// Calculates the power of a raised the b
Integer pow(const Integer& a, const Integer& b);

}

#endif