#ifndef _MATHSOLVER_NODE_H_
#define _MATHSOLVER_NODE_H_

#include <list>
#include <string>
#include "../common/base.h"
#include "../types/float.h"
#include "../types/integer.h"
#include "../types/range.h"

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
        RANGE,
        SYNTAX,
        BOOLEAN
    };

public:

    // Destructor    
    virtual ~ExprNode() = 0;

    // Returns a list of pointers to the.children() of this node.
    inline std::list<ExprNode*>& children() { return mChildren; }
    inline const std::list<ExprNode*>& children() const { return mChildren; }

    // Returns true if this node is a number (i.e. Integer or Float type).
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

    SyntaxNode(const std::string& data = "", ExprNode* parent = nullptr);
    ~SyntaxNode() {}

    inline std::string& name() { return mData; }
    inline const std::string& name() const { return mData; }

    inline bool isNumber() const { return false; }
    inline bool isOperator() const { return false; }
    inline bool isSyntax() const { return true; }
    inline bool isValue() const { return false; }

    inline void setName(const std::string& str);

    inline std::string toString() const { return mData; }
 
    inline Type type() const { return SYNTAX; }

private:
    std::string mData;
};

// Operator node
class OpNode : public ExprNode
{
public:

    OpNode(const std::string& data = "", ExprNode* parent = nullptr);
    ~OpNode() {}

    inline std::string& name() { return mData; }
    inline const std::string& name() const { return mData; }

    inline bool isNumber() const { return false; }
    inline bool isOperator() const { return true; }
    inline bool isSyntax() const { return false; }
    inline bool isValue() const { return false; }

    void setName(const std::string& str);

    inline std::string toString() const { return mData; }
 
    inline Type type() const { return OPERATOR; }

private:
    std::string mData;
};

// Function node
class FuncNode : public ExprNode
{
public:

    FuncNode(const std::string& data = "", ExprNode* parent = nullptr);
    ~FuncNode() {}

    inline std::string& name() { return mData; }
    inline const std::string& name() const { return mData; }

    inline bool isNumber() const { return false; }
    inline bool isOperator() const { return false; }
    inline bool isSyntax() const { return false; }
    inline bool isValue() const { return false; }

    inline void setName(const std::string& str) { mData = str; }

    inline std::string toString() const { return mData; }

    inline Type type() const { return FUNCTION; }

private:
    std::string mData;
};

// Variable node
class VarNode : public ExprNode
{
public:

    VarNode(const std::string& data = "", ExprNode* parent = nullptr);
    ~VarNode() {}

    inline std::string& name() { return mData; }
    inline const std::string& name() const { return mData; }

    inline bool isNumber() const { return false; }
    inline bool isOperator() const { return false; }
    inline bool isSyntax() const { return false; }
    inline bool isValue() const { return true; }

    inline void setName(const std::string& str) { mData = str; }

    inline std::string toString() const { return mData; }
 
    inline Type type() const { return VARIABLE; }

private:
    std::string mData;
};

// Const node
class ConstNode : public ExprNode
{
public:

    ConstNode(const std::string& data = "", ExprNode* parent = nullptr);
    ~ConstNode() {}

    inline std::string& name() { return mData; }
    inline const std::string& name() const { return mData; }

    inline bool isNumber() const { return false; }
    inline bool isOperator() const { return false; }
    inline bool isSyntax() const { return false; }
    inline bool isValue() const { return true; }

    inline void setName(const std::string& str) { mData = str; }

    inline std::string toString() const { return mData; }
 

    inline Type type() const { return CONSTANT; }

private:
    std::string mData;
};

// Integer node
class IntNode : public ExprNode
{
public:

    IntNode(const Integer& data = Integer(), ExprNode* parent = nullptr);
    ~IntNode() {}

    inline Integer& value() { return mData; }
    inline const Integer& value() const { return mData; }

    inline bool isNumber() const { return true; }
    inline bool isOperator() const { return false; }
    inline bool isSyntax() const { return false; }
    inline bool isValue() const { return true; }

    inline void setValue(const Integer& i) { mData = i; }

    inline std::string toString() const { return mData.toString(); }

    inline Type type() const { return INTEGER; }

private:
    Integer mData;
};

// Float node
class FloatNode : public ExprNode
{
public:

    FloatNode(const Float& data = Float("0"), ExprNode* parent = nullptr);
    FloatNode(Float&& data, ExprNode* parent = nullptr);
    ~FloatNode() {}

    inline Float& value() { return mData; }
    inline const Float& value() const { return mData; }

    inline bool isNumber() const { return true; }
    inline bool isOperator() const { return false; }
    inline bool isSyntax() const { return false; }
    inline bool isValue() const { return true; }

    inline void setValue(const Float& d) { mData = d; }

    inline std::string toString() const { return mData.toString(); }
 
    inline Type type() const { return FLOAT; }

private:
    Float mData;
};

// Range node
class RangeNode : public ExprNode
{
public:

    RangeNode(const Range& data = Range(), ExprNode* parent = nullptr);
    RangeNode(Range&& data, ExprNode* parent = nullptr);
    ~RangeNode() {}

    inline Range& value() { return mData; }
    inline const Range& value() const { return mData; }

    inline bool isNumber() const { return false; }
    inline bool isOperator() const { return false; }
    inline bool isSyntax() const { return false; }
    inline bool isValue() const { return false; }

    inline void setValue(const Range& d) { mData = d; }

    inline std::string toString() const { return mData.toString(); }
 
    inline Type type() const { return RANGE; }

private:
    Range mData;
};

// Boolean node
class BoolNode : public ExprNode
{
public:
    BoolNode(bool data, ExprNode* parent = nullptr);
    ~BoolNode() {}

    inline bool value() const { return mData; }

    inline bool isNumber() const { return false; }
    inline bool isOperator() const { return false; }
    inline bool isSyntax() const { return false; }
    inline bool isValue() const { return true; }

    inline void setValue(bool b) { mData = b; }

    inline std::string toString() const { return (mData) ? "true" : "false"; }
 
    inline Type type() const { return BOOLEAN; }

private:
    bool mData;
};

//
//  Special nodes
//

// Returns true if the expression node represents a zero value in the context
// of the type.
bool isZeroNode(ExprNode* expr);

// Returns true if the expression node represents a one, identity, or unit value
// depending on the context of the type.
bool isIdentityNode(ExprNode* expr);

// Returns the floating-point value of this node. Must be a number or an error will be generated.
Float toFloat(ExprNode* node);

//
// Parsing (TODO: move to parsing?)
//

// Returns true if the character is an operator.
bool isOperator(char c);

// Returns true if the character is syntax
bool isSyntax(char c);

// Returns true if the character is a bracket: (), {}, [].
bool isBracket(char c);

// Returns true if the string is a bracket: ),},]
bool isClosingBracket(const std::string& str);

// Returns true if the string is a bracket: (,{,[
bool isOpeningBracket(const std::string& str);

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
    6   * / %, mod          LR
    7   + -                 LR
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
std::list<ExprNode*>::iterator replaceChild(ExprNode* parent, ExprNode* src, std::list<ExprNode*>::iterator pos, bool remove = false);

// Removes this expression from the parent's list of children. Assumes another pointer is tracking this node,
// else data will be lost.
void releaseChild(ExprNode* node);


}

#endif