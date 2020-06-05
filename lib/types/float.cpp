#include <cstdio>
#include <cstring>
#include "float.h"

namespace MathSolver
{

Float::Float()
{
    mpfr_init2(mData, MATHSOLVER_FLOAT_DEFAULT_PREC);
    mRoundDir = 0;
}

Float::Float(const Float& other)
{
    mpfr_init2(mData, MATHSOLVER_FLOAT_DEFAULT_PREC);
    mRoundDir = mpfr_set(mData, other.mData, MATHSOLVER_FLOAT_DEFAULT_RND_MODE);
}

Float::Float(const char* data)
{
    mpfr_init2(mData, MATHSOLVER_FLOAT_DEFAULT_PREC);
    fromString(data);
}

Float::Float(const std::string& data)
{
    mpfr_init2(mData, MATHSOLVER_FLOAT_DEFAULT_PREC);
    fromString(data.c_str());
}

Float::~Float()
{
    mpfr_clear(mData);
}

Float& Float::operator=(const Float& other)
{
    if (mData->_mpfr_d == NULL) mpfr_init2(mData, MATHSOLVER_FLOAT_DEFAULT_PREC);
    mRoundDir = mpfr_set(mData, other.mData, MATHSOLVER_FLOAT_DEFAULT_RND_MODE);
    return *this;
}

Float& Float::operator=(const char* data)
{
    if (mData->_mpfr_d == NULL) mpfr_init2(mData, MATHSOLVER_FLOAT_DEFAULT_PREC);
    fromString(data);
    return *this;
}

Float& Float::operator=(const std::string& data)
{
    if (mData->_mpfr_d == NULL) mpfr_init2(mData, MATHSOLVER_FLOAT_DEFAULT_PREC);
    fromString(data.c_str());
    return *this;
}

Float Float::operator+(const Float& other) const
{
    Float res;
    res.mRoundDir = mpfr_add(res.mData, mData, other.mData, MATHSOLVER_FLOAT_DEFAULT_RND_MODE);
    return res;
}

Float Float::operator-(const Float& other) const 
{
    Float res;
    res.mRoundDir = mpfr_sub(res.mData, mData, other.mData, MATHSOLVER_FLOAT_DEFAULT_RND_MODE);
    return res;
}

Float Float::operator*(const Float& other) const
{
    Float res;
    res.mRoundDir = mpfr_mul(res.mData, mData, other.mData, MATHSOLVER_FLOAT_DEFAULT_RND_MODE);
    return res;
}

Float Float::operator/(const Float& other) const
{
    Float res;
    res.mRoundDir = mpfr_div(res.mData, mData, other.mData, MATHSOLVER_FLOAT_DEFAULT_RND_MODE);
    return res;
}

Float& Float::operator+=(const Float& other)
{
    mRoundDir = mpfr_add(mData, mData, other.mData, MATHSOLVER_FLOAT_DEFAULT_RND_MODE);
    return *this;
}

Float& Float::operator-=(const Float& other)
{
    mRoundDir = mpfr_sub(mData, mData, other.mData, MATHSOLVER_FLOAT_DEFAULT_RND_MODE);
    return *this;
}

Float& Float::operator*=(const Float& other)
{
    mRoundDir = mpfr_mul(mData, mData, other.mData, MATHSOLVER_FLOAT_DEFAULT_RND_MODE);
    return *this;
}

Float& Float::operator/=(const Float& other)
{
    mRoundDir = mpfr_div(mData, mData, other.mData, MATHSOLVER_FLOAT_DEFAULT_RND_MODE);
    return *this;
}

Float Float::operator-() const
{
    Float res = *this;
    res.mData->_mpfr_sign *= -1;
    return res;
}

Float Float::operator+() const
{
    return Float(*this);
}

std::string Float::toString() const
{
    char* c = new char[30];
    mpfr_snprintf(c, 30, "%.16Rg", mData);  
    mpfr_free_cache();

    std::string s(c);
    delete[] c;

    if (s == "@Inf@")      return "inf";
    if (s == "-@Inf@")     return "-inf";
    if (s == "@NaN@")      return "nan";
    return s;
}

std::string Float::toExactString() const
{
    mpfr_exp_t e;
    char* c = mpfr_get_str(NULL, &e, 10, 0, mData, MATHSOLVER_FLOAT_DEFAULT_RND_MODE);

    std::string s(c);
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

 void Float::fromString(const char* str)
 {
    char* end;
    int t = mpfr_strtofr(mData, str, &end, 0, MATHSOLVER_FLOAT_DEFAULT_RND_MODE);
    if (end[0] != '\0')  gErrorManager.log("Failed conversion from \"" + std::string(str) + "\" to Float", ErrorManager::WARNING);
    if (t != 0)          gErrorManager.log("Rounding occurred when converting \"" + std::string(str) + "\" to a Float", ErrorManager::MESSAGE);
 }

}