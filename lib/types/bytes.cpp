#include <cassert>
#include "bytes.h"

namespace MathSolver
{

uint32_t addByte2(uint32_t* res, uint32_t a, uint32_t b)
{
    uint64_t r = (uint64_t)a + (uint64_t)b;
    *res = r & (uint64_t)0xFFFFFFFF;
    return (r >> 32);
}

uint32_t addByte3(uint32_t* res, uint32_t a, uint32_t b, uint32_t c)
{
    uint64_t r = (uint64_t)a + (uint64_t)b + (uint64_t)c;
    *res = r & (uint64_t)0xFFFFFFFF;
    return (r >> 32);
}

int cmpBytes(uint32_t* a, size_t alen, uint32_t* b, size_t blen)
{
    size_t low;
    if (alen > blen) // check extra bytes
    {
        if (!rangeIsEmpty(&a[blen], &a[alen]))
            return 1;
        low = blen;
    }
    else if (blen > alen)
    {
        if (!rangeIsEmpty(&b[alen], &b[blen]))
            return -1;
        low = alen;
    }
    else
    {
        low = alen;
    }
    
    for (size_t i = low - 1; i < low; --i)
    {   
        int64_t d = (int64_t)a[i] - (int64_t)b[i];
        if (d > 0)      return 1;
        else if (d < 0) return -1;
    }

    return 0;
}

uint32_t getBit(uint32_t* x, size_t len, size_t bit)
{
    assert(bit <= 32 * len);
    size_t byteOffset = bit / 32;
    size_t bitOffset = bit % 32;
    return (x[byteOffset] >> bitOffset) & 0x1;
}

size_t highestNonZeroBit(uint32_t* x, size_t len)
{
    for (size_t i = len - 1; i < len; --i)
    {
        size_t mul = 0x80000000;
        for (size_t j = 31; j < 32; --j, mul /= 2)
        {
            if (x[i] & mul)
                return (32 * i) + j + 1;
        }

    }

    return 0;
}

size_t highestNonZeroByte(uint32_t* x, size_t len)
{
    for (size_t i = len - 1; i < len; --i)
        if (x[i]) return i + 1;
    return 0;
}

bool rangeIsEmpty(uint32_t* low, uint32_t* high)
{
    assert(high >= low);
    for (uint32_t* it = low; it != high; ++it)
    {
        if (*it != 0)
            return false;
    }

    return true;
}

void setBit(uint32_t* x, size_t len, size_t bit, bool value)
{
    assert(bit <= 32 * len);
    uint32_t byteOffset = bit / 32;
    uint32_t bitOffset = bit % 32;
    uint32_t invBitOffset = 32 - bitOffset;
    uint32_t highMask = ((bitOffset == 31) ? 0x7FFFFFFF : (0xFFFFFFFF << (bitOffset + 1)));
    uint32_t lowMask = ((invBitOffset == 32) ? 0 : (0xFFFFFFFF >> invBitOffset));
    x[byteOffset] = (x[byteOffset] & highMask) | ((uint32_t)value << bitOffset) | (x[byteOffset] & lowMask);
}

} // END MathSolver namespace
