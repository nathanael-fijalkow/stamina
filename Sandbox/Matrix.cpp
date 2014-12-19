#include "Matrix.hpp"

using namespace std;

// Class ExplicitMatrix
ExplicitMatrix::ExplicitMatrix(uint stateNb) : stateNb(stateNb)
{
	coefficients = (char *)malloc(stateNb * stateNb * sizeof(char));
};

ExplicitMatrix::~ExplicitMatrix()
{
	free(coefficients);
	coefficients = NULL;
};


// Constructor
Matrix::Matrix(uint stateNb) : _hash(0), stateNb(stateNb)
{
};

//Random matrix
ExplicitMatrix * ExplicitMatrix::random(uint stateNb)
{
	ExplicitMatrix * pe = new ExplicitMatrix(stateNb);
	ExplicitMatrix & e = *pe;
	for (int i = 0; i < stateNb; i++)
	{
		int sel = (stateNb * rand() / (RAND_MAX + 1));
		e.coefficients[i*stateNb + sel] = 2;
		for (int j = 0; j < stateNb; j++)
			if (j != sel)
				e.coefficients[i*stateNb + j] = (rand() < RAND_MAX / 30) ? 2 : 0;
	}
	return pe;
}


std::ostream& operator<< (std::ostream& os, const Matrix & mat){ mat.print(os); return os; };


// The set of known vectors
std::unordered_set<Vector> Matrix::vectors;

// The zero vector
const Vector * Matrix::zero_vector = NULL;

// Construct a vector obtained by multiplying the line vec by all columns of mat
const Vector * Matrix::sub_prod(const Vector * vec, const Vector ** mat, size_t stateNb){
	if (vec == Matrix::zero_vector)	return Matrix::zero_vector;

#if USE_SPARSE_MATRIX

	size_t * new_vec = (size_t *)malloc(stateNb * sizeof(size_t));
	size_t * new_vec_start = new_vec;

	for (uint j = 0; j < stateNb; j++)
	{
		if (mat[j] == Matrix::zero_vector) continue;

		const size_t * vec_entries = vec->entries;
		const size_t * vec_entries_end = vec->end();

		const size_t * mat_entries_j = mat[j]->entries;
		const size_t * mat_entries_j_end = mat[j]->end();

		while(vec_entries != vec_entries_end && mat_entries_j != mat_entries_j_end)
		{
			if (*vec_entries == *mat_entries_j)
			{
                *new_vec++ = j;
				break;
			}
            else
            {
                if(*vec_entries<*mat_entries_j)
                    vec_entries++;
                else
                    mat_entries_j++;
            } ;
		} ;
	}
	unordered_set<Vector>::iterator it = vectors.emplace(new_vec_start, new_vec - new_vec_start, false).first;
	return &(*it);
#else

	uint * new_vec = (uint *)malloc(  vec->bitsNb * sizeof(uint));
	memset(new_vec, 0, (size_t) ( vec->bitsNb  * sizeof(uint) ));


	for (int j = vec->entriesNb - 1; j >=0; j--)
	{
		//cout << "Vector "; vec->print(); cout << endl;
		//cout << "times "; mat[j]->print(); cout << endl;

		bool ok = false;
		if (mat[j] != Matrix::zero_vector)
			for (int i = 0; i < vec->bitsNb; i++)
			{
			ok = (vec->bits[i] & mat[j]->bits[i]) != 0;
			if (ok) break;
			}
		//cout << "Equal " << (ok ? 1 : 0) << endl;
		new_vec[j / (8 * sizeof(uint))] = (new_vec[j / (8 * sizeof(uint)) ] << 1) | (ok ? 1 : 0);
	}
	auto it = vectors.emplace(new_vec, vec->entriesNb, false).first;
	//cout << "Final result "; (*it).print(); cout << endl;
	return &(*it);
#endif
}


// Create a new vector, keep only coordinates of v that are true in tab
#if USE_SPARSE_MATRIX
const Vector * Matrix::purge(const Vector *varg, bool * tab){
	// size of purged vector to precompute
	int nbtab = 0;
	for (size_t * ent = varg->entries; ent != varg->entries + varg->entriesNb; ent++){
		if (tab[*ent]) nbtab++;
	}

	size_t * data = (size_t*)malloc(nbtab*(sizeof(size_t)));
	size_t * datastart = data;
	for (size_t * ent = varg->entries; ent != varg->entries + varg->entriesNb; ent++)
		if (tab[*ent]) *data++ = *ent;

	unordered_set<Vector>::iterator it = vectors.emplace(datastart, nbtab, false).first;
	return &(*it);
}
#else
// Create a new vector, keep only coordinates of v that are true in tab
const Vector * Matrix::purge(const Vector *varg, const Vector * tab){
	uint * new_vec = (uint *) malloc(varg->bitsNb * sizeof(uint));

	for (int i = 0; i < varg->bitsNb; i++)
		new_vec[i] = (varg->bits[i] & tab->bits[i]);

	auto it = vectors.emplace(new_vec, varg->entriesNb, false).first;
	return &(*it);
}
#endif


