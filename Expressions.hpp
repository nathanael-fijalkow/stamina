#ifndef EXPRESSIONS_HPP
#define EXPRESSIONS_HPP

#include <string>

typedef unsigned int uint;

// Defines a hash function
#define _HASH_SEED      (size_t)0xdeadbeef

#ifndef MSVC
size_t hash_value(const size_t & _Keyval);
#else
#include <functional>
#include <xhash>
using namespace std;

#endif

// The expressions are accessible by their hashes
typedef size_t HashExpr;

class SharpedExpr;
class ConcatExpr;
class LetterExpr;
class ExtendedExpression;

// Dynamic casts to test the type of an extended expression
const SharpedExpr * isSharpedExpr(const ExtendedExpression * expr);
const ConcatExpr * isConcatExpr(const ExtendedExpression * expr);
const LetterExpr * isLetterExpr(const ExtendedExpression * expr);


// The class of all expressions
class ExtendedExpression
{
public:

	// Function returning the hash
	HashExpr Hash() const { return _hash ; };

	// Print
	virtual void print() const = 0;

	// Equality operator
	bool operator == (const ExtendedExpression & exp) const;

protected:
	// The hash
	HashExpr _hash;

	// The constructor is protected to avoid direct instantiation
	ExtendedExpression(){};
};

// The class of expressions starting with a hash, extending the general class
class SharpedExpr : public ExtendedExpression
{
public:
	// The hash of the sub-expression
	const ExtendedExpression * son;

	// The constructor
	SharpedExpr(const ExtendedExpression * son);

	// Equality operator
	bool operator == (const SharpedExpr & exp) const
	{
			return exp.son == this -> son;
	}

	// Print
	void print() const;
};

// The class of expressions consisting only of one letter, extending the general class
class LetterExpr : public ExtendedExpression
{
public:
	// A char is used to represent the letter
	const char letter;

	// The constructor
	LetterExpr(char letter);

	// Equality operator
	bool operator == (const LetterExpr & exp) const { return exp.letter == letter; };

	// Print
	virtual void print() const;
};

// The class of expressions consisting of concatenation of expressions, extending the general class
class ConcatExpr : public ExtendedExpression
{
public:
	// The number of sons
	uint sonsNb;

	// The hashes of the subtrees
	// It is a C-style array of size sonsNb
	// The rightmost son is stored at the beginning of the array,
	// the leftmost at address sons + sonsNb - 1
	const ExtendedExpression ** sons;

	// First constructor:
	ConcatExpr(const ExtendedExpression * expr, uint maxSonsNb);

	// This is a copy constructor which performs a memcopy of the field sons
	ConcatExpr(const ConcatExpr & other);

	// This is an assignment operator which performs a memcopy  of the field sons
	ConcatExpr & operator=(const ConcatExpr &);

	// Free the memory for a useless expression
	~ConcatExpr();

	// Equality operator
	bool operator==(const ConcatExpr & exp) const
	{
		if ((exp.sonsNb != this->sonsNb) || (exp.Hash() != this->Hash() ) )
			return false;
		else
		{
			for (uint i = 0; i < sonsNb; i++)
			{
				if (exp.sons[i] != sons[i])
					return false;
			}
		}
		return true;
	};


	// Print
	virtual void print() const;

	/* Adds a son to the left
	* Assumes that enough memory
	* was allocated when creating the son's pointer
	*/
	void addLeftSon(const ExtendedExpression *);

protected:
	// Function that computes the hash, using the formula used in the Boost library
	void update_hash()
	{
		_hash = 0x777;
		for (const ExtendedExpression ** son = this->sons; son != sons + sonsNb; son++)
			_hash ^= hash_value((*son)->Hash()) + 0x9e3779b9 + (_hash << 6) + (_hash >> 2);
	}
};

// Defines default hash for the class of extendedExpression
namespace std
{
	template <> struct hash<SharpedExpr>
	{
		size_t operator()(const SharpedExpr & expr) const
		{
			return expr.Hash();
		}
	};
}

namespace std
{
	template <> struct hash<ConcatExpr>
	{
		size_t operator()(const ConcatExpr & expr) const
		{
			return expr.Hash();
		}
	};
}

namespace std
{
	template <> struct hash<LetterExpr>
	{
		size_t operator()(const LetterExpr & expr) const
		{
			return expr.Hash();
		}
	};
}

#endif
