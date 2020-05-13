#ifndef _MATHSOLVER_POLYNOMIAL_H_
#define _MATHSOLVER_POLYNOMIAL_H_

#include "../common/base.h"
#include "../expr/expr.h"

namespace MathSolver
{

// Returns true if the expression is a monomial basis (i.e x^n, such that x contains a variable,
// and n is a non-negative integer, or x where x contains a variable).
bool isMonomialBasis(ExprNode* expr);

// Returns true if the expression is equivalent to a monomial.  Used for unsimplified expressions that
// can be simplified into a monomial
bool isMonomialEqv(ExprNode* expr);

// Returns true if the expression is equivalent to a polynomial. Used for unsimplified expressions that
// can be simplified into a polynomial.
bool isPolynomialEqv(ExprNode* expr);

// Returns true if the expression is a monomial in standard form, i.e (constant * term).
bool isMonomial(ExprNode* expr);

// Returns true if the expression is a polynomial in standard form, i.e ((constant * term) ...).
bool isPolynomial(ExprNode* expr);

// Returns the order of the monomial.
int monomialOrder(ExprNode* expr);

// Returns the polynomial at expr as a polynomial in standard form. By default, higher order terms appear first.
// Settings down to false reverses this order.
ExprNode* reorderPolynomial(ExprNode* expr, bool down = true);

}



#endif