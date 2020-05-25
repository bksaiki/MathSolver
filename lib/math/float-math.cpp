#include <mpfr.h>
#include "float-math.h"

namespace MathSolver
{

Float exp(const Float& x)
{
    Float res;
    mpfr_exp(res.data(), x.data(), MPFR_RNDN);
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