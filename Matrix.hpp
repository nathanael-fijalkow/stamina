#include <unordered_set>
#include <vector>

//#include "Expressions.hpp"

typedef size_t HashMat;
typedef size_t HashRowCol;

using namespace std;

// Class of explicit matrices, represented as arrays
class ExplicitMatrix
{
public:
	// Number of states of the matrix
	const uint stateNb;

	// coefficients is a C-style array
	//  the coefficient i,j is stored at position  i * n + j
	// 0 means 0, 1 means +, 2 means 1
	char * coefficients;

	// Constructor 
	ExplicitMatrix(uint stateNb);

	// Free a useless explicit matrix
	~ExplicitMatrix();
};

// Class of vectors
class Vector
{
public:
	// Entries is a C-style array containing all the non-zero values in increasing order
	size_t * entries;

	// Number of non-zero entries
	const uint entriesNb;

	// Constructor
	Vector(uint size);

	// Constructor
	Vector(const Vector & other);

	// ????
	Vector(vector <size_t> data);

	// ?????
	Vector(size_t * data, size_t data_size);

	// Function returning the hash
	size_t Hash() const { return _hash; };

	// Equality operator
	bool operator == (const Vector & vec) const;

	// Free a useless vector
	~Vector();

	//
	const size_t * end() const { return entries + entriesNb; };

	// Print
	void print() const;

protected:

	// Hash
	size_t _hash;

	static hash <vector <bool> > hash_val;

	// Function computing the hash
	void update_hash(){
	_hash = 0;
	for (size_t * index = entries; index != entries + entriesNb; index++)
		_hash ^= hash_value(*index) + 0x9e3779b9 + (_hash << 6) + (_hash >> 2);
	} ;

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
	static Matrix stab(const Matrix &);

	// Function returning the hash
	HashMat hash() const { return _hash; };

	// This is the set of known vectors
	// Note that it is static
	static unordered_set <Vector> vectors;

	// This is the constant vector with only zero entries
	static const Vector * zero_vector;

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

	// Function checking whether a state is idempotent
    bool recurrent(int) const;

	// Function checking whether a matrix is idempotent
	bool isIdempotent() const;
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
