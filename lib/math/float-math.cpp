#include <mpfr.h>
#include "float-math.h"

namespace MathSolver
{


Float mod(const Float&x, const Float n)
{
    Float res;
    mpfr_fmod(res.data(), x.data(), n.data(), MPFR_RNDN);
    return res;
}

Float exp(const Float& x)
{
    Float res;
    mpfr_exp(res.data(), x.data(), MPFR_RNDN);
    return res;   
}

Float pow(const Float& b, const Float& x)
{
    Float res;
    mpfr_pow(res.data(), b.data(), x.data(), MPFR_RNDN);
    return res;
}

Float log(const Float& x)
{
    Float res;
    mpfr_log(res.data(), x.data(), MPFR_RNDN);
    return res;   
}

Float sin(const Float& x)
{
    Float res;
    mpfr_sin(res.data(), x.data(), MPFR_RNDN);
    return res;   
}

Float cos(const Float& x)
{
    Float res;
    mpfr_cos(res.data(), x.data(), MPFR_RNDN);
    return res;   
}

Float tan(const Float& x)
{
    Float res;
    mpfr_tan(res.data(), x.data(), MPFR_RNDN);
    return res;   
}

}