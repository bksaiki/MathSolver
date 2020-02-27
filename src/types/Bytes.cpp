#include <cassert>
#include "Bytes.h"

namespace Mathsolver
{

uint8_t addByte2(uint8_t* res, uint8_t a, uint8_t b)
{
    uint16_t r = (uint16_t)a + (uint16_t)b;
    *res = r & 0x00FF;
    return (r >> 8);
}

// Adds a triple of single bytes, stores the resultant uint8_t at the location specified by
// res, and returns the overflow. 
uint8_t addByte3(uint8_t* res, uint8_t a, uint8_t b, uint8_t c)
{
    uint16_t r = (uint16_t)a + (uint16_t)b + (uint16_t)c;
    *res = r & 0x00FF;
    return (r >> 8);
}

int cmpBytes(uint8_t* a, size_t alen, uint8_t* b, size_t blen)
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
        int16_t d = (int16_t)a[i] - (int16_t)b[i];
        if (d > 0)      return 1;
        else if (d < 0) return -1;
    }

    return 0;
}

uint8_t getBit(uint8_t* x, size_t len, size_t bit)
{
    assert(bit <= 8 * len);
    size_t byteOffset = bit / 8;
    size_t bitOffset = bit % 8;
    
    if (bitOffset == 0)         return x[byteOffset] & 0x01;
    else if (bitOffset == 1)    return (x[byteOffset] >> 1) & 0x01;
    else if (bitOffset == 2)    return (x[byteOffset] >> 2) & 0x01;
    else if (bitOffset == 3)    return (x[byteOffset] >> 3) & 0x01;
    else if (bitOffset == 4)    return (x[byteOffset] >> 4) & 0x01;
    else if (bitOffset == 5)    return (x[byteOffset] >> 5) & 0x01;
    else if (bitOffset == 6)    return (x[byteOffset] >> 6) & 0x01;
    else                        return (x[byteOffset] >> 7) & 0x01;
}

size_t highestNonZeroByte(uint8_t* x, size_t len)
{
    for (size_t i = len - 1; i < len; --i)
        if (x[i])
            return i + 1;
    return 0;
}

bool rangeIsEmpty(uint8_t* low, uint8_t* high)
{
    assert(high >= low);
    for (uint8_t* it = low; it != high; ++it)
    {
        if (*it != 0)
            return false;
    }

    return true;
}

void setBit(uint8_t* x, size_t len, size_t bit, bool value)
{
    assert(bit <= 8 * len);
    size_t byteOffset = bit / 8;
    size_t bitOffset = bit % 8;
    size_t invBitOffset = 8 - bitOffset;
    uint8_t highMask = 0xFF << (bitOffset + 1);
    uint8_t lowMask = 0xFF >> invBitOffset;
    x[byteOffset] = (x[byteOffset] & highMask) | ((uint8_t)value << bitOffset) | (x[byteOffset] & lowMask);
}

} // END MathSolver namespace
