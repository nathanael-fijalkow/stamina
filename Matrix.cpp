
#include "Matrix.hpp"

using namespace std;

// Class ExplicitMatrix
ExplicitMatrix::ExplicitMatrix(uint size) : stateNb(size)
{
	coefficients.resize(size);
	for (auto & row : coefficients)
		row.resize(size);
};

ExplicitMatrix::ExplicitMatrix(const ExplicitMatrix& mat)
{
	stateNb = mat.stateNb;
	coefficients.resize(stateNb);
	for (auto & row : coefficients)
		row.resize(stateNb);
	for (int i=0;i<stateNb;i++)
		for(int j=0;j<stateNb;j++)
			coefficients[i][j]=mat.coefficients[i][j];
}

void ExplicitMatrix::print(std::ostream& os) const
{
	for (int i = 0; i < stateNb; i++)
	{
		for (int j = 0; j < stateNb; j++)
			cout << (int) coefficients[i][j] << " ";
		cout << endl;
	}
}


// Constructor
Matrix::Matrix() : _hash(0)
{
};

//Random matrix
ExplicitMatrix * ExplicitMatrix::random(uint stateNb)
{
	ExplicitMatrix * pe = new ExplicitMatrix(stateNb);
	ExplicitMatrix & e = *pe;
	auto seed = rand();
	for (uint i = 0; i < Vector::GetStateNb(); i++)
	{
		int sel = (stateNb * rand() / RAND_MAX);
		e.coefficients[i][sel] = 2;
		for (uint j = 0; j < stateNb; j++)
			if (j != sel)
				e.coefficients[i][j] = (rand() > seed) ? 2 : 0;
	}
	return pe;
}


std::ostream& operator<< (std::ostream& os, const Matrix & mat){ mat.print(os); return os; };


// The set of known vectors
std::unordered_set<Vector> Matrix::vectors;

// The zero vector
const Vector * Matrix::zero_vector = NULL;

// Construct a vector obtained by multiplying the line vec by all columns of mat
const Vector * Matrix::sub_prod(const Vector * vec, const Vector ** mat){
	if (vec == Matrix::zero_vector)	return Matrix::zero_vector;

	size_t * new_vec = (size_t *)malloc(  Vector::GetBitSize() * sizeof(size_t));
	memset(new_vec, 0, (size_t)(Vector::GetBitSize()  * sizeof(size_t)));


	for (int j = Vector::GetStateNb() - 1; j >= 0; j--)
	{
		//cout << "Vector "; vec->print(); cout << endl;
		//cout << "times "; mat[j]->print(); cout << endl;

		bool ok = false;
		if (mat[j] != Matrix::zero_vector)
			for (uint i = 0; i < Vector::GetBitSize(); i++)
			{
			  ok = ((vec->bits[i]) & (mat[j]->bits[i])) != 0;
			if (ok) break;
			}
		//cout << "Equal " << (ok ? 1 : 0) << endl;
		new_vec[j / (8 * sizeof(uint))] = (new_vec[j / (8 * sizeof(uint)) ] << 1) | (ok ? 1 : 0);
	}
	/* old patch
	if (!(*new_vec))
		return Matrix::zero_vector;
		*/
	auto it = vectors.emplace(new_vec, Vector::GetStateNb()).first;
	free(new_vec);
	//cout << "Final result "; (*it).print(); cout << endl;
	return &(*it);
}

// Create a new vector, keep only coordinates of v that are true in tab
const Vector * Matrix::purge(const Vector *varg, const Vector * tab){
	size_t * new_vec = (size_t *)malloc(Vector::GetBitSize() * sizeof(size_t));

	for (uint i = 0; i < Vector::GetBitSize(); i++)
	  new_vec[i] = ((varg->bits[i]) & (tab->bits[i]));

	auto it = vectors.emplace(new_vec, false).first;
	return &(*it);
}
