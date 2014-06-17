#include <string.h>
#include <iostream>
#include <typeinfo>

#include "Expressions.hpp"

// Constructors
LetterExpr::LetterExpr(char letter) : letter(letter)
{
	_hash = hash_value(letter);
};

SharpedExpr::SharpedExpr(const ExtendedExpression * son) : son(son)
{
	_hash = hash_value(son->Hash());
}

// What are those two constructors for????
ConcatExpr::ConcatExpr(const LetterExpr * expr, uint maxSonsNb)
{
	sons = (const ExtendedExpression **)malloc(maxSonsNb * sizeof(void *));

	sons[0] = expr;
	sonsNb = 1;

	update_hash();
};

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

ConcatExpr::ConcatExpr(const ConcatExpr & other) : sonsNb(other.sonsNb)
{
	_hash = other._hash;
	sons = (const ExtendedExpression **) malloc( sonsNb * sizeof(void *));
	memcpy(sons,other.sons,sonsNb * sizeof(void *));
}

void ConcatExpr::addLeftSon(const ExtendedExpression * new_son)
{
	*(sons + sonsNb) = new_son;
	sonsNb++;
	_hash ^= hash_value(new_son->Hash()) + 0x9e3779b9 + (_hash << 6) + (_hash >> 2);
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

// Dynamic casts
const SharpedExpr * isSharpedExpr(const ExtendedExpression * expr) { return dynamic_cast<const SharpedExpr *>(expr); }
const ConcatExpr * isConcatExpr(const ExtendedExpression * expr) { return dynamic_cast<const ConcatExpr *>(expr); }
const LetterExpr * isLetterExpr(const ExtendedExpression * expr){ return dynamic_cast<const LetterExpr *>(expr); }


// Printing functions
using namespace std;
void SharpedExpr::print() const
{
	if (isLetterExpr(son))
	{
		son->print();
		cout << "#";
	}
	else
	{
		cout << "(";
		son->print();
		cout << ")^#";
	}
}


void ConcatExpr::print() const
{
	for (uint i = sonsNb ; i > 0; i--)
		sons[i -1]->print();
}

void LetterExpr::print() const
{
	cout << letter;
}

// Free the memory
ConcatExpr::~ConcatExpr()
{
	free(sons);
	sons = NULL;
}
