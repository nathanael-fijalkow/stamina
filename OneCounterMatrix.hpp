
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
	/* coefficients getters and setters */
	virtual char get(int i, int j) const = 0;

	// Print
	void print(std::ostream& os = std::cout, vector<string> state_names = vector<string>()) const;

        ExplicitMatrix* toExplicitMatrix() const;

};

/* Matrices of dimension less than sizeof(uint) */
class OneCounterSmallMatrix : public OneCounterMatrix
{

public:
	// Constructor from explicit representation
	OneCounterSmallMatrix(const ExplicitMatrix &);

    //Copy constructor
    OneCounterSmallMatrix(const OneCounterSmallMatrix &);
    
    //Assignement operator, performs copy
    OneCounterSmallMatrix & operator=(const OneCounterSmallMatrix &);

	// Equality operator
	bool operator == (const OneCounterSmallMatrix & mat) const;

	// Function checking whether a matrix is idempotent
	bool isIdempotent() const;

	/* coefficients getters */
	char get(int i, int j) const;

	// Function computing the product and stabilization
	// They update the matrices, rows and columns
	//The caller is in charge of deleting the returned object
	Matrix * prod(const Matrix  *) const;

	// compute stabilisation
	//The caller is in charge of deleting the returned object
	Matrix * stab() const;

	void update_hash()
	{
		_hash = 0;
		for (unsigned char c = 0; c < 4; c++)
		{
			for (uint i = 0; i < Vector::GetStateNb(); i++)
			{
				_hash ^= hash_value((int) rows[c][i]) + 0x9e3779b9 + (_hash << 6) + (_hash >> 2);
				_hash ^= hash_value((int) cols[c][i]) + 0x9e3779b9 + (_hash << 6) + (_hash >> 2);
			}
		}
	}

protected:
	/* array of size 4 of arrays of size Vector::StateNb containing bit representation */
	uint ** rows;
	uint ** cols;

	/* intializes memory, for C++0x constructor compatibility */
	void init();
    
    //one shall not contruct a matrix from nothing
    OneCounterSmallMatrix();
    
    void copy_from(const OneCounterSmallMatrix & other);
    
};

/* Matrices of dimension more than sizeof(uint) */

class OneCounterLargeMatrix : public OneCounterMatrix
{
public:
	// Constructor from explicit representation
	OneCounterLargeMatrix(const ExplicitMatrix &);

    //Copy constructor
    OneCounterLargeMatrix(const OneCounterLargeMatrix &);
    
    //Assignement operator, performs copy
    OneCounterLargeMatrix & operator=(const OneCounterLargeMatrix &);

	// Function computing the product and stabilization
	// They update the matrices, rows and columns
	//The caller is in charge of deleting the returned object
	Matrix * prod(const Matrix  *) const;

	// compute stabilisation
	//The caller is in charge of deleting the returned object
	Matrix * stab() const;

	// Function checking whether a matrix is idempotent
	bool isIdempotent() const;

	// Equality operator
	bool operator == (const OneCounterLargeMatrix & mat) const;

	/* coefficients getters */
	char get(int i, int j) const;

protected:
	// Four C-style matrices of size Vector::GetStateNb() containing all rows, state per state
	const Vector *** rows;
	const Vector *** cols;
    void init();
    void copy_from(const OneCounterLargeMatrix & other);

    //you shall not contruct a matrix from nothing
    OneCounterLargeMatrix();
    

	void update_hash()
	{
		_hash = 0;
		for (int i = 0; i<4; i++){
			for (const Vector ** p = rows[i]; p != rows[i] + Vector::GetStateNb(); p++)
				_hash ^= hash_value((size_t)*p) + 0x9e3779b9 + (_hash << 6) + (_hash >> 2);
			for (const Vector ** p = cols[i]; p != cols[i] + Vector::GetStateNb(); p++)
				_hash ^= hash_value((size_t)*p) + 0x9e3779b9 + (_hash << 6) + (_hash >> 2);
		}
	};

	static const Vector * sub_prodor(const Vector *, const Vector **, const Vector *);

	//used in the product of OneCounter
	static const Vector * sub_prod2(const Vector * vec1, const Vector ** mat1, const Vector * vec2, const Vector ** mat2);



};

namespace std
{

			template <> struct hash<OneCounterLargeMatrix>
			{
				size_t operator()(const OneCounterLargeMatrix & mat) const
				{
					return mat.hash();
				}
			};

				template <> struct hash<OneCounterSmallMatrix>
				{
					size_t operator()(const OneCounterSmallMatrix & mat) const
					{
						return mat.hash();
					}
				};

					template <> struct hash<OneCounterMatrix>
					{
						size_t operator()(const OneCounterMatrix & mat) const
						{
							return mat.hash();
						}
					};

}


#endif
