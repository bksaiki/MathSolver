#ifndef _MATHSOLVER_FLOAT_H_
#define _MATHSOLVER_FLOAT_H_

#include <mpfr.h>
#include "../common/base.h"

#define MATHSOLVER_FLOAT_DEFAULT_PREC      128
#define MATHSOLVER_FLOAT_DISPLAY_PREC      64
#define MATHSOLVER_SCI_NOTATION_POS_LIM    10
#define MATHSOLVER_SCI_NOTATION_NEG_LIM    -5

namespace MathSolver
{

// MPFR wrapper type
class Float
{
public:

    Float();                            // Default constructor
    Float(const std::string& data);     // Constructor from std::string
    Float(const Float& other);          // Copy constructor
    Float(Float&& other);               // Move contructor
    ~Float();                           // Destructor

    Float& operator=(const Float& other);   // Copy assignment
    Float& operator=(Float&& other);        // Move assignment

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

    // Returns a pointer to the underlying MPFR struct
    inline mpfr_ptr data() { return mData; }

    // Converts this Float to a std::string
    std::string toString() const;

private:
    mpfr_t mData;
};

}

#endif