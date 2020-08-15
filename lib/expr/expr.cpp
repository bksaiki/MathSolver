#include <list>
#include <string>
#include "../types/integer.h"
#include "expr.h"

namespace MathSolver
{

const size_t FLATTENABLE_OP_COUNT = 11;
const std::string FLATTENABLE_OPS[FLATTENABLE_OP_COUNT] = 
{ 
	"+", "-", "*", "**",
	">", "<", ">=", "<=", "!="
	"and", "or"
};

ExprNode* copyOf(ExprNode* expr)
{
	ExprNode* cp;
	if (expr->type() == ExprNode::SYNTAX) 			cp = new SyntaxNode(((SyntaxNode*)expr)->name(), expr->parent());
	else if (expr->type() == ExprNode::OPERATOR) 	cp = new OpNode(((OpNode*)expr)->name(), expr->parent());
	else if (expr->type() == ExprNode::FUNCTION) 	cp = new FuncNode(((FuncNode*)expr)->name(), expr->parent());
	else if (expr->type() == ExprNode::VARIABLE) 	cp = new VarNode(((VarNode*)expr)->name(), expr->parent());
	else if (expr->type() == ExprNode::CONSTANT) 	cp = new ConstNode(((ConstNode*)expr)->name(), expr->parent());
	else if (expr->type() == ExprNode::INTEGER) 	cp = new IntNode(((IntNode*)expr)->value(), expr->parent());
	else if (expr->type() == ExprNode::FLOAT)		cp = new FloatNode(((FloatNode*)expr)->value(), expr->parent());
	else if (expr->type() == ExprNode::RANGE)		cp = new RangeNode(((RangeNode*)expr)->value(), expr->parent());
	else if (expr->type() == ExprNode::BOOLEAN)		cp = new BoolNode(((BoolNode*)expr)->value(), expr->parent());
	else 		gErrorManager.log("Should not have executed here", ErrorManager::FATAL, __FILE__, __LINE__);

	for (ExprNode* child : expr->children())
		cp->children().push_back(copyOf(child));
	return cp;
}

bool eqvExpr(ExprNode* a, ExprNode* b)
{
	if (a->type() == b->type())
	{
		if (a->type() == ExprNode::INTEGER) 	return ((IntNode*)a)->value() == ((IntNode*)b)->value();
		if (a->type() == ExprNode::FLOAT) 		return ((FloatNode*)a)->value() == ((FloatNode*)b)->value();
		if (a->type() == ExprNode::CONSTANT) 	return ((ConstNode*)a)->name() == ((ConstNode*)b)->name();
		if (a->type() == ExprNode::VARIABLE) 	return ((VarNode*)a)->name() == ((VarNode*)b)->name();

		if (((a->isOperator() && ((OpNode*)a)->name() == ((OpNode*)b)->name()) ||
			 (a->type() == ExprNode::FUNCTION && ((FuncNode*)a)->name() == ((FuncNode*)b)->name())) && 
		   (a->children().size() == b->children().size()))
		{
			auto a_it = a->children().begin();
			auto b_it = b->children().begin();
			while (a_it != a->children().end())
			{
				if (!eqvExpr(*a_it, *b_it))
					return false;
				++a_it;
				++b_it;
			}

			return true;
		}
	}

	return false;
}

std::list<std::string> extractVariables(ExprNode* expr)
{
	std::list<std::string> ret;
	for (auto e : expr->children())
	{
		std::list<std::string> sub = extractVariables(e);
		ret.insert(ret.end(), sub.begin(), sub.end());
	}

	if (expr->type() == ExprNode::VARIABLE)
		ret.push_back(expr->toString());
	return ret;
}

void flattenExpr(ExprNode* expr)
{
	if (expr == nullptr)	// no expression
		return;

	bool hasGrandChildren = false;
	for (ExprNode* child : expr->children()) // recursively flatten
	{
		if (child->children().size() != 0)
		{
			flattenExpr(child);
			hasGrandChildren = true;
		}
	}

	if (hasGrandChildren && expr->isOperator())
	{
		OpNode* op = (OpNode*)expr;
		for (size_t i = 0; i < FLATTENABLE_OP_COUNT; ++i)
		{
			if (op->name() == FLATTENABLE_OPS[i])
			{	
				auto child = op->children().begin();
				while (child != op->children().end())
				{
					if ((*child)->isOperator() && ((OpNode*)*child)->name() == FLATTENABLE_OPS[i])
					{
						for (auto e : (*child)->children())
						{
							op->children().insert(child, e);
							e->setParent(op);
						}

						delete *child;
						child = op->children().erase(child);
					}
					else
					{
						++child;
					}
						
				}
			}
		}
	}
}

void freeExpression(ExprNode* expr)
{
	if (expr != nullptr)
	{
		for (ExprNode* child : expr->children())
			freeExpression(child);
		delete expr;	
	}
}

std::string toInfixString(ExprNode* expr)
{
	if (expr == nullptr)
	{
		return "<null>";
	}
	else if (expr->type() == ExprNode::FUNCTION)
	{
		FuncNode* func = (FuncNode*)expr;
		std::string sub = func->name() + "(";
		if (!expr->children().empty())
		{
			sub += toInfixString(expr->children().front());
			for (auto it = std::next(expr->children().begin()); it != expr->children().end(); ++it)
				sub += (", " + toInfixString(*it));
		}

		return sub + ")";
	}
	else if (expr->isOperator())
	{
		OpNode* op = (OpNode*)expr;
		if (op->name() == "!")
		{
			if (op->children().front()->isOperator() && ((OpNode*)op->children().front())->name() == "!")
				return "(" + toInfixString(op->children().front()) + ")!";
			else
				return toInfixString(op->children().front()) + "!";
		}
		else if (op->name() == "^")
		{
			return toInfixString(op->children().front()) + "^" + toInfixString(op->children().back());		
		}
		else if (op->name() == "mod")
		{
			return toInfixString(op->children().front()) + " " + op->name() + " " + toInfixString(op->children().back());
		}
		else if (op->name() == "-*" && op->children().size() == 1)
		{
			return "-" + toInfixString(op->children().front());
		}
		else if (op->name() == "or" || op->name() == "and")
		{
			std::string sub = toInfixString(op->children().front());
			for (auto it = std::next(op->children().begin()); it != op->children().end(); ++it)
				sub += (" " + op->name() + " " + toInfixString(*it));
			if (op->parent() != nullptr && !op->parent()->isSyntax() && op->parent()->prec() < op->prec())
				return "(" + sub + ")";
			else
				return sub;
		}
		else
		{
			std::string sub = toInfixString(op->children().front());
			std::string printOp = (op->name() == "**") ? "" : ((op->name() == "-*") ? "-" : op->name());
			for (auto it = std::next(op->children().begin()); it != op->children().end(); ++it)
				sub += (printOp + toInfixString(*it));
			if (op->parent() != nullptr && !op->parent()->isSyntax() && op->parent()->prec() < op->prec())
				return "(" + sub + ")";
			else
				return sub;
		}
		
	}
	else if (expr->isSyntax())
	{
		SyntaxNode* syntax = (SyntaxNode*)expr;
		if (syntax->name() == "|")
		{
			return "{ " + toInfixString(expr->children().front()) + " " + syntax->name() + " " + toInfixString(expr->children().back()) + " }";
		}
	}

	return expr->toString();
}

std::string toPrefixString(ExprNode* expr)
{
	if (expr == nullptr)
	{
		return "<null>";
	}
	else if (!expr->children().empty())
	{
		std::string sub = "(" + expr->toString();
		for (auto e : expr->children())
			sub += (" " + toPrefixString(e));
		return sub + ")";
	}
	else
	{
		return expr->toString();
	}
}

size_t nodeCount(ExprNode* expr)
{
	size_t c = 1;
	if (expr->isOperator() || expr->type() == ExprNode::FUNCTION)
	{
		for (ExprNode* child : expr->children())
			c += nodeCount(child);	
	}

	return c;
}

}