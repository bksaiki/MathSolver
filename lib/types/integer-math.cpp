#include "integer-math.h"

namespace MathSolver
{

Integer gcdr(const Integer& a, const Integer& b)
{
    Integer quo, rem;
    quo = a.divRem(b, rem);
    if (rem.isZero())   return b;
    else                return gcdr(b, rem);
}

Integer gcd(const Integer& a, const Integer& b)
{
    if (cmpBytes(a.data(), a.size(), b.data(), b.size()) >= 0)
        return gcdr(abs(a), abs(b));
    else
        return gcdr(abs(b), abs(a));
}

}