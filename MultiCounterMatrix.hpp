#ifndef MULTICOUNTERMATRIX_HPP
#define MULTICOUNTERMATRIX_HPP

#include "Matrix.hpp"
#include "VectorInt.hpp"
#include <algorithm>


class MultiCounterMatrix : public Matrix
{

protected:

	// This matrix act_prod is of size (2N+3)*(2N+3), it is computed once and for all.
	static char ** act_prod;

// encoding for counter actions, smaller is better
// 0: R_0 = (R,R,...,R) 
// 1: R_1 = (E,R,...,R) 
// ...
// N-1: R_{N-1} = (E,...,E,R)
// N: R_N = (E,...,E)

// N+1: I_0 = (I,R,...,R)
// N+2: I_1 = (E,I,R,...,R)
// ...
// 2N: I_{N-1} = (E,...,E,I)

// 2N+1: OMEGA

// 2N+2: BOT

	/* the set of all vectors */
	static std::unordered_set<VectorInt> int_vectors;

	// This is the constant vector with only zero entries
	static const VectorInt * zero_int_vector;

	/* initializer */
	void init();

public:

	// Number of counters
	static char N;

	/* called once each time a new monoid is created, given th enumber of counters*/
	static void init_act_prod(char N);

	/* coefficients getters and setters */
	string get(int i, int j) const;

	// Print
	void print(std::ostream& os = std::cout) const;

	// Constructor
	MultiCounterMatrix();

	// Constructor from explicit representation
	MultiCounterMatrix(const ExplicitMatrix &, char N);

	// Function computing the product and stabilization
	// They update the matrices, rows and columns
	// The caller is in charge of deleting the returned object
	Matrix * prod(const Matrix  *) const;

	// compute stabilisation
	// The caller is in charge of deleting the returned object
	Matrix * stab() const;

	// Function checking whether a matrix is idempotent
	bool isIdempotent() const;

	// Equality operator
	bool operator == (const MultiCounterMatrix & mat) const;

	void update_hash()
	{
		_hash = 0;
		for (const VectorInt ** p = cols; p != cols + VectorInt::GetStateNb(); p++)
			_hash ^= std::hash_value((size_t)*p) + 0x9e3779b9 + (_hash << 6) + (_hash >> 2);
		for (const VectorInt ** p = rows; p != rows + VectorInt::GetStateNb(); p++)
			_hash ^= std::hash_value((size_t)*p) + 0x9e3779b9 + (_hash << 6) + (_hash >> 2);
	};

	bool isUnlimitedWitness(const vector<int> & inital_states, const vector<int> & final_states) const;

protected:
	// C-style arrays of size VectorInt::GetStateNb() containing all rows
	const VectorInt ** rows;
	// C-style arrays of size VectorInt::GetStateNb() containing all cols
	const VectorInt ** cols;

	// Function used in the product
	static const VectorInt * sub_prod_int(const VectorInt *, const VectorInt **);

	static const VectorInt * sub_prodor(const VectorInt *, const VectorInt **, const VectorInt *);

	//used in the product of MultiCounter
	static const VectorInt * sub_prod2(const VectorInt * vec1, const VectorInt ** mat1, const VectorInt * vec2, const VectorInt ** mat2);

};

namespace std
{
			template <> struct hash<MultiCounterMatrix>
			{
				size_t operator()(const MultiCounterMatrix & mat) const
				{
					return mat.hash();
				}
			};
}


#endif
