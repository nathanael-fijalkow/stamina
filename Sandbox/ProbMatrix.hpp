#ifndef PROBMATRIX_HPP
#define PROBMATRIX_HPP

#include <Matrix.hpp>


class ProbMatrix : public Matrix
{
public:
	// Constructor from explicit representation
	ProbMatrix(const ExplicitMatrix &);

	// Print
	void print(std::ostream& os = std::cout) const;

	// Equality operator
	bool operator == (const ProbMatrix & mat) const;

	// Function computing the product and stabilization
	// They update the matrices, rows and columns
	//The caller is in charge of deleting the returned object
	Matrix * prod(const Matrix  *) const;

	// compute stabilisation
	//The caller is in charge of deleting the returned object
	Matrix * stab() const;

	// computes the list of recurrent states.
	const Vector * recurrent_states() const;

	/* computes the list of recurrence classes given the list of recurrent states.
	The matrix is assumed to be idempotent. */
	const Vector * recurrence_classes(const Vector *) const;

	// Count the number of leaks given the list of recurrence classes
	//the matrix is assumed to be idempotent
	uint countLeaks(const Vector * classes) const;

	//check matrix is well formed
	bool check() const;

	// Function checking whether a matrix is idempotent
	bool isIdempotent() const;

protected:

	// Four C-style matrices of size stateNb containing all rows, state per state
	const Vector ** row_pluses;
	const Vector ** row_ones;
	const Vector ** col_pluses;
	const Vector ** col_ones;

	void update_hash()
	{
		_hash = 0;
		for (const Vector ** p = col_ones; p != col_ones + stateNb; p++)
			_hash ^= std::hash_value((size_t)*p) + 0x9e3779b9 + (_hash << 6) + (_hash >> 2);
		for (const Vector ** p = col_pluses; p != col_pluses + stateNb; p++)
			_hash ^= std::hash_value((size_t)*p) + 0x9e3779b9 + (_hash << 6) + (_hash >> 2);
		for (const Vector ** p = row_ones; p != row_ones + stateNb; p++)
			_hash ^= std::hash_value((size_t)*p) + 0x9e3779b9 + (_hash << 6) + (_hash >> 2);
		for (const Vector ** p = row_pluses; p != row_pluses + stateNb; p++)
			_hash ^= std::hash_value((size_t)*p) + 0x9e3779b9 + (_hash << 6) + (_hash >> 2);
	};

	// Function allocating memory for pluses and ones
	void allocate();

	// Function checking whether a state is recurrent
	//the matrix is assumed idempotent
	bool recurrent(int) const;
};

namespace std
{

		template <> struct hash<ProbMatrix>
		{
			size_t operator()(const ProbMatrix & mat) const
			{
				return mat.hash();
			}
		};

}


#endif
