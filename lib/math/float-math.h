#ifndef _MATHSOLVER_FLOAT_MATH_H_
#define _MATHSOLVER_FLOAT_MATH_H_

#include "../types/float.h"
#include "../common/base.h"

namespace MathSolver
{

// Returns the absolute value of x.
inline Float abs(const Float& x) { return x.sign() ? -x : x; }

// Numerical functions

Float exp(const Float& x);  // e^x
Float log(const Float& x);  // natural log
Float sin(const Float& x);  // sine
Float cos(const Float& x);  // cosine
Float tan(const Float& x);  // tangent

}

#endif