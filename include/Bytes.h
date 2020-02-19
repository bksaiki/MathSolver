#ifndef _MATHSOLVER_BYTES_H_
#define _MATHSOLVER_BYTES_H_

#include <stdint.h>
#include <stddef.h>

namespace MathSolver
{

// Adds a pair of single bytes, stores the resultant uint8_t at the location specified by
// res, and returns the overflow. 
uint8_t addByte2(uint8_t* res, uint8_t a, uint8_t b);

// Adds a triple of single bytes, stores the resultant uint8_t at the location specified by
// res, and returns the overflow. 
uint8_t addByte3(uint8_t* res, uint8_t a, uint8_t b, uint8_t c);

// Adds a and b and returns a pointer to the result result. The width of a and b must be
// specified. The size of the result will either be the the larger of the two sizes or one
// byte larger. A reference to a variable to store the size of the variable must be passed.
uint8_t* addBytes(uint8_t* a, uint8_t* b, size_t alen, size_t blen, size_t& rlen);

// Compares two byte arrays by highest non-zero bit. Returns 1 if a is greater than b,
// 0 if a equals b, and -1 if a is less than b.
int cmpBytes(uint8_t* a, uint8_t* b, size_t alen, size_t blen);

// Returns one higher than the index of the last non-zero byte in a given byte array.
// (Returns 0 if all bytes are zero.)
size_t highestNonZeroByte(uint8_t* x, size_t len);

// Returns true if all bytes within the specified range are equal to 0. Returns false
// otherwise. Assertion: high >= low.
bool rangeIsEmtpy(uint8_t* low, uint8_t* high);

// Shifts a byte array right by a given number of bits. Assertion: 8 * bit <= length.
// void shrBytes(uint8_t* x, size_t bytes);

// Subtracts a and b and returns a pointer to the result result. The width of a and b must be
// specified. The size of the result will the larger of the two sizes or smaller.
// A reference to a variable to store the size of the variable must be passed.
uint8_t* subBytes(uint8_t* a, uint8_t* b, size_t alen, size_t blen, size_t& rlen);

} // END mathsolver namespace

#endif
