#include "IntegerMath.h"

namespace MathSolver
{

Integer gcd(const Integer& a, const Integer& b)
{
    Integer div, dsr, rem, quo, last;
    if (cmpBytes(a.data(), a.size(), b.data(), b.size()) >= 0)
    {
        div = abs(a);
        dsr = abs(b);
    }
    else
    {
        div = abs(b);
        dsr = abs(a);
    }

    do
    {
        last = rem;
        quo = div.divRem(dsr, rem);
        div = dsr;
        dsr = rem;
    } while (!rem.isZero());

    return last;
}

}