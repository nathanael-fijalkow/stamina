#include <string.h>
#include <iostream>
#include <typeinfo>

#include "Expressions.hpp"

// Constructors
LetterExpr::LetterExpr(char letter) : letter(letter)
{
	_hash = hash_value(letter);
};

// First constructor: takes an expression and a number of sons (to allocate the array of sons)
// and constructs a ConcatExpr with only one son
ConcatExpr::ConcatExpr(const ExtendedExpression * expr, uint maxSonsNb)
{
	sons = (const ExtendedExpression **)malloc(maxSonsNb * sizeof(void *));

	sons[0] = expr;
	sonsNb = 1;

	update_hash();
};

ConcatExpr::ConcatExpr(ExtendedExpression * expr, uint maxSonsNb)
{
	sons = (const ExtendedExpression **)malloc(maxSonsNb * sizeof(void *));

	sons[0] = expr;
	sonsNb = 1;

	update_hash();
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

	// First case: elt_left and elt_right are concatenations of expressions
	if ((ConcatExprLeft != NULL) && (ConcatExprRight != NULL))
	{
		sonsNb = ConcatExprRight->sonsNb + ConcatExprLeft->sonsNb;
		sons = (const ExtendedExpression **)malloc(sonsNb * sizeof(void *));

		for (uint i = 0; i < ConcatExprRight->sonsNb; i++)
		{
			sons[i] = ConcatExprRight->sons[i]; 
		}
		for (uint i = 0; i < ConcatExprLeft->sonsNb; i++)
		{
			sons[i + ConcatExprRight->sonsNb] = ConcatExprLeft->sons[i]; 
		}
	}

	// Second case: elt_right is a concatenation of expressions
	else if (ConcatExprRight != NULL)
	{
		sonsNb = ConcatExprRight->sonsNb + 1;
		sons = (const ExtendedExpression **)malloc(sonsNb * sizeof(void *));

		for (uint i = 0; i < ConcatExprRight->sonsNb; i++)
		{
			sons[i] = ConcatExprRight->sons[i]; 
		}
		sons[ConcatExprRight->sonsNb] = expr_left; 
	}

	// Third case: elt_left is a concatenation expression
	else if (ConcatExprLeft != NULL)
	{
		sonsNb = 1 + ConcatExprLeft->sonsNb;
		sons = (const ExtendedExpression **)malloc(sonsNb * sizeof(void *));

		sons[0] = expr_right; 
		for (uint i = 0; i < ConcatExprLeft->sonsNb; i++)
		{
			sons[i + 1] = ConcatExprLeft->sons[i]; 
		}
	}
	
	// Last case: both letters or sharped expressions
	else
	{
		sonsNb = 2;
		sons = (const ExtendedExpression **)malloc(sonsNb * sizeof(void *));

		sons[0] = expr_right;
		sons[1] = expr_left;
	}

	update_hash();
}

ConcatExpr::ConcatExpr(uint k, const ConcatExpr * expr)
{
	sonsNb = k;
	sons = (const ExtendedExpression **)malloc(sonsNb * sizeof(void *));
	for (uint i = 0; i < k; i++)
	{
		sons[i] = expr->sons[i + expr->sonsNb - k]; 
	}

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
void ConcatExpr::addLeftSon(const ExtendedExpression * new_son)
{
	*(sons + sonsNb) = new_son;
	sonsNb++;
	//_hash ^= hash_value(new_son->Hash()) + 0x9e3779b9 + (_hash << 6) + (_hash >> 2);
	update_hash();
}

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

// Dynamic casts
const SharpedExpr * isSharpedExpr(const ExtendedExpression * expr) { return dynamic_cast<const SharpedExpr *>(expr); }
const ConcatExpr * isConcatExpr(const ExtendedExpression * expr) { return dynamic_cast<const ConcatExpr *>(expr); }
const LetterExpr * isLetterExpr(const ExtendedExpression * expr){ return dynamic_cast<const LetterExpr *>(expr); }


// Printing functions
using namespace std;

void LetterExpr::print() const
{
	cout << letter;
}

void ConcatExpr::print() const
{
	for (uint i = sonsNb ; i > 0; i--)
		sons[i -1]->print();
}

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

// Free the memory
ConcatExpr::~ConcatExpr()
{
	free(sons);
	sons = NULL;
}
