#ifndef _MATHSOLVER_BYTES_H_
#define _MATHSOLVER_BYTES_H_

#include <stdint.h>
#include <stddef.h>

namespace Mathsolver
{

// Adds a pair of single bytes, stores the resultant uint8_t at the location specified by
// res, and returns the overflow. 
uint8_t addByte2(uint8_t* res, uint8_t a, uint8_t b);

// Adds a triple of single bytes, stores the resultant uint8_t at the location specified by
// res, and returns the overflow. 
uint8_t addByte3(uint8_t* res, uint8_t a, uint8_t b, uint8_t c);

// Compares two byte arrays by highest non-zero bit. Returns 1 if a is greater than b,
// 0 if a equals b, and -1 if a is less than b.
int cmpBytes(uint8_t* a, size_t alen, uint8_t* b, size_t blen);

// Returns the value of a given bit in a byte array. Assertion: bit < 8 * len.
uint8_t getBit(uint8_t* x, size_t len, size_t bit);

// Returns the highest non-zero bit. Returns 0 if all bytes are zero.
size_t highestNonZeroBit(uint8_t* x, size_t len);

// Returns one higher than the index of the last non-zero byte in a given byte array.
// (Returns 0 if all bytes are zero.)
size_t highestNonZeroByte(uint8_t* x, size_t len);


// Returns true if all bytes within the specified range are equal to 0. Returns false
// otherwise. Assertion: high >= low.
bool rangeIsEmpty(uint8_t* low, uint8_t* high);

// Sets a given bit in a byte array. Assertion: bit < 8 * len.
void setBit(uint8_t* x, size_t len, size_t bit, bool value);

} // END MathSolver namespace

#endif
