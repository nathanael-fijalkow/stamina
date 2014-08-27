#ifndef ACME_TYPES_H
#define ACME_TYPES_H

#include <string>
#include <map>
#include<vector>
#include <functional>
#include <unordered_set>

using namespace std;

typedef unsigned int uint;

#define USE_SPARSE_VECTOR 1

#define _HASH_SEED      (size_t)0xdeadbeef

size_t hash_value(const size_t & _Keyval)
{
        return ((size_t)_Keyval ^ _HASH_SEED);
}


/**************************************************************************************/
/* Types used by Acme++
	we avoid using vectors for storing fixed size amount of data in elementary structures
	since vector<> usually reserve more space than needed
	Instead we make use of C-style arrays
 */
/**************************************************************************************/

/**************************************************************************************
****************** Sharp -expressions *************************************************
**************************************************************************************/
/* Expressions are accessible by their hashes, this is needed to speed up
the test of reducibility of a product of expressions */


typedef size_t HashExpr;

class SharpedExpr;
class ConcatExpr;
class LetterExpr;
class ExtendedExpression;

/* Dynamic casts to test the type of an extended expression */
const SharpedExpr * isSharpedExpr(const ExtendedExpression * expr);
const ConcatExpr * isConcatExpr(const ExtendedExpression * expr);
const LetterExpr * isLetterExpr(const ExtendedExpression * expr);

class ExtendedExpression
{
public:
	HashExpr Hash() const { return _hash; };

	virtual void print() const = 0;

	bool operator==(const ExtendedExpression & exp) const;

protected:
	/* Constructor should update the hash value */
	ExtendedExpression(){ };

	/* hash expressions in a recursive way. Lazy. */
	HashExpr _hash;

};



/* An expression starting with a hash */
class SharpedExpr : public ExtendedExpression
{
public:
	//constructor
	SharpedExpr(ExtendedExpression * son);

	//equality operator
	bool operator==(const SharpedExpr & exp) const
	{
			return exp.son == this->son;
	}

	//the hash of the sub-expression
	const ExtendedExpression * son;

	void print() const;

};

class LetterExpr : public ExtendedExpression
{
public:
	//constructor
	LetterExpr(char letter);

	//equality operator
	bool operator==(const LetterExpr & exp) const { return exp.letter == letter; };

	//a chars used to represent the letter
	const char letter;

	virtual void print() const;

};

/* An expression made of a concatenation of expressions */
class ConcatExpr : public ExtendedExpression
{
public:
	//constructor
	ConcatExpr(const LetterExpr * expr, uint maxSonsNb); //todo letter a remplacer par extendedExpr

	ConcatExpr(const ConcatExpr & other);

	~ConcatExpr();

	//equality operator
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


	//number of sons
	uint sonsNb;

	//hashes of the subtrees. Array of size sonsNb
	//We don't use vector<> in order to save space
	//The rightmost son is stored at the beginning of the array,
	//the leftmost at address sons + sonsNb - 1
	const ExtendedExpression ** sons;

	virtual void print() const;

	/* adds a son at the left
	* Assumes that enough memory
	* was allocated when creating the sons pointer
	*/
	void addLeftSon(const ExtendedExpression *);

protected:
	void update_hash()
	{
		/* to compute hash, we use the formula used in the Boost library*/
		_hash = 0x777;
		for (const ExtendedExpression ** son = this->sons; son != sons + sonsNb; son++)
			_hash ^= hash_value((*son)->Hash()) + 0x9e3779b9 + (_hash << 6) + (_hash >> 2);
	}
};








/**************************************************************************************
****************** Matrices *************************************************
**************************************************************************************/

typedef size_t HashMat;
typedef size_t HashRowCol;


class ExplicitMatrix
{
public:
	ExplicitMatrix(uint stateNb);
	~ExplicitMatrix();

	/* number of states of the matrix and its cols and rows */
	const uint stateNb;

	/* the coefficient i,j is stored at position  i * n + j
		0 means 0, 1 means +, 2 means 1
	*/
	char * coefficients;
};


class Vector
{
public:

	//equality operator
#if USE_SPARSE_VECTOR
#else

	bool operator==(const Vector & vec) const
	{
		return entries == vec.entries;
	}

	Vector(vector<bool> data) : entries(data)
	{
		update_hash();
	}


#endif

	size_t Hash() const { return _hash; };

#if USE_SPARSE_VECTOR

	Vector(uint size);
	bool operator==(const Vector & vec) const;

	~Vector();

	Vector(const Vector & other);

	Vector(vector<size_t> data);

	Vector(size_t * data, size_t data_size);

	const size_t * end() const { return entries + entriesNb; };

	/* the nb of non-zero entries */
	const uint entriesNb;

	/* the vector of rank of entries with non zero value*/
	size_t * entries;


#else
	vector<bool> entries;
#endif

	void print() const;

protected:
	static hash<vector<bool> > hash_val;
	void update_hash();
	size_t _hash;

private:
	/* non assignable */
	Vector & operator= (const Vector & other);

};

/* Defines default hash for the class of Vector */
namespace std
{
	template <> struct hash< Vector >
	{
		size_t operator()(const Vector & expr) const
		{
			return expr.Hash();
		}
	};
}

class Matrix
{
public:
	/* number of states of the matrix and its cols and rows */
	const uint stateNb;

	Matrix(const ExplicitMatrix &);

	/* product constructor */
	Matrix(const Matrix &, const Matrix &);

	void print() const;
	void print_col() const;

	//equality operator
	bool operator==(const Matrix & mat) const;

    bool recurrent(int) const;

	/* product and stabilization of matrices.
		Updates the matrices, rows and columns HashTable */
	static Matrix prod(const Matrix &, const Matrix &);

	static Matrix stab(const Matrix &);

	bool isIdempotent() const;

	HashMat hash() const {	return _hash; };

	/* known vectors */
	static unordered_set<Vector> vectors;

	static const Vector * zero_vector;

protected:
	/* hash expressions in a recursive way. Lazy. */
	HashMat _hash;

	void update_hash();

	Matrix(uint stateNb);

	//two arrays of size stateNb containing all rows, state per state
	const Vector ** row_pluses;
	const Vector ** row_ones;
	const Vector ** col_pluses;
	const Vector ** col_ones;

	//allocate memory for pluses and ones
	void allocate();


	static const Vector * sub_prod(
		const Vector * ,
		const Vector **,
		size_t stateNb
		//size_t i
		);

};



/**************************************************************************************
****************** Element *************************************************
**************************************************************************************/


/**************************************************************************************
****************** Markov Monoid *************************************************
**************************************************************************************/



/******************************************************************************
********************* Computation **********************************************
**************************************************************************************/

/* Defines default hash for the class of extendedExpression */
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


/* Defines default hash for the matrix class */
namespace std
{
	template <> struct hash<Matrix>
	{
		size_t operator()(const Matrix & expr) const
		{
			return expr.hash();
		}
	};
}

class MarkovMonoid
{
public:
	/* all elements in the monoid and the associated matrices */
	map<const ExtendedExpression *, const Matrix *> expr_to_mat;
	map<const Matrix *, const ExtendedExpression *> mat_to_expr;

	/* the rewrite rules */
	map<const ExtendedExpression *, const ExtendedExpression *> rewriteRules;

	void print();

protected:
	MarkovMonoid(){};
};


class UnstableMarkovMonoid : public MarkovMonoid
{
public:

	/* creates zero vector */
	UnstableMarkovMonoid(uint dim);

	/* Cleans up knowns vectors */
	~UnstableMarkovMonoid();

		/* adds a new letter to the monoid, used for initialization*/
	void addLetter(char a, ExplicitMatrix & mat);

	/* adds a pair of expression and matrix in the monoid*/
	void addElement(const ExtendedExpression *, const Matrix * mat);

	/* adds a rewrite rule */
	void addRewriteRule(const ExtendedExpression *, const ExtendedExpression *);

	/* known expressions */
	unordered_set<SharpedExpr> sharpExpressions;
	unordered_set<ConcatExpr> concatExpressions;
	unordered_set<LetterExpr> letterExpressions;

	/* known matrices */
	unordered_set <Matrix> matrices;

	/* elements added at last closure step */
	vector<const ExtendedExpression *> new_elements;

	bool CloseByProduct(uint max_elements_nb = 0);

	bool CloseByStabilization();//TODO

protected:

	void process_expression(const ExtendedExpression *, vector<const ExtendedExpression *> & added_elts);

	uint dim;

};


/******************************************************************************
********************* Test **********************************************
**************************************************************************************/
/* Creates a monoid with only letters */
UnstableMarkovMonoid createLeakTest();


#endif
