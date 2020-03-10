#ifndef _MATHSOLVER_PARSER_H_
#define _MATHSOLVER_PARSER_H_

#include <list>
#include <string>
#include "expression.h"

namespace MathSolver
{

// Parses a mathematic expression and returns a list of tokens
// in order.
std::list<ExpressionNode*> tokenizeStr(const std::string& expr);

ExpressionNode* buildExpression(const std::list<ExpressionNode*>& tokens);

}

#endif
