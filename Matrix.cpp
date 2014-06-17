#include <string.h>
#include <iostream>

#include "Matrix.hpp"


ExplicitMatrix::ExplicitMatrix(uint stateNb) : stateNb(stateNb)
{
	coefficients = (char *)malloc(stateNb * stateNb * sizeof(char));
};

ExplicitMatrix::~ExplicitMatrix()
{
	free(coefficients);
	coefficients = NULL;
};

Vector::Vector(uint size) : entriesNb(size)
{
	entries = (size_t *)malloc(entriesNb
		* sizeof(size_t));
}

Vector::Vector(vector<size_t> data) : entriesNb(data.size())
{
	entries = (size_t *)malloc(entriesNb * sizeof(size_t));
	size_t * p = entries;
	for (vector<size_t>::iterator it = data.begin(); it != data.end(); it++)
		*p++ = *it;
	update_hash();
}

Vector::Vector(size_t * data, size_t data_size) : entriesNb(data_size)
{
	entries = (size_t *)malloc(entriesNb * sizeof(size_t));
	memcpy(entries, data, entriesNb * sizeof(size_t));
	update_hash();
};

Vector::Vector(const Vector & other) : entriesNb(other.entriesNb), _hash(other.Hash())
{
	entries = (size_t *)malloc(entriesNb * sizeof(size_t));
	memcpy(entries, other.entries, entriesNb * sizeof(size_t));
}

void Vector::print() const
{
	for (size_t * v = entries; v != entries + entriesNb; v++)
	{
		cout << *v << " ";
	}
	cout << endl;
}


Vector::~Vector()
{
	free(entries);
	entries = NULL;
}

bool Vector::operator==(const Vector & vec) const
{
	if (entriesNb != vec.entriesNb) return false;
	size_t * v1 = entries;
	size_t * v2 = vec.entries;

	for (; v1 != entries + entriesNb; v1++, v2++)
	{
		if (*v1 != *v2)
			return false;
	}

	return true;
}

// The set of known vectors
unordered_set<Vector> Matrix::vectors;

// The zero vector
const Vector * Matrix::zero_vector = NULL;

void Matrix::allocate()
{
	row_pluses = (const Vector **)malloc(stateNb * sizeof(void *));
	row_ones = (const Vector **)malloc(stateNb * sizeof(void *));
	col_pluses = (const Vector **)malloc(stateNb * sizeof(void *));
	col_ones = (const Vector **)malloc(stateNb * sizeof(void *));
}

// Constructor
Matrix::Matrix(uint stateNb) : _hash(0), stateNb(stateNb)
{
	allocate();
};

// Convert an explicit matrix into a matrix
Matrix::Matrix(const ExplicitMatrix & explMatrix) : stateNb(explMatrix.stateNb)
{
	allocate();
	for (uint i = 0; i < stateNb; i++)
	{

		vector<size_t> r_pluses;
		vector<size_t> r_ones;
		vector<size_t> c_pluses;
		vector<size_t> c_ones;
		for (uint j = 0; j < stateNb; j++)
		{
			char c1 = explMatrix.coefficients[i * stateNb + j];
			if (c1 >= 1)
				r_pluses.push_back(j);
			if (c1 == 2)
				r_ones.push_back(j);

			char c2 = explMatrix.coefficients[j * stateNb + i];
			if (c2 >= 1)
				c_pluses.push_back(j);
			if (c2 == 2)
				c_ones.push_back(j);
		}

		unordered_set<Vector>::iterator it = vectors.insert(r_ones).first;
		row_ones[i] = &(*it);

		it = vectors.insert(r_pluses).first;
		row_pluses[i] = &(*it);

		it = vectors.insert(c_ones).first;
		col_ones[i] = &(*it);

		it = vectors.insert(c_pluses).first;
		col_pluses[i] = &(*it);
	}
	update_hash();
}

// Print
void Matrix::print() const
{
	for (uint i = 0; i < stateNb; i++)
	{
		cout << i << ":" << " ";
		const Vector & ones = *row_ones[i];
		const Vector & pluses = *row_pluses[i];

		uint jones = 0, jpluses = 0;
		for(uint j = 0; j < stateNb; j++)
		{
			if (ones.entriesNb > jones && ones.entries[jones] == j)
			{
				cout << "1 ";
				jones++;
				jpluses++;
			}
			else if (pluses.entriesNb > jpluses && pluses.entries[jpluses] == j)
			{
				cout << "+ ";
				jpluses++;
			}
			else
				cout << "_ ";
		}
		cout << endl;
	}
}

// Construct a vector obtained by multiplying the line vec by all columns of mat
const Vector * Matrix::sub_prod(
	const Vector * vec,
	const Vector ** mat,
	size_t stateNb
	)
{
	if (vec == Matrix::zero_vector)	return Matrix::zero_vector;

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

	unordered_set<Vector>::iterator it = vectors.emplace(new_vec_start, new_vec - new_vec_start).first;
	free(new_vec_start);
	return &(*it);
}

// Function computing the product of two matrices
Matrix Matrix::prod(const Matrix & mat1, const Matrix & mat2)
{
	uint n = mat1.stateNb;
    Matrix result(n);

	for (uint i = 0; i < n; i++)
	{
		result.row_ones[i] = sub_prod(mat1.row_ones[i],mat2.col_ones,n);
		result.row_pluses[i] = sub_prod(mat1.row_pluses[i],mat2.col_pluses,n);
		result.col_ones[i] = sub_prod(mat2.col_ones[i],mat1.row_ones,n);
		result.col_pluses[i] = sub_prod(mat2.col_pluses[i],mat1.row_pluses,n);
	}

	result.update_hash();
	return result;
}

// Function checking whether j is recurrent. Only works if this is idempotent
bool Matrix::recurrent (int j) const{
    size_t * vpred = this->col_ones[j]->entries;
    size_t * vmax = this->col_ones[j]->entries+this->col_ones[j]->entriesNb;
    size_t * vstart = this->row_ones[j]->entries;
    size_t * vend = vstart + this->row_ones[j]->entriesNb;

    for (size_t * vsucc = vstart; vsucc != vend ; vsucc++)
	{
            while(*vpred<*vsucc) {vpred++; if (vpred==vmax) return false;}
            if(*vpred!=*vsucc) return false;
            else vpred++;
	}
	return true;
}

// Create a new vector, keep only coordinates of v that are true in tab
Vector purge(const Vector *varg,bool * tab){
    int nbtab=0;//size of purged vector to precompute
        for (size_t * ent = varg->entries; ent != varg->entries + varg->entriesNb; ent++)
	{
        if(tab[*ent]) nbtab++;
    }//end of precomputation
    size_t * data = (size_t*) malloc (nbtab*(sizeof(size_t)));
    size_t * datastart = data;
    for (size_t * ent = varg->entries; ent != varg->entries + varg->entriesNb; ent++)
	{
        if(tab[*ent]) *data++=*ent;
    }
    Vector result(datastart,nbtab);
	return result;
}

// Function computing the stabilization. Only works on idempotent
Matrix Matrix::stab(const Matrix & mat) 
{
    uint n=mat.stateNb;
    Matrix result(n);

    // Precompute recurrent elements, and columns
	bool *tabrec = (bool *) malloc (n*sizeof(bool));
	for(uint j = 0 ; j < n ; j++){
		tabrec[j] = mat.recurrent(j);
		if (tabrec[j]) result.col_ones[j] = mat.col_ones[j] ;
		else result.col_ones[j] = Matrix::zero_vector;

		result.col_pluses[j] = mat.col_pluses[j];
    }
    for(uint i = 0; i < n ; i++){
        result.row_pluses[i] = mat.row_pluses[i];
        Vector v = purge(mat.row_ones[i],tabrec);
        unordered_set<Vector>::iterator it = Matrix::vectors.emplace(v).first;
        result.row_ones[i] = &(*it);
    }

    result.update_hash();
    return result;
}

bool Matrix::isIdempotent() const
{
	return false;
}

bool Matrix::operator==(const Matrix & mat) const
{
	if (mat.stateNb != stateNb) return false;
	if (mat._hash != _hash) return false;

	const Vector ** row = row_ones;
	const Vector ** row1 = mat.row_ones;
	for (; row != row_ones + stateNb; row++, row1++)
	{
		if (*row != *row1) return false;
	}

	row = row_pluses;
	row1 = mat.row_pluses;
	for (; row != row_pluses + stateNb; row++, row1++)
	{
		if (*row != *row1) return false;
	}

	return true;
};
