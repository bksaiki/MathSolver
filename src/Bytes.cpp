#include <cassert>
#include "../include/Bytes.h"

namespace MathSolver
{

uint8_t addByte2(uint8_t* res, uint8_t a, uint8_t b)
{
    uint16_t r = (uint16_t)a + (uint16_t)b;
    *res = r & 0x00FF;
    return (r >> 8);
}

uint8_t addByte3(uint8_t* res, uint8_t a, uint8_t b, uint8_t c)
{
    uint16_t r = (uint16_t)a + (uint16_t)b + (uint16_t)c;
    *res = r & 0x00FF;
    return (r >> 8);
}

int cmpBytes(uint8_t* a, uint8_t* b, size_t alen, size_t blen)
{
    size_t low;
    if (alen > blen) // check extra bytes
    {
        if (!rangeIsEmtpy(&a[blen], &a[alen]))
            return 1;
        low = blen;
    }
    else if (blen > alen)
    {
        if (!rangeIsEmtpy(&b[alen], &b[blen]))
            return 1;
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

size_t highestNonZeroByte(uint8_t* x, size_t len)
{
    for (size_t i = len - 1; i < len; --i)
    {
        if (x[i] == 0)
            return i + 1;
    }

    return 0;
}

bool rangeIsEmtpy(uint8_t* low, uint8_t* high)
{
    assert(high >= low);
    uint8_t* it = low;
    for (; it != high; ++it)
    {
        if (*it != 0)
            return false;
    }

    return true;
}

}