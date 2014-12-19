#ifndef ONECOUNTERMATRIX_HPP
#define ONECOUNTERMATRIX_HPP

#include "Matrix.hpp"

class OneCounterMatrix : public Matrix
{

	//encoding for counter actions, smaller is better
#define RESET 0
#define EPS 1
#define INC 2
#define OM 3
#define BOT 4

public:
	// Constructor from explicit representation
	OneCounterMatrix(const ExplicitMatrix &);

	// Constructor from ENTRIESnB
	OneCounterMatrix(int nbState);

	// Print
	void print(std::ostream& os = std::cout) const;

	// Equality operator
	bool operator == (const OneCounterMatrix & mat) const;

	// Function computing the product and stabilization
	// They update the matrices, rows and columns
	//The caller is in charge of deleting the returned object
	Matrix * prod(const Matrix  *) const;

	// compute stabilisation
	//The caller is in charge of deleting the returned object
	Matrix * stab() const;

	// Function checking whether a matrix is idempotent
	bool isIdempotent() const;

protected:

	// Four C-style matrices of size stateNb containing all rows, state per state
	const Vector *** rows;
	const Vector *** cols;

	void update_hash()
	{
		_hash = 0;
		for (int i = 0; i<4; i++){
			for (const Vector ** p = rows[i]; p != rows[i] + stateNb; p++)
				_hash ^= hash_value((size_t)*p) + 0x9e3779b9 + (_hash << 6) + (_hash >> 2);
			for (const Vector ** p = cols[i]; p != cols[i] + stateNb; p++)
				_hash ^= hash_value((size_t)*p) + 0x9e3779b9 + (_hash << 6) + (_hash >> 2);
		}
	};

	// Function allocating memory for rows and cols
	void allocate();

};

namespace std
{

			template <> struct hash<OneCounterMatrix>
			{
				size_t operator()(const OneCounterMatrix & mat) const
				{
					return mat.hash();
				}
			};
}


#endif