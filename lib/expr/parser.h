#ifndef _MATHSOLVER_PARSER_H_
#define _MATHSOLVER_PARSER_H_

#include <list>
#include <string>
#include "../expr/expr.h"

namespace MathSolver
{

// Parses a mathematic expression and returns a vector of tokens
// in order.
std::list<ExprNode*> tokenizeStr(const std::string& expr);

// Corrects a vector of tokens by expanding implied operations.
void expandTokens(std::list<ExprNode*>& tokens);

// Builds an expression tree from a vector of tokens.
ExprNode* parseTokens(const std::list<ExprNode*>& tokens);

}

#endif
