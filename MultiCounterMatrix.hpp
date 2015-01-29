#ifndef MULTICOUNTERMATRIX_HPP
#define MULTICOUNTERMATRIX_HPP

#include "Matrix.hpp"




class MultiCounterMatrix : public Matrix
{

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

public:
	// Number of counters
	int N;
	
	// Size
	int Nb_state;

	// This matrix mat_prod is of size (2N+2)*(2N+2), it is computed once and for all.
	// Quel est le mot clé pour la rendre globale ?
	int ** mat_prod;

	// Print
	void print(std::ostream& os = std::cout) const;

	// Constructor
	MultiCounterMatrix();

	// Constructor from explicit representation
	MultiCounterMatrix(const ExplicitMatrix &);

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
	
protected:
	// C-style matrices of size VectorInt::GetStateNb() containing all rows
	const VectorInt ** rows;
	// C-style matrices of size VectorInt::GetStateNb() containing all cols
	const VectorInt ** cols;

	void update_hash()
	{
		_hash = 0;
		for (const VectorInt ** p = cols; p != cols + VectorInt::GetStateNb(); p++)
			_hash ^= std::hash_value((size_t)*p) + 0x9e3779b9 + (_hash << 6) + (_hash >> 2);
		for (const VectorInt ** p = rows; p != rows + VectorInt::GetStateNb(); p++)
			_hash ^= std::hash_value((size_t)*p) + 0x9e3779b9 + (_hash << 6) + (_hash >> 2);
	};

	static const Vector * sub_prodor(const Vector *, const Vector **, const Vector *);

	//used in the product of MultiCounter
	static const Vector * sub_prod2(const Vector * vec1, const Vector ** mat1, const Vector * vec2, const Vector ** mat2);

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
