#include <cassert>
#include <cstring>
#include "../include/Integer.h"

namespace MathSolver
{

Integer::Integer()
{
    mData = new uint8_t[MATHSOLVER_DEFAULT_INT_WIDTH];
    mSize = MATHSOLVER_DEFAULT_INT_WIDTH;
    mSign = false;
    memset(mData, 0, MATHSOLVER_DEFAULT_INT_WIDTH);
}

Integer::Integer(const Integer& other)
{
    mData = new uint8_t[other.mSize];
    mSize = other.mSize;
    mSign = other.mSign;
    memcpy(mData, other.mData, mSize);
}

Integer::Integer(Integer&& other)
{
    mData = other.mData;
    mSize = other.mSize;
    mSign = other.mSign;
    other.mData = nullptr;
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

Integer::Integer(signed int x)
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
    mData = new uint8_t[MATHSOLVER_DEFAULT_INT_WIDTH];
    mSize = MATHSOLVER_DEFAULT_INT_WIDTH;
    mSign = false;
    memset(mData, 0, MATHSOLVER_DEFAULT_INT_WIDTH);

    size_t i = 0;
    while (str[i])
    {
        if (!isdigit(str[i]))
            return;

        *this *= (int)(10);
        *this += (int)(str[i] - '0');
        ++i;
    }
}

Integer::Integer(const std::string& str)
{
    mData = new uint8_t[MATHSOLVER_DEFAULT_INT_WIDTH];
    mSize = MATHSOLVER_DEFAULT_INT_WIDTH;
    mSign = false;
    memset(mData, 0, MATHSOLVER_DEFAULT_INT_WIDTH);

    size_t i = 0;
    while (str[i])
    {
        if (!isdigit(str[i]))
            return;

        *this *= (int)(10);
        *this += (int)(str[i] - '0');
        ++i;
    }
}

Integer::~Integer()
{
    if(mData != nullptr)
        delete[] mData;
    mData = nullptr;
}

Integer& Integer::operator=(const Integer& other)
{
    if (mData != nullptr)
        delete[] mData;

    mData = new uint8_t[other.mSign];
    mSize = other.mSize;
    mSign = other.mSign;
    memcpy(mData, other.mData, mSize);
    return *this;
}

Integer& Integer::operator=(Integer&& other)
{
    if (mData != nullptr)
        delete[] mData;
        
    mData = other.mData;
    mSize = other.mSize;
    mSign = other.mSign;
    other.mData = nullptr;
    return *this;
}

Integer& Integer::operator=(const char* str)
{
    if (mData != nullptr)
        delete[] mData;

    mData = new uint8_t[MATHSOLVER_DEFAULT_INT_WIDTH];
    mSize = MATHSOLVER_DEFAULT_INT_WIDTH;
    mSign = false;
    memset(mData, 0, MATHSOLVER_DEFAULT_INT_WIDTH);

    size_t i = 0;
    while (str[i])
    {
        if (!isdigit(str[i]))
            return *this;

        *this *= (int)(10);
        *this += (int)(str[i] - '0');
        ++i;
    }

    return *this;
}

Integer& Integer::operator=(const std::string& str)
{
    if (mData != nullptr)
        delete[] mData;

    mData = new uint8_t[MATHSOLVER_DEFAULT_INT_WIDTH];
    mSize = MATHSOLVER_DEFAULT_INT_WIDTH;
    mSign = false;
    memset(mData, 0, MATHSOLVER_DEFAULT_INT_WIDTH);

    size_t i = 0;
    char c = str[i];
    while (c)
    {
        if (!isdigit(c))
            return *this;

        *this *= (int)(10);
        *this += (int)(c - '0');
        ++i;
        c = str[i];
    }

    return *this;
}

Integer Integer::operator+(const Integer& other) const
{
    if(mSign == other.mSign)
    {
        return add(other, mSign);
    }
    else
    {
        int cmp = cmpBytes(mData, other.mData, mSize, other.mSize);
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
    int cmp = cmpBytes(mData, other.mData, mSize, other.mSize);
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
    size_t thisSize = highestNonZeroByte(mData, mSize);
    size_t otherSize = highestNonZeroByte(other.mData, other.mSize);
    size_t maxSize = thisSize + otherSize;

    Integer res(new uint8_t[maxSize], maxSize, mSign ^ other.mSign);
    Integer acc(new uint8_t[maxSize], maxSize, false);

    memset(res.mData, 0, maxSize);
    memset(acc.mData, 0, maxSize);
    for (size_t i = 0; i < maxSize; ++i)
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

Integer& Integer::operator+=(const Integer& other)
{
    if(mSign == other.mSign)
    {
        addAssign(other, mSign);
    }
    else
    {
        int cmp = cmpBytes(mData, other.mData, mSize, other.mSize);
        if (cmp == 0) // Avoid computation: x + -x = 0
        {
            uint8_t* arr = new uint8_t[1];
            arr[0] = 0;
            set(arr, 1, false);
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
    int cmp = cmpBytes(mData, other.mData, mSize, other.mSize); 
    if(mSign == other.mSign)
    {
        if (cmp == 0) // Avoid computation: x - x = 0
        {
            uint8_t* arr = new uint8_t[1];
            arr[0] = 0;
            set(arr, 1, false);
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
    size_t thisSize = highestNonZeroByte(mData, mSize);
    size_t otherSize = highestNonZeroByte(other.mData, other.mSize);
    size_t maxResSize = thisSize + otherSize;
    size_t maxSize = (maxResSize > mSize) ? maxResSize : mSize;

    Integer res(new uint8_t[maxSize], maxSize, mSign ^ other.mSign);
    Integer acc(new uint8_t[maxSize], maxSize, false);

    memset(res.mData, 0, maxSize);
    memset(acc.mData, 0, maxSize);
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

    *this = res;
    return *this;
}

Integer Integer::operator-() const
{
    Integer res = *this;
    res.mSign = !mSign;
    return res;
}

Integer& Integer::operator>>=(size_t bits)
{
    assert(8 * mSize >= bits);
    if (bits == 0)  // no computation 
        return *this;

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

        for (; i < bytesToShift; ++i)
            mData[i] = ((mData[i + byteShift + 1] & lowMask) << invBitShift) | ((mData[i + byteShift] & highMask) >> bitShift); 
        mData[i] = (mData[i + byteShift] & lowMask);
        ++i;
    }
    else
    {
        for (; i < bytesToShift; ++i)
            mData[i] = mData[i + byteShift];
    }
    
    for(; i < mSize; ++i)
        mData[i] = 0;

    return *this;
}

Integer& Integer::operator<<=(size_t bits)
{
    assert(mSize + (bits * 8) <= MATHSOLVER_MAX_INT_WIDTH); // ensure that Integer stays below limit
    if (bits == 0)  // no computation 
        return *this;

    size_t byteShift = bits / 8;
    size_t bitShift = bits % 8;
    size_t maxSize = highestNonZeroByte(mData, mSize) + byteShift + ((bitShift > 0) ? 1 : 0);
    size_t i = maxSize - 1;

    if (maxSize > mSize)        // resize Integer if need more space. Will resize one byte too large if no bit shift.
        resizeNoCheck(maxSize);

    if (bitShift > 0)
    {
        size_t invBitShift = 8 - bitShift; 
        uint8_t highMask = 0xFF << bitShift;
        uint8_t lowMask = 0xFF >> invBitShift;

        mData[i] = (mData[i - byteShift - 1] & highMask) >> bitShift;
        for (--i; i > byteShift && i > 0; --i)
            mData[i] = ((mData[i - byteShift] & lowMask) << invBitShift) | ((mData[i - byteShift - 1] & highMask) >> bitShift);   
        mData[i] = (mData[i - byteShift] & lowMask) << invBitShift;
        --i;
    }
    else
    {
        for (; i >= byteShift && i > 0; --i)
            mData[i] = mData[i - byteShift];
    }

    for (; i < byteShift; --i)
            mData[i] = 0;
    return *this;
}

void Integer::set(uint8_t* arr, size_t len, bool sign)
{
    if (mData != nullptr)
        delete[] mData;
    mData = arr;
    mSize = len;
    mSign = sign;
}

// 
// Helper functions
//

Integer Integer::add(const Integer& other, bool sign) const
{
    uint8_t* l;
    size_t low, high;

    if (cmpBytes(mData, other.mData, mSize, other.mSize) > 0)
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

    if (cmpBytes(mData, other.mData, mSize, other.mSize) > 0)
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
        resizeNoCheck(high); // must resize to store result
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

void Integer::resizeNoCheck(size_t size)
{
    assert(size > 0);
    if (size != mSize) // larger requested size
    {
        uint8_t* t = new uint8_t[size];
        if (size > mSize)
        {
            memcpy(t, mData, size);
            memset(&t[mSize], 0, size - mSize);
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

Integer Integer::sub(const Integer& other, bool sign) const
{
    uint8_t *l, *s;
    size_t low, high;

    if (cmpBytes(mData, other.mData, mSize, other.mSize) > 0)
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
    uint8_t *l, *s;
    size_t low, high;
    mSign = sign;

    if (cmpBytes(mData, other.mData, mSize, other.mSize) > 0)
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
        resizeNoCheck(high); // must resize to store result
    }

    size_t i = 0;
    uint8_t c = 1;
    for (; i < low; ++i)
        c = addByte3(&mData[i], l[i], ~s[i], c);

   for (; i < high; ++i)
        c = addByte3(&mData[i], l[i], 0xFF, c);
}

} // END mathsolver namespace