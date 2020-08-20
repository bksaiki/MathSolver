#ifndef _MATHSOLVER_MATCHER_H_
#define _MATHSOLVER_MATCHER_H_

#include <map>
#include <list>
#include <string>
#include "expr.h"

namespace MathSolver
{

std::list<std::string> tokenizeMatchString(const std::string& match);

bool matchExpr(const std::string& match, ExprNode* expr);

bool isMatchString(const std::string& match);

ExprNode* applyMatchTransform(const std::string& input, const std::string& output, ExprNode* expr);

/* Matchers */

// Stores and applies transformations on expressions using expression matching logic
// Unlike the TransformMatcher, this object will return the first matching expression it encounters.
// Useful for matching unique transformations.
class UniqueTransformMatcher
{
public:

    // Default constructors
    // Default destructor

    // Adds a transformation. Both parameters must be match expressions.
    void add(const std::string& input, const std::string& output);

    // Attempts to apply one of the stored transformations. Returns the updated expression if 
    // successful or the unaltered expression if not.
    ExprNode* transform(ExprNode* expr);

private:

    std::map<std::string, std::string> mTransforms;
    
};

}

#endif