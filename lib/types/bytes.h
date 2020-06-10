#ifndef _MATHSOLVER_BYTES_H_
#define _MATHSOLVER_BYTES_H_

#include <stdint.h>
#include <stddef.h>
#include "../common/base.h"

namespace MathSolver
{

// Adds a pair of quad bytes, stores the resultant uint32_t at the location specified by
// res, and returns the overflow. 
uint32_t addByte2(uint32_t* res, uint32_t a, uint32_t b);

// Adds a triple of quad bytes, stores the resultant uint32_t at the location specified by
// res, and returns the overflow. 
uint32_t addByte3(uint32_t* res, uint32_t a, uint32_t b, uint32_t c);

// Compares two quad byte arrays by highest non-zero bit. Returns 1 if a is greater than b,
// 0 if a equals b, and -1 if a is less than b.
int cmpBytes(uint32_t* a, size_t alen, uint32_t* b, size_t blen);

// Returns the value of a given bit in a quad byte array. Assertion: bit < 8 * len.
uint32_t getBit(uint32_t* x, size_t len, size_t bit);

// Returns the highest non-zero bit. Returns 0 if all bytes are zero.
size_t highestNonZeroBit(uint32_t* x, size_t len);

// Returns one higher than the index of the last non-zero quad byte in a given quad byte array.
// (Returns 0 if all bytes are zero.)
size_t highestNonZeroByte(uint32_t* x, size_t len);

// Returns true if all bytes within the specified range are equal to 0. Returns false
// otherwise. Assertion: high >= low.
bool rangeIsEmpty(uint32_t* low, uint32_t* high);

// Sets a given bit in a quad byte array. Assertion: bit < 8 * len.
void setBit(uint32_t* x, size_t len, size_t bit, bool value);

} // END MathSolver namespace

#endif
