#ifndef _MATHSOLVER_MATCHER_H_
#define _MATHSOLVER_MATCHER_H_

#include <map>
#include <vector>
#include <string>
#include "expr.h"

namespace MathSolver
{

/* Match helper objects */

class MatchDict
{
public:

    typedef std::map<std::string, ExprNode*>                            dict_t;
    typedef std::map<std::pair<std::string, std::string>, ExprNode*>    ell_dict_t;

    inline MatchDict() = default;
    inline ~MatchDict() { clear(); }

    // Adds a key-value pair to the primary dictionary.
    void add(const std::string& id, ExprNode* expr);

    // Deletes extra nodes in this dictionary
    void clear();

    // Returns a pointer the expression associated with id. Returns nullptr on failure.
    ExprNode* get(const std::string& id) const;

    // Returns a reference to the ellipse dictionary.
    inline ell_dict_t& ellDict() { return mEll; }
    inline const ell_dict_t& ellDict() const { return mEll; }

private:

    dict_t mDict;       // primary dictionary
    ell_dict_t mEll;    // ellipse dictionary (temporary storage during matching)
};

// Stores the syntax tree of a match expression
class MatchExpr
{
public:

    struct node
    {
        enum Type { SINGLE, VARIABLE, ELLIPSE, REL_ELLIPSE };

        std::string name;
        std::vector<node> children;
        ExprNode* expr;
        Type type;
        int depth;
    };

public:

    inline MatchExpr() = default;
    inline MatchExpr(const std::string& match) { set(match); }

    // Creates an expression based on this match expression and dictionary.
    ExprNode* create(const MatchDict& dict) const;

    // Returns true if the expression matches. A match dictionary can be optionally
    // passed to store match data.
    bool match(ExprNode* expr, MatchDict& dict) const;
    bool match(ExprNode* expr) const;

    // Sets this object based on a match string.
    void set(const std::string& match);

    // Returns this object as a string.
    inline std::string toString() const { return toString(mTop); }

private:

    // Given a list of match expression tokens, returns the corresponding
    // syntax tree. Called recursively.
    node buildMatchTree(const std::vector<std::string>& tokens) const;

    // Creates a single leaf based on a syntax node.
    ExprNode* createLeaf(const node& match, const MatchDict& dict) const;

    // Creates a subexpression based on a syntax tree.
    ExprNode* createSubexpr(const node& match, const MatchDict& dict) const;

    // Returns true if the expression matches a single token. Stores match
    // data in a dictionary.
    bool matchLeaf(const node& match, ExprNode* expr, MatchDict& dict) const;

    // Returns true if the expression matches the given syntax tree. Stores
    // match data in a dictionary
    bool matchSubexpr(const node& match, ExprNode* expr, MatchDict& dict) const;

    // Returns the subtree as a string.
    std::string toString(const node& tree) const;

private:

    node mTop;
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
    typedef std::vector<std::pair<MatchExpr, MatchExpr>> transform_t;

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

    transform_t mTransforms;
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

    MatchDict mSubexprs;
};

}

#endif