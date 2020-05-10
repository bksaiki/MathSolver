#ifndef _MATHSOLVER_NODE_H_
#define _MATHSOLVER_NODE_H_

#include <list>
#include <string>
#include "../common/base.h"
#include "../types/integer.h"

namespace MathSolver
{

// Most basic expression unit. Abstract base class
class ExprNode
{   
public:

    enum Type
    {
        VARIABLE,
        OPERATOR,
        FUNCTION,
        CONSTANT,
        INTEGER,
        FLOAT,
        SYNTAX
    };

public:

    // Destructor    
    virtual ~ExprNode() = 0;

    // Returns a list of pointers to the.children() of this node.
    inline std::list<ExprNode*>& children() { return mChildren; }
    inline const std::list<ExprNode*>& children() const { return mChildren; }

    // Returns true if this node is a number (i.e. Integer of float type).
    virtual bool isNumber() const = 0;

    // Returns true if this node is an operator (i.e. not a value).
    virtual bool isOperator() const = 0;

    // Returns true if this node is syntax (i.e. bracket, comma, etc.).
    virtual bool isSyntax() const = 0;

    // Returns true if this node is a value (i.e. not an operator).
    virtual bool isValue() const = 0;

    // Returns a pointer to the parent node.
    inline ExprNode* parent() const { return mParent; }

    // Returns the precedence of this node.
    inline int prec() const { return mPrec; }

    // Sets the parent of this node.
    inline void setParent(ExprNode* node) { mParent = node; }

    // Returns a string representation of this node.
    virtual std::string toString() const = 0;

    // Returns the type of this node.
    virtual Type type() const = 0;

protected:
    ExprNode* mParent;
    std::list<ExprNode*> mChildren;
    int mPrec;
};

//
// Derived types
//

// Syntax node (i.e. brackets, commas, etc.)
class SyntaxNode : public ExprNode
{
public:

    // Default constructor 
    SyntaxNode(const std::string& data = "", ExprNode* parent = nullptr);

    // Destructor
    ~SyntaxNode() {}

    // Returns the data stored at this node.
    inline std::string& name() { return mData; }
    inline const std::string& name() const { return mData; }

    // Returns true if this node is a number (i.e. Integer of float type).
    inline bool isNumber() const { return false; }

    // Returns true if this node is an operator (i.e. not a value).
    inline bool isOperator() const { return false; }

    // Returns true if this node is syntax (i.e. bracket, comma, etc.).
    inline bool isSyntax() const { return true; }

    // Returns true if this node is a value (i.e. not an operator).
    inline bool isValue() const { return false; }

    // Sets the data at this node.
    inline void setName(const std::string& str);

    // Returns a string representation of this node.
    inline std::string toString() const { return mData; }
 
    // Returns the type of this node.
    inline Type type() const { return SYNTAX; }

private:
    std::string mData;
};

// Operator node
class OpNode : public ExprNode
{
public:

    // Default constructor 
    OpNode(const std::string& data = "", ExprNode* parent = nullptr);

    // Destructor
    ~OpNode() {}

    // Returns the data stored at this node.
    inline std::string& name() { return mData; }
    inline const std::string& name() const { return mData; }

    // Returns true if this node is a number (i.e. Integer of float type).
    inline bool isNumber() const { return false; }

    // Returns true if this node is an operator (i.e. not a value).
    inline bool isOperator() const { return true; }

    // Returns true if this node is syntax (i.e. bracket, comma, etc.).
    inline bool isSyntax() const { return false; }

    // Returns true if this node is a value (i.e. not an operator).
    inline bool isValue() const { return false; }

    // Sets the data at this node.
    void setName(const std::string& str);

    // Returns a string representation of this node.
    inline std::string toString() const { return mData; }
 
    // Returns the type of this node.
    inline Type type() const { return OPERATOR; }

private:
    std::string mData;
};

// Function node
class FuncNode : public ExprNode
{
public:

    // Default constructor 
    FuncNode(const std::string& data = "", ExprNode* parent = nullptr);

    // Destructor
    ~FuncNode() {}

    // Returns the data stored at this node.
    inline std::string& name() { return mData; }
    inline const std::string& name() const { return mData; }

    // Returns true if this node is a number (i.e. Integer of float type).
    inline bool isNumber() const { return false; }

    // Returns true if this node is an operator (i.e. not a value).
    inline bool isOperator() const { return false; }

    // Returns true if this node is syntax (i.e. bracket, comma, etc.).
    inline bool isSyntax() const { return false; }

    // Returns true if this node is a value (i.e. not an operator).
    inline bool isValue() const { return false; }

    // Sets the data at this node.
    inline void setName(const std::string& str) { mData = str; }

    // Returns a string representation of this node.
    inline std::string toString() const { return mData; }
 
    // Returns the type of this node.
    inline Type type() const { return FUNCTION; }

private:
    std::string mData;
};

// Variable node
class VarNode : public ExprNode
{
public:

    // Default constructor 
    VarNode(const std::string& data = "", ExprNode* parent = nullptr);

    // Destructor
    ~VarNode() {}

    // Returns the data stored at this node.
    inline std::string& name() { return mData; }
    inline const std::string& name() const { return mData; }

    // Returns true if this node is a number (i.e. Integer of float type).
    inline bool isNumber() const { return false; }

    // Returns true if this node is an operator (i.e. not a value).
    inline bool isOperator() const { return false; }

    // Returns true if this node is syntax (i.e. bracket, comma, etc.).
    inline bool isSyntax() const { return false; }

    // Returns true if this node is a value (i.e. not an operator).
    inline bool isValue() const { return true; }

    // Sets the data at this node.
    inline void setName(const std::string& str) { mData = str; }

    // Returns a string representation of this node.
    inline std::string toString() const { return mData; }
 
    // Returns the type of this node.
    inline Type type() const { return VARIABLE; }

private:
    std::string mData;
};

// Const node
class ConstNode : public ExprNode
{
public:

    // Default constructor 
    ConstNode(const std::string& data = "", ExprNode* parent = nullptr);

    // Destructor
    ~ConstNode() {}

    // Returns the data stored at this node.
    inline std::string& name() { return mData; }
    inline const std::string& name() const { return mData; }

    // Returns true if this node is a number (i.e. Integer of float type).
    inline bool isNumber() const { return false; }

    // Returns true if this node is an operator (i.e. not a value).
    inline bool isOperator() const { return false; }

    // Returns true if this node is syntax (i.e. bracket, comma, etc.).
    inline bool isSyntax() const { return false; }

    // Returns true if this node is a value (i.e. not an operator).
    inline bool isValue() const { return true; }

    // Sets the data at this node.
    inline void setName(const std::string& str) { mData = str; }

    // Returns a string representation of this node.
    inline std::string toString() const { return mData; }
 
    // Returns the type of this node.
    inline Type type() const { return CONSTANT; }

private:
    std::string mData;
};

// Integer node
class IntNode : public ExprNode
{
public:

    // Default constructor 
    IntNode(const Integer& data = Integer(), ExprNode* parent = nullptr);

    // Destructor
    ~IntNode() {}

    // Returns the data stored at this node.
    inline Integer& value() { return mData; }
    inline const Integer& value() const { return mData; }

    // Returns true if this node is a number (i.e. Integer of float type).
    inline bool isNumber() const { return true; }

    // Returns true if this node is an operator (i.e. not a value).
    inline bool isOperator() const { return false; }

    // Returns true if this node is syntax (i.e. bracket, comma, etc.).
    inline bool isSyntax() const { return false; }

    // Returns true if this node is a value (i.e. not an operator).
    inline bool isValue() const { return true; }

    // Sets the data at this node.
    inline void setValue(const Integer& i) { mData = i; }

    // Returns a string representation of this node.
    inline std::string toString() const { return mData.toString(); }
 
    // Returns the type of this node.
    inline Type type() const { return INTEGER; }

private:
    Integer mData;
};

// Float node
class FloatNode : public ExprNode
{
public:

    // Default constructor 
    FloatNode(double data = 0.0, ExprNode* parent = nullptr);

    // Destructor
    ~FloatNode() {}

    // Returns the data stored at this node.
    inline double value() const { return mData; }

    // Returns true if this node is a number (i.e. Integer of float type).
    inline bool isNumber() const { return true; }

    // Returns true if this node is an operator (i.e. not a value).
    inline bool isOperator() const { return false; }

    // Returns true if this node is syntax (i.e. bracket, comma, etc.).
    inline bool isSyntax() const { return false; }

    // Returns true if this node is a value (i.e. not an operator).
    inline bool isValue() const { return true; }

    // Sets the data at this node.
    inline void setValue(double d) { mData = d; }

    // Returns a string representation of this node.
    inline std::string toString() const { return std::to_string(mData); }
 
    // Returns the type of this node.
    inline Type type() const { return FLOAT; }

private:
    double mData;
};

//
// Parsing
//

// Returns true if the character is an operator.
bool isOperator(char c);

// Returns true if the character is a bracket: (), {}, [].
bool isBracket(char c);

// Returns true if the string is a predefined or user-defined function.
bool isFunction(const std::string& func);

// Returns true if the string is an operator
bool isOperator(const std::string& op);

/*
    Operator precedence
    0   numbers, constants  --
    1   functions           RL
    2   -*                  RL
    3   ^                   RL
    4   !                   LR
    5   **                  LR
    6   * / %               LR
    7   +-                  LR
    8   > < >= < == !=      --

    N   , | () [] {}
*/

// Returns the precedence of the string
int opPrec(const std::string& str);

//
// Node manipulation
//

// Sets the parent of src to the parent of dest, deletes the original node at dest, and
// returns the new node at dest. Effectively overwrites dest with src. The node at dest is
// unusable after this operation
ExprNode* moveNode(ExprNode* dest, ExprNode* src);

// Replaces dest from the parent's list of children with src. Assumes dest is an invalid
// pointer that needs updating.
void replaceChild(ExprNode* parent, ExprNode* src, std::list<ExprNode*>::iterator pos);

}

#endif