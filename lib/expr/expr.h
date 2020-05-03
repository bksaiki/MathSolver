#ifndef _MATHSOLVER_EXPRESSION_H_
#define _MATHSOLVER_EXPRESSION_H_

#include "../expr/node.h"


namespace MathSolver
{

//
//  Expression operations
//

// Returns a copy of the given expression tree.
ExprNode* copyOf(ExprNode* expr);

// Returns true if the value of two nodes is the same.
bool equivExpression(ExprNode* a, ExprNode* b);

// Takes an expression node and recursively simplifes certain operators with interior nodes of the
// same operator into a single operator with many operands. e.g. (+ (+ a (+ b c)) d) ==> (+ a b c d)
void flattenExpr(ExprNode* expr);

// Deletes an expression tree.
void freeExpression(ExprNode* expr);

// Returns true if the expression only contains numerical operands (Non-symbolic expression).
bool isNumerical(ExprNode* expr);

// Returns true if the node is a number.
bool isNumber(ExprNode* node);

// Returns true if the node is a number, constant, or variable
bool isValue(ExprNode* node);

// Returns the number of nodes in the expression tree.
size_t nodeCount(ExprNode* expr);

// Returns an expression tree as a string in infix notation. Assumes the tree is valid.
std::string toInfixString(ExprNode* expr);

// Returns an expression tree as a string in prefix notation. Assumes the tree is valid.
std::string toPrefixString(ExprNode* expr);

}

#endif
