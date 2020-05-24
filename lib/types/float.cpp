#include <cstring>
#include "float.h"

namespace MathSolver
{

Float::Float()
{
    mpfr_init2(mData, MATHSOLVER_FLOAT_DEFAULT_PREC);
}

Float::Float(const std::string& data)
{
    mpfr_init2(mData, MATHSOLVER_FLOAT_DEFAULT_PREC);
    if (mpfr_set_str(mData, data.c_str(), 0, MPFR_RNDN))
        gErrorManager.log("Failed conversion from " + data + " to Float", ErrorManager::WARNING);
}

Float::Float(const Float& other)
{
    mpfr_init2(mData, MATHSOLVER_FLOAT_DEFAULT_PREC);
    mpfr_set(mData, other.mData, MPFR_RNDN);
}

Float::Float(Float&& other)
{
    mpfr_init2(mData, MATHSOLVER_FLOAT_DEFAULT_PREC);
    mData->_mpfr_sign = other.mData->_mpfr_sign;
    mData->_mpfr_prec = other.mData->_mpfr_prec;
    mData->_mpfr_exp = other.mData->_mpfr_exp;
    mData->_mpfr_d = other.mData->_mpfr_d;
    other.mData->_mpfr_d = NULL;
}

Float::~Float()
{
    mpfr_clear(mData);
}

Float& Float::operator=(const Float& other)
{
    mpfr_init2(mData, MATHSOLVER_FLOAT_DEFAULT_PREC);
    mpfr_set(mData, other.mData, MPFR_RNDN);
    return *this;
}

Float& Float::operator=(Float&& other)
{
    mpfr_init2(mData, MATHSOLVER_FLOAT_DEFAULT_PREC);
    mData->_mpfr_sign = other.mData->_mpfr_sign;
    mData->_mpfr_prec = other.mData->_mpfr_prec;
    mData->_mpfr_exp = other.mData->_mpfr_exp;
    mData->_mpfr_d = other.mData->_mpfr_d;
    other.mData->_mpfr_d = NULL;
    return *this;
}

Float Float::operator+(const Float& other) const
{
    Float res;
    mpfr_add(res.mData, mData, other.mData, MPFR_RNDN);
    return res;
}

Float Float::operator-(const Float& other) const 
{
    Float res;
    mpfr_sub(res.mData, mData, other.mData, MPFR_RNDN);
    return res;
}

Float Float::operator*(const Float& other) const
{
    Float res;
    mpfr_mul(res.mData, mData, other.mData, MPFR_RNDN);
    return res;
}

Float Float::operator/(const Float& other) const
{
    Float res;
    mpfr_div(res.mData, mData, other.mData, MPFR_RNDN);
    return res;
}

Float& Float::operator+=(const Float& other)
{
    mpfr_add(mData, mData, other.mData, MPFR_RNDN);
    return *this;
}

Float& Float::operator-=(const Float& other)
{
    mpfr_sub(mData, mData, other.mData, MPFR_RNDN);
    return *this;
}

Float& Float::operator*=(const Float& other)
{
    mpfr_mul(mData, mData, other.mData, MPFR_RNDN);
    return *this;
}

Float& Float::operator/=(const Float& other)
{
    mpfr_div(mData, mData, other.mData, MPFR_RNDN);
    return *this;
}

std::string Float::toString() const
{
    mpfr_t t;
    mpfr_exp_t e;
    char* c;

    mpfr_init2(t, 64);
    mpfr_set(t, mData, MPFR_RNDN);
    c = mpfr_get_str(NULL, &e, 10, 0, t, MPFR_RNDN);

    std::string s(c);
    mpfr_clear(t);
    mpfr_free_str(c);
    
    if (s == "@Inf@")      return "inf";
    if (s == "-@Inf@")     return "-inf";
    if (s == "@NaN@")      return "nan";

    --e;
    if (e >= 0 && e < MATHSOLVER_SCI_NOTATION_POS_LIM)
    {
        size_t idx = e + ((s[0] == '-') ? 2 : 1);
        s.insert(std::next(s.begin(), idx), '.');
        s.erase(std::next(s.begin(), 1 + s.find_last_not_of("0")), s.end());
        s.erase(std::next(s.begin(), 1 + s.find_last_not_of(".")), s.end());
    }
    else if (e < 0 && e > MATHSOLVER_SCI_NOTATION_NEG_LIM)
    {
        s = "0." + std::string(-(e + 1), '0') + s;
        s.erase(std::next(s.begin(), 1 + s.find_last_not_of('0')), s.end());
        s.erase(std::next(s.begin(), 1 + s.find_last_not_of(".")), s.end());
    }
    else
    {
        s.insert((((s[0] == '-') ? std::next(s.begin(), 2) : std::next(s.begin()))), '.');
        s.erase(std::next(s.begin(), 1 + s.find_last_not_of('0')), s.end());
        s.erase(std::next(s.begin(), 1 + s.find_last_not_of(".")), s.end());
        s.append(((e < 0) ? "e-" : "e+") + std::to_string(abs(e)));
    }
    
    return s;
}

}