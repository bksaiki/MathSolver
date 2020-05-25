#include <cassert>
#include <cstring>
#include "integer.h"

namespace MathSolver
{

Integer::Integer()
{
    setZero(MATHSOLVER_DEFAULT_INT_WIDTH);
}

Integer::Integer(const Integer& other)
{
    mData = new uint8_t[other.mSize];
    mSize = other.mSize;
    mSign = other.mSign;
    memcpy(mData, other.mData, other.mSize);
}

Integer::Integer(Integer&& other)
{
    move(other);
}

Integer::Integer(uint8_t* arr, size_t len, bool sign)
{
    mData = arr;
    mSize = len;
    mSign = sign;
}

Integer::Integer(unsigned int x)
{
    mData = new uint8_t[4];
    mSize = 4;
    mSign = false;
    memcpy(mData, &x, 4);
}

Integer::Integer(signed x)
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
    
    mData = new uint8_t[4];
    mSize = 4;
    memcpy(mData, &x, 4);
}

Integer::Integer(const char* str)
{
    fromString(str);
}

Integer::Integer(const std::string& str)
{   
    fromString(str.c_str());
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
        mData = new uint8_t[other.mSize];
        mSize = other.mSize;
        mSign = other.mSign;
        memcpy(mData, other.mData, other.mSize);
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

Integer& Integer::operator=(const char* str)
{
    delete[] mData;
    fromString(str);

    return *this;
}

Integer& Integer::operator=(const std::string& str)
{
    delete[] mData;
    fromString(str.c_str());

    return *this;
}

bool Integer::operator==(const Integer& other) const
{
    return (mSign == other.mSign) && (cmpBytes(mData, mSize, other.mData, other.mSize) == 0);
}

bool Integer::operator!=(const Integer& other) const
{
    return (mSign != other.mSign) || (cmpBytes(mData, mSize, other.mData, other.mSize) != 0);
}

bool Integer::operator>(const Integer& other) const
{
    if (mSign && other.mSign)           return (cmpBytes(mData, mSize, other.mData, other.mSize) < 0);
    else if (mSign && !other.mSign)     return false;
    else if (!mSign && other.mSign)     return true;
    else                                return (cmpBytes(mData, mSize, other.mData, other.mSize) > 0);
}

bool Integer::operator<(const Integer& other) const
{
    if (mSign && other.mSign)           return (cmpBytes(mData, mSize, other.mData, other.mSize) > 0);
    else if (mSign && !other.mSign)     return true;
    else if (!mSign && other.mSign)     return false;
    else                                return (cmpBytes(mData, mSize, other.mData, other.mSize) < 0);
}

bool Integer::operator>=(const Integer& other) const
{
    if (mSign && other.mSign)           return (cmpBytes(mData, mSize, other.mData, other.mSize) <= 0);
    else if (mSign && !other.mSign)     return false;
    else if (!mSign && other.mSign)     return true;
    else                                return (cmpBytes(mData, mSize, other.mData, other.mSize) >= 0);
}

bool Integer::operator<=(const Integer& other) const
{
    if (mSign && other.mSign)           return (cmpBytes(mData, mSize, other.mData, other.mSize) >= 0);
    else if (mSign && !other.mSign)     return true;
    else if (!mSign && other.mSign)     return false;
    else                                return (cmpBytes(mData, mSize, other.mData, other.mSize) <= 0);
}

Integer Integer::operator+(const Integer& other) const
{
    if(mSign == other.mSign)
    {
        return add(other, mSign);
    }
    else
    {
        int cmp = cmpBytes(mData, mSize, other.mData, other.mSize);
        if (cmp == 0) // Avoid computation: x + -x = 0
        {
            uint8_t* arr = new uint8_t[1];
            arr[0] = 0;
            return Integer(arr, 1, false);
        }
        else // result != 0
        {        
            return sub(other, (cmp > 0) ? (mSign) : (other.mSign));
        }
    }
}

Integer Integer::operator-(const Integer& other) const
{
    int cmp = cmpBytes(mData, mSize, other.mData, other.mSize);
    if (mSign == other.mSign)
    {
        return sub(other, (cmp > 0) ? (mSign) : (!mSign));
    }
    else
    {
        if (cmp == 0) // Avoid computation: x + -x = 0
        {
            uint8_t* arr = new uint8_t[1];
            arr[0] = 0;
            return Integer(arr, 1, false);
        }
        else // result != 0
        {        
            return add(other, mSign);
        }
    }
}

Integer Integer::operator*(const Integer& other) const
{
    return mul(other, mSign ^ other.mSign);
}

Integer Integer::operator/(const Integer& other) const
{
    Integer quo;
    Integer rem;
    divAndRem(other, quo, rem);
    return quo;
}

Integer Integer::operator%(const Integer& other) const
{
    Integer quo;
    Integer rem;
    divAndRem(other, quo, rem);
    rem.mSign = (mSign ? !rem.mSign : rem.mSign);
    return rem;
}

Integer& Integer::operator+=(const Integer& other)
{
    if(mSign == other.mSign)
    {
        addAssign(other, mSign);
    }
    else
    {
        int cmp = cmpBytes(mData, mSize, other.mData, other.mSize);
        if (cmp == 0) // Avoid computation: x + -x = 0
        {
            delete[] mData;
            setZero(MATHSOLVER_DEFAULT_INT_WIDTH);
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
    int cmp = cmpBytes(mData, mSize, other.mData, other.mSize); 
    if(mSign == other.mSign)
    {
        if (cmp == 0) // Avoid computation: x - x = 0
        {
            delete[] mData;
            setZero(MATHSOLVER_DEFAULT_INT_WIDTH);
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
    Integer res = mul(other, mSign ^ other.mSign);  
    delete[] mData;
    move(res);
    return *this;
}

Integer& Integer::operator/=(const Integer& other)
{
    Integer rem;
    divAssignAndRem(other, rem);
    return *this;
}

Integer& Integer::operator%=(const Integer& other)
{
    Integer quo;
    Integer rem;
    divAndRem(other, quo, rem);
    rem.mSign = (mSign ? !rem.mSign : rem.mSign);
    delete[] mData;
    move(rem);
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
    Integer res = *this;
    res.shrAssign(bits);
    return res;
}

Integer Integer::operator<<(int bits) const
{
    Integer res = *this;   
    res.shlAssign(bits);
    return res;
}

Integer& Integer::operator>>=(int bits)
{
    shrAssign(bits);
    return *this;
}

Integer& Integer::operator<<=(int bits)
{
    shlAssign(bits);
    return *this;
}

Integer::operator bool() const
{
    return !(rangeIsEmpty(mData, &mData[mSize]));
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
    integer.setZero(MATHSOLVER_DEFAULT_INT_WIDTH);
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

Integer Integer::divRem(const Integer& other, Integer& rem) const
{
    Integer quo;
    divAndRem(other, quo, rem);
    return quo;
}

void Integer::fromString(const std::string& str)
{
    delete[] mData;
    fromString(str.c_str());
}

void Integer::set(uint8_t* arr, size_t len, bool sign)
{
    delete[] mData;
    mData = arr;
    mSize = len;
    mSign = sign;
}

int Integer::toInt() const
{
    if (mSize < 4)
    {
        uint8_t* arr = new uint8_t[4];
        for (size_t i = 0; i < 4; ++i) arr[i] = 0;
        memcpy(arr, mData, mSize);
        return (mSign ? -1 : 1) * (*((int*)arr));
    }

    if (mSize > 4 && !rangeIsEmpty(&mData[4], &mData[mSize]))
        gErrorManager.log("Integer to int conversion: value to large, data lost", ErrorManager::WARNING);
    return (mSign ? -1 : 1) * (*((int*)mData));
}

std::string Integer::toString() const
{
    std::string str = "";
    Integer tmp = *this;
    Integer base = 10;
    Integer rem = 0;
    while (cmpBytes(tmp.mData, tmp.mSize, base.mData, 1) >= 0)
    {
        tmp.divAssignAndRem(base, rem);
        str = (char)((char)rem.mData[0] + '0') + str;
        memset(rem.mData, 0, rem.mSize);
    }

    str = (char)((char)tmp.mData[0] + '0') + str;
    return  ((mSign) ? "-" : "") + str;
}

// 
// Helper functions
//

Integer Integer::add(const Integer& other, bool sign) const
{
    uint8_t* l;
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

    uint8_t* r = new uint8_t[high];
    size_t i = 0;
    uint8_t c = 0;
    for (; i < low; ++i)
        c = addByte3(&r[i], mData[i], other.mData[i], c);

   for (; i < high; ++i)
        c = addByte2(&r[i], l[i], c);

    if (c > 0)
    {
        uint8_t* tmp = new uint8_t[high + 1];
        memcpy(tmp, r, high);
        delete[] r;
        
        tmp[high] = c;
        r = tmp;
        ++high;
    }

    return Integer(r, high, sign);
}

void Integer::addAssign(const Integer& other, bool sign)
{
    uint8_t* l;
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
    uint8_t c = 0;
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
        memset(mData, 0, mSize);
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

void Integer::fromString(const char* str)
{
    Integer base = 10;
    size_t i = 0;

    setZero(MATHSOLVER_DEFAULT_INT_WIDTH);
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

void Integer::move(Integer& other)
{
    mData = other.mData;
    mSize = other.mSize;
    mSign = other.mSign;
    other.mData = nullptr;
}

Integer Integer::mul(const Integer& other, bool sign) const
{
    size_t thisSize = highestNonZeroByte(mData, mSize);
    size_t otherSize = highestNonZeroByte(other.mData, other.mSize);
    size_t maxResSize = thisSize + otherSize;
    size_t maxSize = (maxResSize >= mSize) ? maxResSize : mSize;

    Integer res(new uint8_t[maxSize], maxSize, sign);
    Integer acc(new uint8_t[maxResSize], maxResSize, false);

    memset(acc.mData, 0, maxResSize);
    memset(&res.mData[maxResSize], 0, maxSize - maxResSize);
    for (size_t i = 0; i < maxResSize; ++i)
    {
        for (size_t j = 0; j <= i && j < thisSize; ++j)
        {
            size_t k = i - j;
            if (k <= otherSize)
                acc += Integer((uint32_t)mData[j] * (uint32_t)other.mData[k]);
        }

        res.mData[i] = acc.mData[0];
        acc >>= 8;
    }

    return res;
}

void Integer::resizeNoCheck(size_t size)
{
    assert(size > 0);
    if (size != mSize)
    {
        uint8_t* t = new uint8_t[size];
        if (size > mSize)
        {
            memset(&t[mSize], 0, size - mSize); 
            memcpy(t, mData, mSize);
        }
        else
        {
            memcpy(t, mData, size);
        }
        
        delete[] mData; 
        mData = t;
        mSize = size;
    }
}

void Integer::setZero(size_t len)
{
    mData = new uint8_t[len];
    mSize = len;
    mSign = false;
    memset(mData, 0, len);
}

void Integer::shlAssign(int bits)
{
    assert(mSize + (bits * 8) <= MATHSOLVER_MAX_INT_WIDTH); // ensure that Integer stays below limit
    if (bits < 0)  // no computation 
        return;

    size_t byteShift = bits / 8;
    size_t bitShift = bits % 8;
    size_t maxSize = highestNonZeroByte(mData, mSize) + byteShift + ((bitShift > 0) ? 1 : 0);
    size_t i = maxSize - 1;

    if (mSize < maxSize)        // resize Integer if need more space. Will resize one byte too large if no bit shift.
        resizeNoCheck(maxSize);

    if (bitShift > 0)
    {
        size_t invBitShift = 8 - bitShift; 
        uint8_t highMask = 0xFF << invBitShift;
        uint8_t lowMask = 0xFF >> bitShift;

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

    size_t byteShift = bits / 8;
    size_t bitShift = bits % 8;  
    size_t size = highestNonZeroByte(mData, mSize);
    size_t bytesToShift = (size < byteShift) ? 0 : (size - byteShift);
    size_t i = 0;

    if (bitShift > 0)
    {
        size_t invBitShift = 8 - bitShift;
        uint8_t highMask = 0xFF << bitShift;
        uint8_t lowMask = 0xFF >> invBitShift;

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
    uint8_t *l, *s;
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

    uint8_t* r = new uint8_t[high];
    size_t i = 0;
    uint8_t c = 1;
    for (; i < low; ++i)
        c = addByte3(&r[i], l[i], ~s[i], c);

   for (; i < high; ++i)
        c = addByte3(&r[i], l[i], 0xFF, c);

    return Integer(r, high, sign);
}

void Integer::subAssign(const Integer& other, bool sign)
{
    size_t thisSize = highestNonZeroByte(mData, mSize);
    size_t otherSize = highestNonZeroByte(other.mData, other.mSize);
    uint8_t *l, *s;
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
    uint8_t c = 1;
    for (; i < low; ++i)
        c = addByte3(&mData[i], l[i], ~s[i], c);

   for (; i < high; ++i)
        c = addByte3(&mData[i], l[i], 0xFF, c);
}

} // END MathSolver namespace