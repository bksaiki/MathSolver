#ifndef _MATHSOLVER_PARSER_H_
#define _MATHSOLVER_PARSER_H_

#include <list>
#include <string>
#include "expression.h"

namespace MathSolver
{

// Parses a mathematic expression and returns a list of tokens
// in order.
std::list<std::string> tokenizeStr(const std::string& expr);

// Expands a list of tokens, checks for parenthesis match, and reorders
// into prefix notation. Returns true on success.
bool reorderTokens(const std::list<std::string>& tokens);

}

#endif
