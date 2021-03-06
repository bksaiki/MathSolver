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

// a^nb equivalent to: 
//    a*(a^2)^((b-1)/2),    if b is odd
//    (a^2)^(b/2),          if b is even    
Integer powr(const Integer& a, const Integer& b)
{
    if (b.isZero())         return Integer(1);
    if (b == Integer(1))    return a;
    if (b.isEven())         return powr(a * a, b >> 1);
    else                    return a * powr(a * a, (b - Integer(1)) >> 1);
}

Integer pow(const Integer& a, const Integer& b)
{
    if (b.sign()) 
    {
        gErrorManager.log("Integer rasied to a negative integer does not result in an integer: " + b.toString(),
                          ErrorManager::ERROR, __FILE__, __LINE__);
        return Integer(0);
    }

    return powr(a, b);
}


Integer factr(const Integer& p, int n)
{
    if (n > 1)  return factr(p * Integer(n), n - 1);
    else        return p;
}

Integer fact(int n)
{
    if (n > MATHSOLVER_FACTORIAL_MAX)
    {
        gErrorManager.log("Factorial value too large. Giving up and returning infinity", ErrorManager::WARNING);
        return Integer(0);  // TODO: return inf
    }    
    
    if (n < 0)
    {
        gErrorManager.log("Cannot take the factorial of a negative integer", ErrorManager::ERROR);
        return Integer(0); // TODO: return undef
    }
    
    if (n == 0) 
        return Integer(1);
    return factr(Integer(1), n);
}

}