#ifndef _MATHSOLVER_EXPRESSION_H_
#define _MATHSOLVER_EXPRESSION_H_

#include "../common/base.h"
#include "../expr/node.h"

namespace MathSolver
{

//
//  Expression operations
//

// Returns true if every node in the expression satisfies the given predicate.
template <typename Pred>
bool containsAll(ExprNode* expr, Pred pred)
{
    for (auto e : expr->children())
        if (!containsAll(e, pred)) return false;
    return pred(expr);
}

// Returns true if the expression contains at least one node satisfying the given predicate.
template <typename Pred>
bool containsOnce(ExprNode* expr, Pred pred)
{
    for (auto e : expr->children())
        if (containsOnce(e, pred)) return true;
    return pred(expr);
}

// Returns true if the expression contains at least one instance of a certain type
inline bool containsType(ExprNode* expr, ExprNode::Type type) { return containsOnce(expr, [&](ExprNode* node) { return node->type() == type; }); }

// Returns a copy of the given expression tree.
ExprNode* copyOf(ExprNode* expr);

// Returns true if the value of two nodes is the same.
bool eqvExpr(ExprNode* a, ExprNode* b);

// Takes an expression node and recursively simplifes certain operators with interior nodes of the
// same operator into a single operator with many operands. e.g. (+ (+ a (+ b c)) d) ==> (+ a b c d)
void flattenExpr(ExprNode* expr);

// Deletes an expression tree.
void freeExpression(ExprNode* expr);

// Returns true if the expression only contains numerical operands (Non-symbolic expression).
inline bool isNumerical(ExprNode* expr)
{ 
    return containsAll(expr, [](ExprNode* node) { return node->isNumber() || node->isOperator() || node->type() == ExprNode::FUNCTION; }); 
}

// Returns the number of nodes in the expression tree.
size_t nodeCount(ExprNode* expr);

// Returns an expression tree as a string in infix notation. Assumes the tree is valid.
std::string toInfixString(ExprNode* expr);

// Returns an expression tree as a string in prefix notation. Assumes the tree is valid.
std::string toPrefixString(ExprNode* expr);

}

#endif
