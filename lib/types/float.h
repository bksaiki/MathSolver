#ifndef _MATHSOLVER_FLOAT_H_
#define _MATHSOLVER_FLOAT_H_

#include <mpfr.h>
#include "../common/base.h"

#define MATHSOLVER_FLOAT_DEFAULT_PREC      128
#define MATHSOLVER_FLOAT_DISPLAY_PREC      64
#define MATHSOLVER_FLOAT_DISPLAY_DIGITS    20
#define MATHSOLVER_SCI_NOTATION_POS_LIM    10
#define MATHSOLVER_SCI_NOTATION_NEG_LIM    -5

namespace MathSolver
{

// MPFR wrapper type
class Float
{
public:

    Float();                            // Default constructor
    Float(const char* data);            // Constructor from C string
    Float(const std::string& data);     // Constructor from std::string
    Float(const Float& other);          // Copy constructor
    Float(Float&& other);               // Move contructor
    ~Float();                           // Destructor

    Float& operator=(const Float& other);       // Copy assignment
    Float& operator=(Float&& other);            // Move assignment
    Float& operator=(const char* data);  // Assignment from c string
    Float& operator=(const std::string& data);  // Assignment from std::string

    // Establishing what is "equal" is difficult. Please use comparators with caution
    inline bool operator==(const Float& other) const { return mpfr_cmp(mData, other.mData) == 0; } // Equality
    inline bool operator!=(const Float& other) const { return mpfr_cmp(mData, other.mData) == 0; } // Inequality
    inline bool operator>=(const Float& other) const { return mpfr_cmp(mData, other.mData) >= 0; } // Greater than or equal
    inline bool operator<=(const Float& other) const { return mpfr_cmp(mData, other.mData) <= 0; } // Less than or equal
    inline bool operator>(const Float& other) const { return mpfr_cmp(mData, other.mData) > 0; } // Greater than
    inline bool operator<(const Float& other) const { return mpfr_cmp(mData, other.mData) < 0; } // Less than

    Float operator+(const Float& other) const;   // Addition
    Float operator-(const Float& other) const;   // Subtraction
    Float operator*(const Float& other) const;   // Multiplication
    Float operator/(const Float& other) const;   // Division

    Float& operator+=(const Float& other);   // Addition assignment
    Float& operator-=(const Float& other);   // Subtraction assignment
    Float& operator*=(const Float& other);   // Multiplication assignment
    Float& operator/=(const Float& other);   // Division assignment

    Float operator-() const; // Negation operator
    Float operator+() const;  // Unary plus operator

    // Returns a pointer to the underlying MPFR struct
    inline mpfr_ptr data() { return (mpfr_ptr)mData; }
    inline const mpfr_ptr data() const { return (mpfr_ptr)mData; }

    // Returns true if the float stored is exact.
    inline bool exact() const { return mRoundDir; }

    // Returns true if the float stored is zero.
    inline bool isZero() const { return mpfr_zero_p(mData) != 0; }

    inline bool isNaN() const { return mpfr_nan_p(mData) != 0; }

    // Returns true if the float is negative.
    inline bool sign() const { return mpfr_signbit(mData); }

    // Converts this Float to a std::string. This conversion may result in more than one value mapping to same string.
    std::string toString() const;

    // Converts this Float to a std::string so data is lost
    std::string toExactString() const;

private:

    // Sets this float from a C string. Assumes the mpfr pointer has already been initialized.
    void fromString(const char* str); 

private:
    mpfr_t mData;
    int    mRoundDir;
};

}

#endif