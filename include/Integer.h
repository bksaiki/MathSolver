#ifndef _MATHSOLVER_INTEGER_H_
#define _MATHSOLVER_INTEGER_H_

#include "Bytes.h"

#define MATHSOLVER_DEFAULT_INT_WIDTH  4

namespace MathSolver
{

class Integer
{
public: 

    // Constructs an Integer with a default length.
    Integer();

    // Construts an Integer from a byte-array of a given length
    Integer(uint8_t* arr, size_t len, bool sign = false);

    // Constructs an Integer from a 32-bit unsigned integer.
    Integer(unsigned int x);

    // Constructs an Integer from a 32-bit unsigned integer.
    Integer(signed int x);

    // Copy constructor from another Integer.
    Integer(const Integer& other);

    // Move constructor from another Integer.
    Integer(Integer&& other);

    // Frees byte array.
    ~Integer();

    // Copy assignment
    Integer& operator=(const Integer& other);

    // Move assignment
    Integer& operator=(Integer&& other);

    // Addition operator
    Integer operator+(const Integer& other) const;

    // Subtraction operator
    Integer operator-(const Integer& other) const;

    // Addition assignment operator
    Integer& operator+=(const Integer& other);

    // Subtraction assignment operator
    Integer& operator-=(const Integer& other);

    // Unary minus operator
    Integer operator-() const;

    // Returns a pointer to the byte array.
    inline uint8_t* data() const { return mData; }

    // Sets the data of this Integer using a byte array of a specified length.
    // The default sign is false (positive).
    void set(uint8_t* arr, size_t len, bool sign = false);

    // Returns true if this integer is unsigned.
    inline bool sign() const { return mSign; }

    // Returns the width of this Integer.
    inline size_t size() const { return mSize; } 

private:

    // Helper function. Adds this Integer and another and returns the result. The sign
    // of the result must be specified.
    Integer add(const Integer& other, bool sign) const;

    // Helper function. Adds another Integer to this one. The sign of the result must
    // be specified.
    void addAssign(const Integer& other, bool sign);

    // Helper function. Resizes the underlying byte array. Assertion: size > 0. Does not check
    // if data will be lost.
    void resizeNoCheck(size_t size);

    // Helper function. Adds this Integer and another and returns the result. The sign
    // of the result must be specified.
    Integer sub(const Integer& other, bool sign) const;

    // Helper function. Subtracts this Integer by another. The sign of the result must
    // be specified.
    void subAssign(const Integer& other, bool sign);

private:
    
    uint8_t*    mData;
    size_t      mSize;
    bool        mSign;
};

} // END mathsolver namespace

#endif