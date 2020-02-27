#ifndef _MATHSOLVER_INTEGER_H_
#define _MATHSOLVER_INTEGER_H_

#include <iostream>
#include <string>
#include "Bytes.h"

#define MATHSOLVER_DEFAULT_INT_WIDTH    4
#define MATHSOLVER_MAX_INT_WIDTH        65536

namespace Mathsolver
{

class Integer
{
public: 

    // Constructs an Integer with a default length.
    Integer();

    // Copy constructor from another Integer.
    Integer(const Integer& other);

    // Move constructor from another Integer.
    Integer(Integer&& other);

    // Construts an Integer from a byte-array of a given length
    Integer(uint8_t* arr, size_t len, bool sign = false);

    // Constructs an Integer from a 32-bit unsigned integer.
    Integer(unsigned int x);

    // Constructs an Integer from a 32-bit signed integer.
    Integer(signed int x);

    // Constructs an Integer from a C-style string.
    Integer(const char* str);

    // Constructs an Integer from a std::string.
    Integer(const std::string& str);

    // Frees byte array.
    ~Integer();

    // Copy assignment
    Integer& operator=(const Integer& other);

    // Move assignment
    Integer& operator=(Integer&& other);

    // Assignement from C-style string.
    Integer& operator=(const char* str);

    // Assignement from std::string.
    Integer& operator=(const std::string& str);

    // Equality operator
    bool operator==(const Integer& other) const;

    // Inequality operator
    bool operator!=(const Integer& other) const;

    // Greater than operator
    bool operator>(const Integer& other) const;

    // Less than operator
    bool operator<(const Integer& other) const;

    // Greater than or equal operator
    bool operator>=(const Integer& other) const;

    // Less than or equal operator
    bool operator<=(const Integer& other) const;

    // Addition operator
    Integer operator+(const Integer& other) const;

    // Subtraction operator
    Integer operator-(const Integer& other) const;

    // Multiplication operator
    Integer operator*(const Integer& other) const;

    // Division operator
    Integer operator/(const Integer& other) const;

    // Addition assignment operator
    Integer& operator+=(const Integer& other);

    // Subtraction assignment operator
    Integer& operator-=(const Integer& other);

    // Mutliplication assignment operator
    Integer& operator*=(const Integer& other);

    // Division assignment operator
    Integer& operator/=(const Integer& other);

    // Unary minus operator
    Integer operator-() const;

    // Arithmetic shift right assignment operator. Preserves the sign. Does not perform two's
    // complement. Thus, x >> bits = -(-x >> bits)
    Integer& operator>>=(size_t bits);

    // Arithmetic shift left assignment oeprator. Preserves the sign. Does not perform two's
    // complement. Thus x << bits = -(-x << bits)
    Integer& operator<<=(size_t bits);

    // Stream extraction operator.
    friend std::ostream& operator<<(std::ostream& out, const Integer& integer);

    // Stream insertion operator.
    friend std::istream& operator>>(std::istream& in, Integer& integer);

    // Returns a pointer to the byte array.
    inline uint8_t* data() const { return mData; }

    // Sets the data of this Integer using a byte array of a specified length.
    // The default sign is false (positive).
    void set(uint8_t* arr, size_t len, bool sign = false);

    // Returns true if this integer is negative.
    inline bool sign() const { return mSign; }

    // Returns the width of this Integer.
    inline size_t size() const { return mSize; } 

    // Returns this Integer as a string.
    std::string toString() const;

private:

    // Helper function. Adds this Integer and another and returns the result. The sign
    // of the result must be specified.
    Integer add(const Integer& other, bool sign) const;

    // Helper function. Adds another Integer to this one. The sign of the result must
    // be specified.
    void addAssign(const Integer& other, bool sign);

    // Helper function. Divides this Integer by another and stores the quotient at quo and
    // the remainder at rem. The size of the result and the remainder is this size.
    void divAndRem(const Integer& other, Integer& quo, Integer& rem) const;

    // Helper function. Divides this Integer by another and stores the quotient at *this and
    // the remainder at rem. The size of the remainder is this size.
    void divAssignAndRem(const Integer& other, Integer& rem);

    // Helper function. Clears the data and sets this Integer based on a C-style string. 
    void fromString(const char* str);

    // Helper function. Moves the contents of one Integer to this one. The argument is unusable
    // after this function is called. Does not free the existing byte array.
    void move(Integer& other);

    // Helper function. Multiplies this Integer by another and returns the result. The resultant
    // sign must be specified.
    Integer mul(const Integer& other, bool sign) const;

    // Helper function. Resizes the underlying byte array. Assertion: size > 0. Does not check
    // if data will be lost.
    void resizeNoCheck(size_t size);

    // Helper function. Sets this Integer with a given length and a positive zero value. Does not 
    // free the existing byte array.
    void setZero(size_t len);

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



} // END MathSolver namespace

#endif