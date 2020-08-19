#ifndef _MATHSOLVER_MATCHER_H_
#define _MATHSOLVER_MATCHER_H_

#include <list>
#include <string>
#include "expr.h"

namespace MathSolver
{

std::list<std::string> tokenizeMatchString(const std::string& match);

bool matchExpr(const std::string& match, ExprNode* expr);

bool isMatchString(const std::string& match);

ExprNode* applyMatchTransform(const std::string& input, const std::string& output, ExprNode* expr);

}

#endif