#ifndef _MATHSOLVER_UTIL_H_
#define _MATHSOLVER_UTIL_H_

namespace MathSolver
{

template <typename T, typename P, typename Func>   
T parameterize(P& param, const P& val, const Func& nullary)
{
    P& old = param;
    param = val;
    T ret = nullary();
    param = old;
    return ret;
}

}

#endif