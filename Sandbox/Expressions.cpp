#include <string.h>
#include <iostream>
#include <typeinfo>

#include "Expressions.hpp"

// Constructors
LetterExpr::LetterExpr(char letter) : letter(letter)
{
	_hash = hash_value(letter);
};

// Second constructor: a copy constructor which performs a memcopy of the field sons
ConcatExpr::ConcatExpr(const ConcatExpr & other) : sonsNb(other.sonsNb)
{
	_hash = other._hash;
	sons = (const ExtendedExpression **)malloc(sonsNb * sizeof(void *));
	memcpy(sons,other.sons,sonsNb * sizeof(void *));
}

// Concatenation constructor: creates the concatenation of two expressions
ConcatExpr::ConcatExpr(const ExtendedExpression * expr_left, const ExtendedExpression * expr_right)
{
	// Casts
	const ConcatExpr * ConcatExprLeft = isConcatExpr(expr_left);
	const ConcatExpr * ConcatExprRight = isConcatExpr(expr_right);

	int subtrees_nb_right = (ConcatExprRight != NULL) ? ConcatExprRight->sonsNb : 1;
	sonsNb = subtrees_nb_right + ((ConcatExprLeft != NULL) ? ConcatExprLeft->sonsNb : 1);

	/* temporary array used to create all infixes */
	sons = (const ExtendedExpression **)malloc(sonsNb * sizeof(void *));

	/* copy the expressions in the array*/
	if (ConcatExprRight != NULL)
		memcpy(sons, ConcatExprRight->sons, ConcatExprRight->sonsNb * sizeof(void*));
	else
		sons[0] = expr_right;

	if (ConcatExprLeft != NULL)
		memcpy(sons + subtrees_nb_right, ConcatExprLeft->sons, ConcatExprLeft->sonsNb * sizeof(void*));
	else
		sons[subtrees_nb_right] = expr_left;

	update_hash();
}

// This is an assignment operator which performs a memcopy of the field sons
ConcatExpr & ConcatExpr::operator=(const ConcatExpr & other)
{
	if (this != &other)
	{
		sonsNb = other.sonsNb;
		_hash = other._hash;
		sons = (const ExtendedExpression **)malloc(sonsNb * sizeof(void *));
		memcpy(sons, other.sons, sonsNb * sizeof(void *));
	}	
	return *this;
}

// Adds a son to the left
// Assumes that enough memory was allocated when creating the son's pointer
/*
void ConcatExpr::addLeftSon(const ExtendedExpression * new_son)
{
	*(sons + sonsNb) = new_son;
	sonsNb++;
	//_hash ^= hash_value(new_son->Hash()) + 0x9e3779b9 + (_hash << 6) + (_hash >> 2);
	update_hash();
}
*/

SharpedExpr::SharpedExpr(const ExtendedExpression * son) : son(son)
{
	_hash = hash_value(son->Hash());
}




// Equality operator

bool ExtendedExpression::operator == (const ExtendedExpression & exp) const
{
	const ExtendedExpression * pexp = &exp;
	if (typeid(this) != typeid(pexp))
		return false;
	const SharpedExpr * sexpr = isSharpedExpr(this);
	if (sexpr != NULL)
		return (*sexpr == *(SharpedExpr *)pexp);
	else
	{
		const ConcatExpr * cexpr = isConcatExpr(this);
		if (cexpr != NULL)
			return (*cexpr == *(ConcatExpr *)pexp);
		else
			return *(LetterExpr *)cexpr == *(LetterExpr *)pexp;
	}
}

ostream& operator<<(ostream& os, const ExtendedExpression & expr){ expr.print(os); return os; };


// Dynamic casts
const SharpedExpr * isSharpedExpr(const ExtendedExpression * expr) { return dynamic_cast<const SharpedExpr *>(expr); }
const ConcatExpr * isConcatExpr(const ExtendedExpression * expr) { return dynamic_cast<const ConcatExpr *>(expr); }
const LetterExpr * isLetterExpr(const ExtendedExpression * expr){ return dynamic_cast<const LetterExpr *>(expr); }


// Printing functions
using namespace std;

void LetterExpr::print(ostream& os) const
{
	os << letter;
}

void ConcatExpr::print(ostream& os) const
{
	for (uint i = sonsNb ; i > 0; i--)
		sons[i -1]->print(os);
}

void SharpedExpr::print(ostream& os) const
{
	if (isLetterExpr(son))
	{
		son->print(os);
		os << "#";
	}
	else
	{
		os << "(";
		son->print(os);
		os << ")^#";
	}
}

// Free the memory
ConcatExpr::~ConcatExpr()
{
	free(sons);
	sons = NULL;
}
