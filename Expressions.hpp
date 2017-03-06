
/* INCLUDES */
#include <string>
#include <iostream>

#ifndef EXPRESSIONS_HPP
#define EXPRESSIONS_HPP

typedef unsigned int uint;

// Defines a hash function
#define _HASH_SEED      (size_t)0xdeadbeef

#ifndef MSVC
namespace std
{
size_t inline hash_value(const size_t & _Keyval)
	{
        return ((size_t)_Keyval ^ _HASH_SEED);
	}
}
#else
#include <functional>
#include <xhash>
#endif

/* CLASS DEFINITIONS */

// The expressions are accessible by their hashes
typedef size_t HashExpr;

class ExtendedExpression;
class LetterExpr;
class ConcatExpr;
class SharpedExpr;

// Dynamic casts to test the type of an extended expression
const LetterExpr * isLetterExpr(const ExtendedExpression * expr);
const ConcatExpr * isConcatExpr(const ExtendedExpression * expr);
const SharpedExpr * isSharpedExpr(const ExtendedExpression * expr);

// The class of all expressions
class ExtendedExpression
{
public:

	// Function returning the hash
	HashExpr Hash() const { return _hash ; };

	// Equality operator
	virtual bool operator == (const ExtendedExpression & exp) const;
        bool operator != (const ExtendedExpression & exp) const
        {
           return (! (*this == exp));
        }

	//print to an output stream
	virtual void print(std::ostream & os = std::cout) const = 0;
	
	// Sharp-height (constant not function)
	char sharp_height;// () const = 0;

    virtual ~ExtendedExpression(){};
    
protected:

	// The hash. It is used in the == operator to quickly detect if two expressions are different.
	HashExpr _hash;

	// The constructor is protected to avoid direct instantiation
	ExtendedExpression(){};
};

/* for printing to a file */
std::ostream& operator<<(std::ostream& os, const ExtendedExpression & expr);

// The class of expressions consisting only of one letter, extending the general class
class LetterExpr : public ExtendedExpression
{
public:

	// A char is used to represent the letter
	const char letter;

	// Letter constructor
	LetterExpr(char letter);

	// Equality operator
	bool operator == (const LetterExpr & exp) const { return exp.letter == letter; };

	// Print
	virtual void print(std::ostream& os = std::cout) const;
	
	// Sharp-height
	//virtual char sharp_height=0;// () const;

};

// The class of expressions consisting of concatenation of expressions, extending the general class
class ConcatExpr : public ExtendedExpression
{
public:

	// The number of sons
	uint sonsNb;

	// The subtrees
	// It is a C-style array of size sonsNb
	// The rightmost son is stored at the beginning of the array,
	// the leftmost at address sons + sonsNb - 1
	
	//denis : reversing this convention to put the "normal" order instead.
	const ExtendedExpression ** sons;

protected:
	ConcatExpr() : sonsNb(0), sons(NULL) {};

public:
	// Second constructor: a copy constructor which performs a memcopy of the field sons
	ConcatExpr(const ConcatExpr & other);

	// Third constructor: concatenation of two expressions
	ConcatExpr(const ExtendedExpression * expr_left, const ExtendedExpression * expr_right);
	
	//Constructor where just specify the number of sons to do the allocation
	ConcatExpr(int n){
		sonsNb=n;
		sons = (const ExtendedExpression **)malloc(sonsNb * sizeof(void *));
		}

	// This is an assignment operator which performs a memcopy of the field sons
	ConcatExpr & operator=(const ConcatExpr &);

	// Adds a son to the left
	// Assumes that enough memory was allocated when creating the son's pointer
	//void addLeftSon(const ExtendedExpression *);

	// Free the memory for a useless expression
	~ConcatExpr();

	// Equality operator
	bool operator==(const ConcatExpr & exp) const
	{
	  if ((exp.sonsNb != this->sonsNb) || (exp.Hash() != this->Hash() ) )
	    return false;
	  else {
	      for (uint i = 0; i < sonsNb; i++) {
			  if (*exp.sons[i] != *sons[i])
				  return false;

			  /*
			  const ExtendedExpression * t1 = exp.sons[i];
		  const ExtendedExpression * t2 = sons[i];
		  
		  const ConcatExpr * c1 = isConcatExpr(exp.sons[i]);
		  const ConcatExpr * c2 = isConcatExpr(sons[i]);

		  if (c1 && (c1->sonsNb == 1))
		    t1 = c1->sons[0];
		  if (c2 && (c2->sonsNb == 1))
		    t2 = c2->sons[0];
			
		  if (t1 != t2)
		    return false;
			*/
		}
	    }
	  return true;
	};

	// Print
	virtual void print(std::ostream& os = std::cout) const;
	
	// Sharp-height
	//virtual char sharp_height;// () const;

	// Function that computes the hash, using the formula used in the Boost library
	void update_hash()
	{
		/* Hugo: we do not want that, a concat expresssion with only one son is different from its son 
	  if(sonsNb == 1) 
	    _hash = sons[0]->Hash();
	  else {
	  */
	    _hash = 0x777;
	    for (const ExtendedExpression ** son = this->sons; son != sons + sonsNb; son++)
			_hash ^= std::hash_value((*son)->Hash()) + 0x9e3779b9 + (_hash << 6) + (_hash >> 2);
	  /* } */
	}
};

// The class of expressions starting with a sharp, extending the general class
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
	virtual void print(std::ostream & os) const;
	
	// Sharp-height
	//virtual char sharp_height;// () const;
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
