#ifndef _MATHSOLVER_PARSER_H_
#define _MATHSOLVER_PARSER_H_

#include <list>
#include <string>
#include "expression.h"

namespace MathSolver
{

// Parses a mathematic expression and returns a vector of tokens
// in order.
std::list<ExpressionNode*> tokenizeStr(const std::string& expr);

// Corrects a vector of tokens by expanding implied operations.
void expandTokens(std::list<ExpressionNode*>& tokens);

// Builds an expression tree from a vector of tokens.
ExpressionNode* parseTokens(const std::list<ExpressionNode*>& tokens);

}

#endif
