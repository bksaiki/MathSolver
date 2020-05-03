#include <list>
#include <string>
#include "../types/Integer.h"
#include "expr.h"

namespace MathSolver
{

const size_t FLATTENABLE_OP_COUNT = 4;
const std::string FLATTENABLE_OPS[FLATTENABLE_OP_COUNT] = { "+", "-", "*", "**" };

ExprNode* copyOf(ExprNode* expr)
{
	ExprNode* cp;
	if (expr->type() == ExprNode::SYNTAX) 			cp = new SyntaxNode(((SyntaxNode*)expr)->name(), expr->parent());
	else if (expr->type() == ExprNode::OPERATOR) 	cp = new OpNode(((OpNode*)expr)->name(), expr->parent());
	else if (expr->type() == ExprNode::FUNCTION) 	cp = new FuncNode(((FuncNode*)expr)->name(), expr->parent());
	else if (expr->type() == ExprNode::VARIABLE) 	cp = new VarNode(((VarNode*)expr)->name(), expr->parent());
	else if (expr->type() == ExprNode::CONSTANT) 	cp = new ConstNode(((ConstNode*)expr)->name(), expr->parent());
	else if (expr->type() == ExprNode::INTEGER) 	cp = new IntNode(((IntNode*)expr)->value(), expr->parent());
	else /* expr->type() == ExprNode::FLOAT) */		cp = new FloatNode(((FloatNode*)expr)->value(), expr->parent());

	for (ExprNode* child : expr->children())
		cp->children().push_back(copyOf(child));
	return cp;
}

bool equivExpression(ExprNode* a, ExprNode* b)
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
				if (!equivExpression(*a_it, *b_it))
					return false;
				++a_it;
				++b_it;
			}

			return true;
		}
	}

	return false;
}

void flattenExpr(ExprNode* expr)
{
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
				for (auto child = op->children().begin(); child != op->children().end(); ++child)
				{
					if ((*child)->isOperator() && ((OpNode*)*child)->name() == FLATTENABLE_OPS[i])
					{
						op->children().insert(child, (*child)->children().begin(), (*child)->children().end());
						delete *child;
						child = op->children().erase(child--);
					}	
				}
			}
		}
	}
}

void freeExpression(ExprNode* expr)
{
	for (ExprNode* child : expr->children())
		freeExpression(child);
	delete expr;	
}

bool isNumerical(ExprNode* expr)
{
	if (expr->type() == ExprNode::FUNCTION || expr->isOperator())
	{
		for (ExprNode* child : expr->children())
		{
			if (!isNumerical(child))
				return false;
		}

		return true;
	}

	return expr->isNumber();
}

std::string toInfixString(ExprNode* expr)
{
	if (expr->type() == ExprNode::FUNCTION)
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
		else if (op->name() == "-*" && op->children().size() == 1)
		{
			return "-" + toInfixString(op->children().front());
		}
		else
		{
			std::string sub = toInfixString(op->children().front());
			std::string printOp = (op->name() == "**") ? "" : ((op->name() == "-*") ? "-" : op->name());
			for (auto it = std::next(op->children().begin()); it != op->children().end(); ++it)
				sub += (printOp + toInfixString(*it));
			if (op->parent() != nullptr && op->parent()->prec() < op->prec())
				return "(" + sub + ")";
			else
				return sub;
		}
		
	}
	else
	{
		return expr->toString();
	}
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