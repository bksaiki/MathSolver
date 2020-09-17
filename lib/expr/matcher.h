#ifndef _MATHSOLVER_MATCHER_H_
#define _MATHSOLVER_MATCHER_H_

#include <map>
#include <vector>
#include <string>
#include "expr.h"

namespace MathSolver
{

/* Match helper objects */

// Stores the syntax tree of a match expression
class MatchExpr
{
    struct node_t
    {
        enum Type { SINGLE, VARIABLE, ELLIPSE, REL_ELLIPSE };

        std::string name;
        std::vector<node_t> children;
        ExprNode* expr;
        Type type;
        int depth;
    };

public:

    inline MatchExpr() {}
    inline MatchExpr(const std::string& match) { set(match); }

    // Sets this object based on a match string.
    void set(const std::string& match);

    // Returns this object as a string.
    inline std::string toString() const { return toString(mTop); }

private:

    // Helper functions. Given a list of match expression tokens, returns the corresponding
    // syntax tree. Called recursively.
    node_t buildMatchTree(const std::vector<std::string>& tokens);

    // Returns the subtree as a string.
    std::string toString(const node_t& node) const;

private:

    node_t mTop;
};

/* Match helper functions */

// Tokenizes a given match expression and returns the tokens in a vector. Not safe, does not
// check if the match expressions is valid.
std::vector<std::string> tokenizeMatchString(const std::string& match);

// Returns true if the expression matches the match expression and false otherwise. Not safe, does not
// check if the match expressions is valid.
bool matchExpr(const std::string& match, ExprNode* expr);

// Returns true if the match expression is valid.
bool isMatchString(const std::string& match);

// Returns an updated expression by applying the given transformation on an expression. Not safe, does not
// check if the match expressions are valid or if the expression matches the transformation.
ExprNode* applyMatchTransform(const std::string& input, const std::string& output, ExprNode* expr);

/* Matchers */

// Stores and applies transformations on expressions using expression matching logic
// Unlike the TransformMatcher, this object will return the first matching expression it encounters.
// Useful for matching unique transformations.
class UniqueExprTransformer
{
public:

    UniqueExprTransformer();   // default constructor

    // Adds a transformation. Both parameters must be match expressions.
    void add(const std::string& input, const std::string& output);

    // Clears the map of transforms and resets the success flag.
    void clear();

    // Attempts to apply one of the stored transformations. 
    ExprNode* transform(ExprNode* expr);

    // Returns whether or not the last transformation attempt was successful.
    // Calling this function after initialization returns false.
    inline bool success() const { return mSuccess; }

private:

    std::map<std::string, std::string> mTransforms;
    bool mSuccess;
};

// Stores a single match expression for matching and a map to access matched subexpressions
class UniqueExprMatcher
{
public:

    // Returns the subexpression mapped by the key. Call this only if match returns true.
    ExprNode* get(const std::string& key) const;

    // Attempts to match a given expression tree against a match expression.
    // Returns true on success, returns false otherwise.
    bool match(ExprNode* expr, const std::string& match);

private:

    std::map<std::string, ExprNode*> mSubexprs;
};

}

#endif