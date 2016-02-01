
#ifndef PROBMATRIX_HPP
#define PROBMATRIX_HPP

#include "Matrix.hpp"


class ProbMatrix : public Matrix
{
public:
	// Constructor from explicit representation
	ProbMatrix(const ExplicitMatrix &);
    
    virtual ~ProbMatrix();

    //copy constructor and assignment operator
    ProbMatrix( const ProbMatrix& other );
    ProbMatrix& operator=( const ProbMatrix& other );
    
	// Print
	void print(std::ostream& os = std::cout, vector<string> state_names = vector<string>()) const;

    ExplicitMatrix* toExplicitMatrix() const;


	// Equality operator
	bool operator == (const ProbMatrix & mat) const;
    //copy constructor
    
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
        
        const Vector *const *const getRowOnes() const;
	const Vector *const *const getRowPluses() const;

protected:

	// Four C-style matrices of size Vector::GetStateNb() containing all rows and cols, state per state, each has length Vector::GetStateNb(), first row_pluses then row_one then col_pluses then col_ones //
    const Vector ** myvectors;
    
    const Vector ** row_pluses() const { return myvectors; }
	const Vector ** row_ones() const { return myvectors + Vector::GetStateNb(); }
	const Vector ** col_pluses() const { return myvectors + 2*Vector::GetStateNb(); }
	const Vector ** col_ones() const { return myvectors + 3*Vector::GetStateNb(); }

	void update_hash()
	{
		_hash = 0;
        auto N = 4*Vector::GetStateNb();
        for (const Vector ** p = myvectors; p != myvectors + N; p++)
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
