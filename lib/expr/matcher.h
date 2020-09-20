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

    typedef std::map<std::string, std::pair<ExprNode*, bool>>           dict_t;
    typedef std::map<std::pair<std::string, std::string>, ExprNode*>    patt_t;
    typedef std::map<std::string, std::vector<ExprNode*>>               subp_t;

    inline MatchDict() {}               // default constructor
    inline ~MatchDict() { clear(); }    // destructor

    // Adds a key-value pair to the primary dictionary.
    void add(const std::string& id, ExprNode* expr, bool pattern = false);

    // Clears all entries in this dictionary. Deletes extra nodes in this dictionary.
    void clear();

    // Returns a reference to the ellipse dictionary.
    inline patt_t& ellDict() { return mEll; }
    inline const patt_t& ellDict() const { return mEll; }

    // Returns a pointer the expression associated with id. Returns nullptr on failure.
    ExprNode* get(const std::string& id) const;

    // Clears all entries in this dictionary. Does not delete extra nodes!!
    void release();

    // Returns a reference to the subpattern dictionary.
    inline subp_t& subp() { return mSubPattern; }
    inline const subp_t& subp() const { return mSubPattern; }

    // Updates a key.
    void update(const std::string& id, ExprNode* expr, bool pattern = false);

private:

    // Helper. Copies extra nodes from the other dictionary to this one.
    void copyPrimary(const MatchDict& other);

private:

    dict_t mDict;       // primary dictionary
    patt_t mEll;    // ellipse dictionary (temporary storage during matching)
    subp_t mSubPattern;
};

// Stores the syntax tree of a match expression
class MatchExpr
{
public:

    struct node
    {
        enum Type { SINGLE, VARIABLE, ELLIPSE, REL_ELLIPSE, UNORD_ELLIPSE };

        std::string name;
        std::vector<node> children;
        ExprNode* expr;
        Type type;
        int depth;
    };

public:

    inline MatchExpr() = default;
    inline MatchExpr(const std::string& match, bool permissive = false) { set(match, permissive); }

    // Creates an expression based on this match expression and dictionary.
    ExprNode* create(const MatchDict& dict, Transform post) const;

    // Returns true if the expression matches. A match dictionary can be optionally
    // passed to store match data.
    bool match(ExprNode* expr, MatchDict& dict) const;
    bool match(ExprNode* expr) const;

    // Sets this object based on a match string.
    void set(const std::string& match, bool permissive = false);

    // Returns this object as a string.
    inline std::string toString() const { return toString(mTop); }

private:

    // Given a list of match expression tokens, returns the corresponding
    // syntax tree. Called recursively.
    node buildMatchTree(const std::vector<std::string>& tokens, bool permissive) const;

    // Creates a single leaf based on a syntax node.
    ExprNode* createLeaf(const node& match, const MatchDict& dict, bool shallow = false) const;

    // Creates a subexpression based on a syntax tree.
    ExprNode* createSubexpr(const node& match, const MatchDict& dict, Transform post, bool top = false) const;

    // Returns true if the expression matches a single token. Stores match
    // data in a dictionary.
    bool matchLeaf(const node& match, ExprNode* expr, MatchDict& dict) const;

    // Returns true if the expression matches the given syntax tree. Stores
    // match data in a dictionary
    bool matchSubexpr(const node& match, ExprNode* expr, MatchDict& dict) const;

    // Attempts to match a range of subexpression children. Returns true on success.
    bool matchRelativeEllipse(const node& match, ExprNode* expr, MatchDict& dict, size_t start) const;

    // Attempts to match a subexpression with an unordered ellipse.
    bool matchUnorderedEllipse(const node& match, ExprNode* expr, MatchDict& dict, size_t start) const;

    // Tokenizes a given match expression and returns the tokens in a vector. Not safe, does not
    // check if the match expressions is valid.
    std::vector<std::string> tokenizeMatchString(const std::string& match);

    // Returns the subtree as a string.
    std::string toString(const node& tree) const;

    // Returns a vector of all the variables in a subexpression
    std::vector<std::string> varsInSubexpr(const node& match) const;

private:

    node mTop;
};

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
    ExprNode* transform(ExprNode* expr, Transform post = [](ExprNode* expr) { return expr; });

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

    // Clears the dictionary of extraneous nodes. Call this after a successful match when
    // the subexpression components are no longer of use and before the underlying
    // expression is altered.
    inline void clear() { mSubexprs.clear(); }

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