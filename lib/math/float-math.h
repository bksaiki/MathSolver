#ifndef _MATHSOLVER_FLOAT_MATH_H_
#define _MATHSOLVER_FLOAT_MATH_H_

#include "../types/float.h"
#include "../common/base.h"

namespace MathSolver
{

// Returns the absolute value of x.
inline Float abs(const Float& x) { return x.sign() ? -x : x; }

// Function templates

template <typename Func>
Float float1ary(const Func& f, const Float& a)
{
    Float res;
    f(res.data(), a.data(), MATHSOLVER_FLOAT_DEFAULT_RND_MODE);
    return res;
}

template <typename Func>
Float float2ary(const Func& f, const Float& a, const Float& b)
{
    Float res;
    f(res.data(), a.data(), b.data(), MATHSOLVER_FLOAT_DEFAULT_RND_MODE);
    return res;
}

template <typename Func>
Float float3ary(const Func& f, const Float& a, const Float& b, const Float& c)
{
    Float res;
    f(res.data(), a.data(), b.data(), c.data(), MATHSOLVER_FLOAT_DEFAULT_RND_MODE);
    return res;
}

inline Float fma(const Float& a, const Float& b, const Float& c) { return float3ary(mpfr_fma, a, b, c); } // a*b+c
inline Float fms(const Float& a, const Float& b, const Float& c) { return float3ary(mpfr_fms, a, b, c); } // a*b-c
inline Float mod(const Float&x, const Float n) { return float2ary(mpfr_fmod, x, n); }  // x mod n

inline Float sqrt(const Float& x) { return float1ary(mpfr_sqrt, x); }      // x^(1/2)
inline Float cbrt(const Float& x) { return float1ary(mpfr_cbrt, x); }      // x^(1/3)

inline Float exp(const Float& x) { return float1ary(mpfr_exp, x); }                      // e^x
inline Float exp2(const Float& x) { return float1ary(mpfr_exp2, x); }                    // 2^x
inline Float exp10(const Float& x) { return float1ary(mpfr_exp10, x); }                  // 10^x
inline Float pow(const Float& b, const Float& x) { return float2ary(mpfr_pow, b, x); }   // b^x
inline Float log(const Float& x) { return float1ary(mpfr_log, x); }                      // ln x
inline Float log2(const Float& x) { return float1ary(mpfr_log2, x); }                    // log2 x
inline Float log10(const Float& x) { return float1ary(mpfr_log10, x); }                  // log2 x

inline Float sin(const Float& x) { return float1ary(mpfr_sin, x); }   // sine
inline Float cos(const Float& x) { return float1ary(mpfr_cos, x); }   // cosine
inline Float tan(const Float& x) { return float1ary(mpfr_tan, x); }   // tangent
inline Float csc(const Float& x) { return float1ary(mpfr_csc, x); }   // cosecant
inline Float sec(const Float& x) { return float1ary(mpfr_sec, x); }   // secant
inline Float cot(const Float& x) { return float1ary(mpfr_cot, x); }   // cotangent

inline Float asin(const Float& x) { return float1ary(mpfr_asin, x); }   // arcsine
inline Float acos(const Float& x) { return float1ary(mpfr_acos, x); }   // arccosine
inline Float atan(const Float& x) { return float1ary(mpfr_atan, x); }   // arctangent
inline Float atan2(const Float& x, const Float& y) { return float2ary(mpfr_atan2, x, y); }   // arctan (y, x)

}

#endif