#ifndef _MATHSOLVER_EXPRESSION_H_
#define _MATHSOLVER_EXPRESSION_H_

#include <list>
#include <string>
#include "../types/Integer.h"

namespace MathSolver
{

//
//  Expression operations
//

// Stores unique identifiers
struct ExprNode
{   
    enum Type
    {
        VARIABLE = 0,
        OPERATOR,
        FUNCTION,
        CONSTANT,
        INTEGER,
        FLOAT
    };

    ExprNode* parent;
    std::list<ExprNode*> children;
    Type type;
    int prec;

    std::string str;
    Integer exact;
    double inexact;
};

// Returns a new expression node
ExprNode* initExprNode();

// Clears this struct ignoring the parent and sets the type
void clear(ExprNode* node, ExprNode::Type newType);

// Moves the data from src to dest, ignoring the parent and updating the children respectively.
void moveExprNode(ExprNode* dest, ExprNode* src);

// Returns a copy of the given expression tree
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

//
// Language constructs
/*
    Operator precedence
    0   numbers, constants  --
    1   functions           RL
    2   -*                  RL
    3   ^                   RL
    4   !                   LR
    5   **                  LR
    6   * / %               LR
    7   +-                  LR
    8   > < >= < == !=      --

    N   , | () [] {}
*/

// Returns true if the string is a predefined or user-defined function.
bool isFunction(const std::string& func);

// Returns true if the string is an operator
bool isOperator(const std::string& op);

// Returns true if the character is an operator.
bool isOperatorChar(char c);

// Returns true if the character is a bracket: (), {}, [].
bool isBracket(char c);

// Returns the precedence of the operator. Returns 0 if the string is not
// an operator.
int operatorPrec(const std::string& op);

}

#endif
