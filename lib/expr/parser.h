#ifndef _MATHSOLVER_PARSER_H_
#define _MATHSOLVER_PARSER_H_

#include <list>
#include <string>
#include "../common/base.h"
#include "../expr/expr.h"

namespace MathSolver
{

// Corrects a vector of tokens by expanding implied operations.
void expandTokens(std::list<ExprNode*>& tokens);

// Converts an expression string to an expression tree. Handles errors. Use this function unless you want to
// inspect the tokens.
ExprNode* parseString(const std::string& expr);

// Recursively parses and builds an expression tree.
ExprNode* parseTokensR(std::list<ExprNode*>::iterator begin, std::list<ExprNode*>::iterator end);

// Builds an expression tree from a vector of tokens. The list will be consumed.
ExprNode* parseTokens(std::list<ExprNode*>& tokens);

// Parses a mathematic expression and returns a vector of tokens in order.
std::list<ExprNode*> tokenizeStr(const std::string& expr);

// Returns a list of tokens as a string.
std::string toString(const std::list<ExprNode*>& list);

}

#endif
