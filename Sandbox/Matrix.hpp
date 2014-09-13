/* INCLUDES */
#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <unordered_set>
#include <vector>
#include <set>

#include "Expressions.hpp"

typedef size_t HashMat;
typedef size_t HashRowCol;

using namespace std;

/* 
if 0 vectors are stored as an array of integers containing indices of non zero entries
if 1 they are stored as bits of an array of uint*/
#define USE_SPARSE_MATRIX 0

/* if 1 recurrent states and idempotent matrices are stored in a static array*/
#define CACHE_RECURRENT_STATES 1


/* CLASS DEFINITIONS */
// Class of explicit matrices, represented as arrays
class ExplicitMatrix
{
public:
	// Number of states of the matrix
	const uint stateNb;

	// coefficients is a C-style array
	// the coefficient i,j is stored at position  i * n + j
	// 0 means 0, 1 means +, 2 means 1
	char * coefficients;

	// Constructor 
	ExplicitMatrix(uint stateNb);

	//Random matrix
	static ExplicitMatrix  * random(uint stateNb);

	// Free a useless explicit matrix
	~ExplicitMatrix();
};

// Class of vectors
class Vector
{
public:
#if USE_SPARSE_MATRIX
	// Number of non-zero entries
	const uint entriesNb;
	// Entries is a C-style array of length entriesNb containing all the non-zero values in increasing order
	size_t * entries;

	bool contains(size_t n) const { for (int i = 0; i < entriesNb; i++) if (entries[i] == n) return true; return false; };

	// end() points right after the last entry
	const size_t * end() const { return entries + entriesNb; };

#else
	// Number of entries
	const uint entriesNb;

	// Entries is an array containing integers whose first entriesNb bits encode the entries
	// thus the array size is the smallest integer larger than entriesNb / (sizeof(uint) * 8) 
	uint * bits;

	bool contains(size_t n) const { return bits[n / (sizeof(uint) * 8)] & ( 1 << (n % (sizeof(uint) * 8)) ) ; };

	// size of th ebits array
	const uint bitsNb;

	Vector() : bits(NULL), bitsNb(0), entriesNb(0) {};

#endif

	// First constructor
	Vector(uint size);

	// Second constructor
	Vector(const Vector & other);

#if USE_SPARSE_MATRIX
	// Third constructor
	Vector(vector <size_t> data);

	// Fourth constructor, data is copied by default
	Vector(size_t * data, size_t data_size, bool copy = true);
#else
	// Third constructor
	Vector(vector <bool> data);

	Vector(uint * data, size_t data_size, bool copy = true);
#endif

	// Function returning the hash
	size_t Hash() const { return _hash; };

	// Equality operator
	bool operator == (const Vector & vec) const;

	// Free a useless vector
	~Vector();

	// Print
	void print() const;


protected:

	// Hash
	size_t _hash;

	static hash <vector <bool> > hash_val;

#if USE_SPARSE_MATRIX
	// Function computing the hash
	void update_hash(){
	_hash = 0;
	for (size_t * index = entries; index != entries + entriesNb; index++)
		_hash ^= hash_value(*index) + 0x9e3779b9 + (_hash << 6) + (_hash >> 2);
	} ;
#else
	// Function computing the hash
	void update_hash(){
		_hash = 0;
		for (size_t * index = bits; index != bits + entriesNb / (sizeof(uint) * 8); index++)
			_hash ^= hash_value(*index) + 0x9e3779b9 + (_hash << 6) + (_hash >> 2);
	};
#endif

private:

	// Equality operator
	Vector & operator = (const Vector & other);

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
	// Number of states of the matrix
	const uint stateNb;

	// Constructor
	Matrix(const ExplicitMatrix &);

	// Constructor
	Matrix(uint stateNb);

	// Print
	void print() const;

	// Equality operator
	bool operator == (const Matrix & mat) const;

	// Function computing the product and stabilization
	// They update the matrices, rows and columns
	static Matrix prod(const Matrix &, const Matrix &);

#if USE_SPARSE_MATRIX
	// compute stabilisation
	static Matrix stab(const Matrix &);
#else
	// compute stabilisation, given recurrent states
	static Matrix stab(const Matrix & mat, const Vector * recs);
#endif

	// Function returning the hash
	HashMat hash() const { return _hash; };

	// Function checking whether a matrix is idempotent
	bool isIdempotent() const;

	// Two STATIC elements
	// This is the set of known vectors
	static unordered_set <Vector> vectors;

	// This is the constant vector with only zero entries
	static const Vector * zero_vector;

	// computes the list of recurrent states.
	const Vector * recurrent_states() const;

	/* computes the list of recurrence classes given the list of recurrent states.
	The matrix is assumed to be idempotent. */
	const Vector * recurrence_classes(const Vector *) const;

	// Count the number of leaks given the list of recurrence classes
	//the matrix is assumed to be idempotent
#if USE_SPARSE_MATRIX
	uint countLeaks(const Vector * classes) const { return 0; };
#else
	uint countLeaks(const Vector * classes) const;
#endif

protected:

	// The hash expression
	HashMat _hash;

	// Four C-style matrices of size stateNb containing all rows, state per state
	const Vector ** row_pluses;
	const Vector ** row_ones;
	const Vector ** col_pluses;
	const Vector ** col_ones;

	// Function computing the hash
	void update_hash(){
	_hash = 0;
	for (const Vector ** p = col_ones; p != col_ones + stateNb; p++)
		_hash ^= hash_value((size_t) *p) + 0x9e3779b9 + (_hash << 6) + (_hash >> 2);
	for (const Vector ** p = col_pluses; p != col_pluses + stateNb; p++)
		_hash ^= hash_value((size_t) *p) + 0x9e3779b9 + (_hash << 6) + (_hash >> 2);
	for (const Vector ** p = row_ones; p != row_ones + stateNb; p++)
		_hash ^= hash_value((size_t) *p) + 0x9e3779b9 + (_hash << 6) + (_hash >> 2);
	for (const Vector ** p = row_pluses; p != row_pluses + stateNb; p++)
		_hash ^= hash_value((size_t) *p) + 0x9e3779b9 + (_hash << 6) + (_hash >> 2);
	};

	// Function allocating memory for pluses and ones
	void allocate();

	// Function used in the product
	static const Vector * sub_prod(const Vector *, const Vector **, size_t stateNb);

	// Create a new vector, keep only coordinates of v that are true in tab
#if USE_SPARSE_MATRIX
	static const Vector * purge(const Vector *varg, bool * tab);
#else
	static const Vector * purge(const Vector *varg, const Vector * tab);
#endif


	// Function checking whether a state is recurrent
	//the matrix is assumed idempotent
    bool recurrent(int) const;

};

// Defines default hash for the matrix class
namespace std
{
	template <> struct hash<Matrix>
	{
		size_t operator()(const Matrix & mat) const
		{
			return mat.hash();
		}
	};
}

#endif
