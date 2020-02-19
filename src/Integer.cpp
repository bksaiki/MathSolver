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

Integer::~Integer()
{
    if(mData != nullptr)
        delete[] mData;
    mData = nullptr;
}

Integer& Integer::operator=(const Integer& other)
{
    mData = new uint8_t[other.mSize];
    mSize = other.mSize;
    mSign = other.mSign;
    memcpy(mData, other.mData, mSize);
    return *this;
}

Integer& Integer::operator=(Integer&& other)
{
    mData = other.mData;
    mSize = other.mSize;
    mSign = other.mSign;
    other.mData = nullptr;
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
    uint8_t* r_arr;
    size_t r_size;
    bool r_sign;

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

Integer Integer::operator-() const
{
    Integer res = *this;
    res.mSign = !mSign;
    return res;
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