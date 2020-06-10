#include <cassert>
#include <cstring>
#include "integer.h"

namespace MathSolver
{

Integer::Integer()
{
    setZero(2);
}

Integer::Integer(const Integer& other)
{
    mData = new uint32_t[other.mSize];
    mSize = other.mSize;
    mFlags = other.mFlags;
    mSign = other.mSign;
    memcpy(mData, other.mData, other.mSize * 4);
}

Integer::Integer(Integer&& other)
{
    move(other);
}

Integer::Integer(uint32_t* arr, size_t len, bool sign)
{
    mData = arr;
    mSize = len;
    mFlags = 0;
    mSign = sign;
}

Integer::Integer(uint64_t x)
{
    mData = new uint32_t[2];
    mSize = 2;
    mFlags = 0;
    mSign = false;
    memcpy(mData, &x, 8);
}

Integer::Integer(int64_t x)
{  
    if (x < 0)
    {
        x *= -1;
        mSign = true;
    }
    else
    {
        mSign = false;
    }
    
    mData = new uint32_t[2];
    mSize = 2;  
    mFlags = 0; 
    memcpy(mData, &x, 8);
}

Integer::Integer(uint32_t x)
{
    mData = new uint32_t[1];
    mSize = 1;
    mFlags = 0;
    mSign = false;
    memcpy(mData, &x, 4);
}

Integer::Integer(int x)
{
    if (x < 0)
    {
        x *= -1;
        mSign = true;
    }
    else
    {
        mSign = false;
    }
    
    mData = new uint32_t[1];
    mSize = 1;  
    mFlags = 0; 
    memcpy(mData, &x, 4);
}

Integer::Integer(const std::string& str)
{   
    fromStringNoCheck(str);
}

Integer::~Integer()
{
    if(mData != nullptr)
        delete[] mData;
    mData = nullptr;
}

Integer& Integer::operator=(const Integer& other)
{
    if (this != &other)
    {
        delete[] mData;
        mData = new uint32_t[other.mSize];
        mSize = other.mSize;
        mFlags = other.mFlags;
        mSign = other.mSign;
        memcpy(mData, other.mData, other.mSize * 4);
    }

    return *this;
}

Integer& Integer::operator=(Integer&& other)
{
    if (this != &other)
    {
        delete[] mData;
        move(other);
    }

    return *this;
}

Integer& Integer::operator=(const std::string& str)
{
    delete[] mData;
    fromStringNoCheck(str);
    return *this;
}

Integer Integer::operator+(const Integer& other) const
{
    if (isUndef() || other.isUndef())
    {
        return Integer("nan");
    }
    else if (isInf() || other.isInf())
    {
        if (isInf() && other.isInf())
        {
            if (mSign != other.mSign) return Integer("nan"); // +inf + -inf = ?
            else                      return Integer(*this);
        }       
        else if (isInf())
        {
            return Integer(*this);
        }
        else
        {
            return Integer(other);
        }
    }
    else if(mSign == other.mSign)
    {
        return add(other, mSign);
    }
    else
    {
        int cmp = cmpBytes(mData, mSize, other.mData, other.mSize);
        if (cmp == 0) // Avoid computation: x + -x = 0
        {
            uint32_t* arr = new uint32_t[2];
            arr[0] = 0; arr[1] = 0;
            return Integer(arr, 2, false);
        }
        else // result != 0
        {        
            return sub(other, (cmp > 0) ? (mSign) : (other.mSign));
        }
    }
}

Integer Integer::operator-(const Integer& other) const
{
    if (isUndef() || other.isUndef())   return Integer("nan");
    if (isInf() || other.isInf())
    {
        if (isInf() && other.isInf())
        {
            if (mSign == other.mSign) return Integer("nan"); // +inf + -inf = ?
            else                      return Integer(*this);
        }       
        else if (isInf())
        {
            return Integer(*this);
        }
        else
        {
            return Integer(-other);
        }
    }

    int cmp = cmpBytes(mData, mSize, other.mData, other.mSize);
    if (mSign == other.mSign)
    {
        return sub(other, (cmp > 0) ? (mSign) : (!mSign));
    }
    else
    {
        if (cmp == 0) // Avoid computation: x + -x = 0
        {
            uint32_t* arr = new uint32_t[2];
            arr[0] = 0; arr[1] = 0;
            return Integer(arr, 2, false);
        }
        else // result != 0
        {        
            return add(other, mSign);
        }
    }
}

Integer Integer::operator*(const Integer& other) const
{
    if (isUndef() || other.isUndef())     return Integer("nan");
    if (isInf() || other.isInf())
    {
        Integer inf("inf");
        inf.mSign = mSign ^ other.mSign;
        return inf;
    }

    if (isZero() || other.isZero()) return Integer(0);
    return mul(other, mSign ^ other.mSign);
}

Integer Integer::operator/(const Integer& other) const
{
    Integer quo;

    if (isUndef() || other.isUndef())   return Integer("nan");
    if (isInf() && other.isInf())       return Integer("nan");
    if (isInf())                        return Integer(*this); 
    if (other.isInf())                  return Integer(); 

    if (other.isZero())  
    {
        quo.mFlags = MATHSOLVER_INT_NAN;
        return quo;
    }   
             
    Integer rem;
    divAndRem(other, quo, rem);
    return quo;
}

Integer Integer::operator%(const Integer& other) const
{
    if (isUndef() || other.isUndef() || isInf() || other.isInf())
        return Integer("nan");

    Integer quo;
    Integer rem;

    if (other.isZero()) 
    {
        rem.mFlags = MATHSOLVER_INT_NAN;
    }
    else                
    {
        divAndRem(other, quo, rem);
        rem.mSign = (mSign ? !rem.mSign : rem.mSign);
    }

    return rem;
}

Integer& Integer::operator+=(const Integer& other)
{
    if (isUndef() || other.isUndef())
    {
        fromString("nan");
    }
    else if (isInf() || other.isInf())
    {
        if (isInf() && other.isInf() && mSign != other.mSign)   fromString("nan"); // else, unchanged      
        else if (other.isInf())                                 *this = other;
    }
    else if(mSign == other.mSign)
    {
        addAssign(other, mSign);
    }
    else
    {
        int cmp = cmpBytes(mData, mSize, other.mData, other.mSize);
        if (cmp == 0) // Avoid computation: x + -x = 0
        {
            delete[] mData;
            setZero(2);
        }
        else // result != 0
        {        
            subAssign(other, (cmp > 0) ? (mSign) : (other.mSign));
        }
    }

    return *this;
}


Integer& Integer::operator-=(const Integer& other)
{
    if (isUndef() || other.isUndef())
    {
        fromString("nan");
    }
    else if (isInf() || other.isInf())
    {
        if (isInf() && other.isInf() && mSign == other.mSign)   fromString("nan"); // else, unchanged      
        else if (other.isInf())                                 *this = -other;
    }
    else if(mSign == other.mSign)
    {
        int cmp = cmpBytes(mData, mSize, other.mData, other.mSize); 
        if (cmp == 0) // Avoid computation: x - x = 0
        {
            delete[] mData;
            setZero(2);
        }
        else // result != 0
        {
            subAssign(other, (cmp > 0) ? (mSign) : (!mSign));
        }
    }
    else
    {   
        addAssign(other, mSign);
    }

    return *this;
}

Integer& Integer::operator*=(const Integer& other)
{
    if (isUndef() || other.isUndef())     fromString("nan");
    else if (isInf() || other.isInf())
    {
        bool sign = mSign ^ other.mSign;
        fromString("inf");
        mSign = sign;
    }
    else
    {    
        Integer res = mul(other, mSign ^ other.mSign);  
        delete[] mData;
        move(res);
    }

    return *this;
}

Integer& Integer::operator/=(const Integer& other)
{
    if (isUndef() || other.isUndef())   fromString("nan");
    else if (isInf() && other.isInf())  fromString("nan");
    else if (other.isInf())             *this = Integer(0);
    else if (other.isZero()) 
    {
        mFlags = MATHSOLVER_INT_NAN;
    }
    else        
    {
        Integer rem;
        divAssignAndRem(other, rem);
    }

    return *this;
}

Integer& Integer::operator%=(const Integer& other)
{
    if (isUndef() || other.isUndef() || isInf() || other.isInf())
    {
        *this = "nan";
    }
    else
    {    
        Integer quo;
        Integer rem;
        divAndRem(other, quo, rem);
        rem.mSign = (mSign ? !rem.mSign : rem.mSign);
        delete[] mData;
        move(rem);
    }

    return *this;
}

Integer Integer::operator+() const
{  
    Integer res = *this;
    return res;
}

Integer Integer::operator-() const
{
    Integer res = *this;
    res.mSign = !mSign;
    return res;
}


Integer& Integer::operator++()
{
    *this += 1;
    return *this;
}

Integer& Integer::operator--()
{
    *this -= 1;
    return *this;
}

Integer Integer::operator++(int)
{
    Integer ret = *this;
    *this += 1;
    return ret;
}

Integer Integer::operator--(int)
{
    Integer ret = *this;
    *this -= 1;
    return ret;
}

Integer Integer::operator>>(int bits) const
{
    if (isUndef() || isInf())
        return Integer(*this);

    Integer res = *this;
    res.shrAssign(bits);
    return res;
}

Integer Integer::operator<<(int bits) const
{
    if (isUndef() || isInf())
        return Integer(*this);

    Integer res = *this;   
    res.shlAssign(bits);
    return res;
}

Integer& Integer::operator>>=(int bits)
{
    if (!isUndef() && !isInf())
        shrAssign(bits);  
    return *this;
}

Integer& Integer::operator<<=(int bits)
{
    if (!isUndef() && !isInf())
        shlAssign(bits);  
    return *this;
}

std::ostream& operator<<(std::ostream& out, const Integer& integer)
{
    std::string str = integer.toString();
    out << str;
    return out;
}

std::istream& operator>>(std::istream& in, Integer& integer)
{ 
    delete[] integer.mData;
    integer.setZero(2);
    if (in.peek() == '-')
    {
        integer.mSign = true;
        in.get();
    }

    if (!isdigit(in.peek()))    // digitless stream
    {
        integer.mSign = false;
        return in;
    }

    Integer base = 10;
    while (in.peek() != EOF && isdigit(in.peek()))
    {
        integer *= base;
        integer.addAssign((int)(in.get() - '0'), integer.mSign);
    }

    return in;
}

int Integer::compare(const Integer& other) const
{
    if (isUndef() && other.isUndef())                     return 0;                        // (undef, undef)
    if (isInf() && other.isInf() && mSign == other.mSign) return 0;                        // (+inf, +inf), (-inf, -inf)
    if (isInf() && other.isInf())                         return ((mSign) ? -1 : 1);       // (+inf, -inf), (-inf, +inf)
    if (isInf())                                          return ((mSign) ? -1 : 1);       // (+inf, fin), (-inf, fin)
    if (other.isInf())                                    return ((other.mSign) ? -1 : 1); // (n, +inf), (n, -inf)
    if (mSign && other.mSign)                             return -(cmpBytes(mData, mSize, other.mData, other.mSize));  
    if (mSign && !other.mSign)                            return -1;
    if (!mSign && other.mSign)                            return 1;
    else                                                  return cmpBytes(mData, mSize, other.mData, other.mSize);
}

Integer Integer::divRem(const Integer& other, Integer& rem) const
{
    Integer quo;
    divAndRem(other, quo, rem);
    return quo;
}

void Integer::fromString(const std::string& str)
{
    delete[] mData;
    fromStringNoCheck(str);
}

void Integer::set(uint32_t* arr, size_t len, bool sign)
{
    delete[] mData;
    mData = arr;
    mSize = len;
    mFlags = 0;
    mSign = sign;  
}

int Integer::toInt() const
{
    if (mSize > 1 && !rangeIsEmpty(&mData[1], &mData[mSize]))
        gErrorManager.log("Integer to int conversion: value to large, data lost", ErrorManager::WARNING);
    return (mSign ? -1 : 1) * (*((int*)mData));
}

std::string Integer::toString() const
{
    if (mFlags & MATHSOLVER_INT_NAN)
    {
        return "nan";
    }
    else if (mFlags & MATHSOLVER_INT_INF)
    {
        return ((mSign) ? "-" : "") + std::string("inf");
    }
    else
    {
        std::string str;
        Integer tmp = *this;
        Integer base = 10;
        Integer rem = 0;

        while (cmpBytes(tmp.mData, tmp.mSize, base.mData, 1) >= 0)
        {
            tmp.divAssignAndRem(base, rem);
            str = (char)(*((char*)&rem.mData[0]) + '0') + str;
            memset(rem.mData, 0, rem.mSize * 4);
        }

        str = (char)(*((char*)&tmp.mData[0]) + '0') + str;
        return  ((mSign) ? "-" : "") + str;
    }
}

// 
// Helper functions
//

Integer Integer::add(const Integer& other, bool sign) const
{
    uint32_t* l;
    size_t low, high;

    if (cmpBytes(mData, mSize, other.mData, other.mSize) >= 0)
    {
        l = mData;
        high = mSize;
        low = other.mSize;
    }
    else
    {
        l = other.mData;
        high = other.mSize;
        low = mSize;
    }

    uint32_t* r = new uint32_t[high];
    size_t i = 0;
    uint32_t c = 0;
    for (; i < low; ++i)
        c = addByte3(&r[i], mData[i], other.mData[i], c);

   for (; i < high; ++i)
        c = addByte2(&r[i], l[i], c);

    if (c > 0)
    {
        uint32_t* tmp = new uint32_t[high + 1];
        memcpy(tmp, r, high * 4);
        delete[] r;
        
        tmp[high] = c;
        r = tmp;
        ++high;
    }

    return Integer(r, high, sign);
}

void Integer::addAssign(const Integer& other, bool sign)
{
    uint32_t* l;
    size_t low, high;
    mSign = sign;

    if(mSize >= other.mSize)
    {
        l = mData;
        high = mSize;
        low = other.mSize;
    }
    else
    {
        l = other.mData;
        high = other.mSize;
        low = mSize;
        resizeNoCheck(other.mSize); // resize to fit result
    }

    size_t i = 0;
    uint32_t c = 0;
    for (; i < low; ++i)
        c = addByte3(&mData[i], mData[i], other.mData[i], c);

    for (; i < high; ++i)
        c = addByte2(&mData[i], l[i], c);

    if (c > 0)
    {
        resizeNoCheck(high + 1); // resize to fit carry byte
        mData[high] = c;
    }
}

void Integer::divAndRem(const Integer& other, Integer& quo, Integer& rem) const
{
    // TODO: assert(other != 0)
    size_t maxSize = highestNonZeroByte(mData, mSize);
    size_t maxBits = highestNonZeroBit(mData, mSize);

    // reset quo and rem data
    delete[] quo.mData;
    delete[] rem.mData;
    quo.setZero(maxSize);
    rem.setZero(maxSize);
    quo.mSign = mSign ^ other.mSign;

    if (cmpBytes (mData, maxSize, other.mData, other.mSize) < 0) // if a < b, avoid computation: a/b = 0
    {
        rem = *this;
        quo.mSign = false;
        return;
    }

    for (size_t i = maxBits - 1; i < maxBits; --i)
    {
        rem <<= 1;
        setBit(rem.mData, rem.mSize, 0, getBit(mData, mSize, i));
        if (cmpBytes(rem.mData, rem.mSize, other.mData, other.mSize) >= 0)
        {
            rem.subAssign(other, false);
            setBit(quo.mData, quo.mSize, i, 1);
        }
    }
}

void Integer::divAssignAndRem(const Integer& other, Integer& rem)
{
    // TODO: assert(other != 0)
    size_t maxSize = highestNonZeroByte(mData, mSize);
    size_t maxBits = highestNonZeroBit(mData, mSize);

    delete[] rem.mData;
    rem.setZero(maxSize); // set remainder size to this size
    if (cmpBytes (mData, mSize, other.mData, other.mSize) < 0) // if a < b, avoid computation: a/b = 0
    {
        rem = *this;
        mSign = false;
        memset(mData, 0, mSize * 4);
        return;
    }

    rem.mSign = false;
    mSign ^= other.mSign;

    for (size_t i = maxBits - 1; i < maxBits; --i)
    {
        rem <<= 1;
        setBit(rem.mData, rem.mSize, 0, getBit(mData, mSize, i));
        if (cmpBytes(rem.mData, rem.mSize, other.mData, other.mSize) >= 0)
        {
            rem.subAssign(other, false);
            setBit(mData, mSize, i, 1);
        }
        else
        {
            setBit(mData, mSize, i, 0);
        }
    }
}

void Integer::fromStringNoCheck(const std::string& str)
{
    if (str == "nan")
    {
        setZero(2);
        mFlags = MATHSOLVER_INT_NAN;
    }
    else if (str == "inf" || str == "-inf")
    {
        setZero(2);
        mFlags = MATHSOLVER_INT_INF;
        mSign = (str[0] == '-');
    }
    else
    {
        Integer base = 10;
        size_t i = 0;

        setZero(2);
        if (str[0] == '\0' || (str[0] == '-' && str[1] == '\0')) // digitless strings
            return;
        
        if (str[0] == '-')
        {
            mSign = true;
            ++i;
        }

        while (str[i])
        {
            if (!isdigit(str[i]))
                return;

            *this *= base;
            addAssign((int)(str[i] - '0'), mSign);
            ++i;
        }
    }
}

void Integer::move(Integer& other)
{
    mData = other.mData;
    mSize = other.mSize;
    mFlags = other.mFlags;
    mSign = other.mSign;  
    other.mData = nullptr;
}

Integer Integer::mul(const Integer& other, bool sign) const
{
    size_t thisSize = highestNonZeroByte(mData, mSize);
    size_t otherSize = highestNonZeroByte(other.mData, other.mSize);
    size_t maxResSize = thisSize + otherSize;
    size_t maxSize = (maxResSize >= mSize) ? maxResSize : mSize;

    Integer res(new uint32_t[maxSize], maxSize, sign);
    Integer acc(new uint32_t[maxResSize], maxResSize, false);

    memset(acc.mData, 0, maxResSize * 4);
    memset(&res.mData[maxResSize], 0, (maxSize - maxResSize) * 4);
    for (size_t i = 0; i < maxResSize; ++i)
    {
        for (size_t j = 0; j <= i && j < thisSize; ++j)
        {
            size_t k = i - j;
            if (k <= otherSize && k < otherSize)
                acc += Integer((uint64_t)mData[j] * (uint64_t)other.mData[k]);
        }

        res.mData[i] = acc.mData[0];
        acc >>= 32;
    }

    return res;
}

void Integer::resizeNoCheck(size_t size)
{
    assert(size > 0);
    if (size != mSize)
    {
        uint32_t* t = new uint32_t[size];
        if (size > mSize)
        {
            memset(&t[mSize], 0, (size - mSize) * 4); 
            memcpy(t, mData, mSize * 4);
        }
        else
        {
            memcpy(t, mData, size * 4);
        }
        
        delete[] mData; 
        mData = t;
        mSize = size;
    }
}

void Integer::setZero(size_t len)
{
    mData = new uint32_t[len];
    mSize = len;
    mFlags = 0;
    mSign = false;
    memset(mData, 0, len * 4);
}

void Integer::shlAssign(int bits)
{
    assert(mSize + (bits * 32) <= MATHSOLVER_MAX_INT_WIDTH); // ensure that Integer stays below limit
    if (bits < 0)  // no computation 
        return;

    size_t byteShift = bits / 32;
    size_t bitShift = bits % 32;
    size_t maxSize = highestNonZeroByte(mData, mSize) + byteShift + ((bitShift > 0) ? 1 : 0);
    size_t i = maxSize - 1;

    if (mSize < maxSize)        // resize Integer if need more space. Will resize one byte too large if no bit shift.
        resizeNoCheck(maxSize);

    if (bitShift > 0)
    {
        size_t invBitShift = 32 - bitShift; 
        uint32_t highMask = ((invBitShift == 32) ? 0 : (0xFFFFFFFF << invBitShift));
        uint32_t lowMask = ((bitShift == 32) ? 0 : (0xFFFFFFFF >> bitShift));

        if (i > 0)
            mData[i] = (mData[i - byteShift - 1] & highMask) >> invBitShift;

        for (--i; i < maxSize - 1 && i > byteShift; --i)
            mData[i] = ((mData[i - byteShift] & lowMask) << bitShift) | ((mData[i - byteShift - 1] & highMask) >> invBitShift);
        
        if (i < maxSize)
            mData[i] = (mData[i - byteShift] & lowMask) << bitShift;
        --i;
    }
    else
    {
        for (; i >= byteShift && i > 0; --i)
            mData[i] = mData[i - byteShift];
    }

    for (; i < byteShift; --i)
        mData[i] = 0;
}

void Integer::shrAssign(int bits)
{
    if (bits < 0)  // no computation 
        return;

    size_t byteShift = bits / 32;
    size_t bitShift = bits % 32;  
    size_t size = highestNonZeroByte(mData, mSize);
    size_t bytesToShift = (size < byteShift) ? 0 : (size - byteShift);
    size_t i = 0;

    if (bitShift > 0)
    {
        size_t invBitShift = 32 - bitShift;

        uint32_t highMask = ((bitShift == 32) ? 0 : (0xFFFFFFFF << bitShift));
        uint32_t lowMask = ((invBitShift == 32) ? 0 : (0xFFFFFFFF >> invBitShift));

        for (; i < bytesToShift - 1; ++i)
            mData[i] = ((mData[i + byteShift + 1] & lowMask) << invBitShift) | ((mData[i + byteShift] & highMask) >> bitShift); 
        mData[i] = ((mData[i + byteShift] & highMask) >> bitShift); 
        ++i;
    }
    else
    {
        for (; i < bytesToShift; ++i)
            mData[i] = mData[i + byteShift];
    }
    
    for(; i < mSize; ++i)
        mData[i] = 0;
}

Integer Integer::sub(const Integer& other, bool sign) const
{
    uint32_t *l, *s;
    size_t low, high;

    if (cmpBytes(mData, mSize, other.mData, other.mSize) >= 0)
    {
        l = mData;
        s = other.mData;
        high = mSize;
        low = other.mSize;
    }
    else
    {
        l = other.mData;
        s = mData;
        high = other.mSize;
        low = mSize;
    }

    uint32_t* r = new uint32_t[high];
    size_t i = 0;
    uint32_t c = 1;
    for (; i < low; ++i)
        c = addByte3(&r[i], l[i], ~s[i], c);

   for (; i < high; ++i)
        c = addByte3(&r[i], l[i], 0xFFFFFFFF, c);

    return Integer(r, high, sign);
}

void Integer::subAssign(const Integer& other, bool sign)
{
    size_t thisSize = highestNonZeroByte(mData, mSize);
    size_t otherSize = highestNonZeroByte(other.mData, other.mSize);
    uint32_t *l, *s;
    size_t low, high;
    mSign = sign;

    if (cmpBytes(mData, thisSize, other.mData, otherSize) >= 0)
    {
        l = mData;
        s = other.mData;
        high = thisSize;
        low = otherSize;
    }
    else
    {
        l = other.mData;
        s = mData;
        high = otherSize;
        low = thisSize;
    }

    if (mSize < high)
        resizeNoCheck(high);

    size_t i = 0;
    uint32_t c = 1;
    for (; i < low; ++i)
        c = addByte3(&mData[i], l[i], ~s[i], c);

   for (; i < high; ++i)
        c = addByte3(&mData[i], l[i], 0xFFFFFFFF, c);
}

} // END MathSolver namespace