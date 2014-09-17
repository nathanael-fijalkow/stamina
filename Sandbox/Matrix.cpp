#include <string.h>
#include <iostream>
#include "Matrix.hpp"

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

// Class Vector
// First constructor
#if USE_SPARSE_MATRIX
Vector::Vector(uint size) : entriesNb(size)
{
	entries = (size_t *)malloc(entriesNb * sizeof(size_t));
}
#else
Vector::Vector(uint size) : entriesNb(size), bitsNb((entriesNb + 8 * sizeof(uint) - 1 ) / (8 * sizeof(uint)))
{
	bits = (uint *)malloc(bitsNb * sizeof(uint));
	memset(bits, (char) 0, bitsNb * sizeof(uint));
}
#endif

// Second constructor
Vector::Vector(const Vector & other) : Vector(other.entriesNb)
{
	_hash = other.Hash();
#if USE_SPARSE_MATRIX
	memcpy(entries, other.entries, entriesNb * sizeof(size_t));
#else
	memcpy(bits, other.bits, bitsNb * sizeof(size_t));
#endif
}

// Third constructor
#if USE_SPARSE_MATRIX
Vector::Vector(vector<size_t> data) : Vector(data.size())
{
	size_t * p = entries;
	for (vector<size_t>::iterator it = data.begin(); it != data.end(); it++)
		*p++ = *it;
#else
Vector::Vector(vector<bool> data) : Vector(data.size())
{
	for (int i = data.size() -1; i >=0 ; i--)
	{
		bits[i / (8 * sizeof(uint))] = (bits[i / (8 * sizeof(uint))] << 1) | (data[i] ? 1 : 0);
//		cout << "i:" << i << " bool " << data[i] << " stored " << bits[i / (8 * sizeof(uint))] << endl;
	}
#endif
	update_hash();
}

// Fourth constructor
#if USE_SPARSE_MATRIX
Vector::Vector(size_t * data, size_t data_size, bool copy) : entriesNb(data_size)
{
	if (copy)
	{
		entries = (size_t *)malloc(entriesNb * sizeof(size_t));
		memcpy(entries, data, entriesNb * sizeof(size_t));
	}
	else
	{
		entries = data;
	}
	update_hash();
};
#else
Vector::Vector(uint * data, size_t data_size, bool copy) : entriesNb(data_size), bitsNb((entriesNb + 8 * sizeof(uint) - 1 )/ (8 * sizeof(uint)))
{
	if (copy)
	{
		bits = (size_t *)malloc( bitsNb * sizeof(uint));
		memcpy(bits, data, bitsNb * sizeof(size_t));
	}
	else
	{
		bits = data;
	}
	update_hash();
};
#endif

void Vector::print(ostream & os) const
{
#if USE_SPARSE_MATRIX
	for (size_t * v = entries; v != entries + entriesNb; v++)
		os << *v << " ";
	os << endl;
#else
	for (int i = 0 ; i < entriesNb; i++)
		os << ( contains(i) ? "1" : "0");
	os << endl;
#endif
}

Vector::~Vector()
{
#if USE_SPARSE_MATRIX
	free(entries);
	entries = NULL;
#else
	free(bits);
	bits = NULL;
#endif
}

bool Vector::operator==(const Vector & vec) const
{
	if (entriesNb != vec.entriesNb) return false;
#if USE_SPARSE_MATRIX
	size_t * v1 = entries;
	size_t * v2 = vec.entries;

	for (; v1 != entries + entriesNb; v1++, v2++)
	{
		if (*v1 != *v2)
			return false;
	}
#else
	for (int i = 0; i < bitsNb; i++)
		if (bits[i] != vec.bits[i]) return false;
#endif
	return true;
}

// Class Matrix
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

// Convert an explicit matrix into a matrix
Matrix::Matrix(const ExplicitMatrix & explMatrix) : stateNb(explMatrix.stateNb)
{
	allocate();
	for (uint i = 0; i < stateNb; i++)
	{
#if USE_SPARSE_MATRIX
		vector<size_t> r_pluses;
		vector<size_t> r_ones;
		vector<size_t> c_pluses;
		vector<size_t> c_ones;
#else
		vector<bool> r_pluses(stateNb);
		vector<bool> r_ones(stateNb);
		vector<bool> c_pluses(stateNb);
		vector<bool> c_ones(stateNb);
#endif

#if USE_SPARSE_MATRIX
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
#else
		for (uint j = 0; j < stateNb; j++)
		{
			char c1 = explMatrix.coefficients[i * stateNb + j];
			r_pluses[j] = (c1 >= 1);
			r_ones[j] = (c1 >= 2);

			char c2 = explMatrix.coefficients[j * stateNb + i];
			c_pluses[j] = (c2 >= 1);
			c_ones[j] = (c2 == 2);
		}
#endif

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
void Matrix::print(ostream & os) const
{
	//cout << "Row description " << endl;
	for (uint i = 0; i < stateNb; i++)
	{
		os << i << ":" << " ";
		const Vector & ones = *row_ones[i];
		const Vector & pluses = *row_pluses[i];

		for(uint j = 0; j < stateNb; j++)
		{
#if USE_SPARSE_MATRIX
			uint jones = 0, jpluses = 0;
			if (ones.entriesNb > jones && ones.entries[jones] == j)
			{
				os << ((pluses.entriesNb > jpluses && pluses.entries[jpluses] == j) ? "2 " : "1 ");
				jones++;
				jpluses++;
			}
			else if (pluses.entriesNb > jpluses && pluses.entries[jpluses] == j)
			{
				os << "+ ";
				jpluses++;
			}
			else
				os << "_ ";
#else
			os << (ones.contains(j) ? "1" : pluses.contains(j) ? "+" : "_");
#endif
		}
		os << endl;
	}
	/*
	Uncomment to get deep description of the matrix, including columns and hashes and adresses
	os << "Col description " << endl;
	for (uint i = 0; i < stateNb; i++)
	{
		os << i << ":" << " ";
		const Vector & ones = *col_ones[i];
		const Vector & pluses = *col_pluses[i];

		uint jones = 0, jpluses = 0;
		for (uint j = 0; j < stateNb; j++)
		{
			if (ones.entriesNb > jones && ones.entries[jones] == j)
			{
				os << ((pluses.entriesNb > jpluses && pluses.entries[jpluses] == j) ? "2 " : "1 ");
				jones++;
				jpluses++;
			}
			else if (pluses.entriesNb > jpluses && pluses.entries[jpluses] == j)
			{
				os << "+ ";
				jpluses++;
			}
			else
				os << "_ ";
		}
		os << endl;
	}
	for (uint i = 0; i < stateNb; i++)
	{
		os << "row " << i << ":" << " ";
		os << "h" << row_ones[i]->Hash() << " #" << row_ones[i] << " h" << row_pluses[i]->Hash() << " #" << row_pluses[i] << " ";
		os << endl;
	}
	for (uint i = 0; i < stateNb; i++)
	{
		os << "col " << i << ":" << " ";
		os << "h" << col_ones[i]->Hash() << " #" << col_ones[i] << " h" << col_pluses[i]->Hash() << " #" << col_pluses[i] << " ";
		os << endl;
	}
	*/
}

ostream& operator<<(ostream& os, const Matrix & mat){ mat.print(os); return os; };

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

// The set of known vectors
unordered_set<Vector> Matrix::vectors;

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
	unordered_set<Vector>::iterator it = vectors.emplace(new_vec, vec->entriesNb, false).first;
	//cout << "Final result "; (*it).print(); cout << endl;
	return &(*it);
#endif
}

// computes the list of recurrent states.
const Vector * Matrix::recurrent_states() const
{
#if USE_SPARSE_MATRIX
	size_t * new_vec = (size_t *)malloc(stateNb * sizeof(size_t));
	size_t * new_vec_start = new_vec;

	for (int i = 0; i < stateNb; i++)
		if (recurrent(i))
			*new_vec++ = i;

	unordered_set<Vector>::iterator it = vectors.emplace(new_vec_start, new_vec - new_vec_start, false).first;
	return &(*it);

#else
	size_t s = (stateNb + 8 * sizeof(uint) - 1)/ (8 * sizeof(uint));

	uint * new_vec = (uint *)malloc(s * sizeof(uint));
	memset((void *)new_vec, (int)0, (size_t) (s * sizeof(uint)));

	for (int i = stateNb-1; i >=0; i--)
		new_vec[i / (8 * sizeof(uint))] = (new_vec[i / (8 * sizeof(uint))] << 1) | (recurrent(i) ? 1 : 0);

	unordered_set<Vector>::iterator it = vectors.emplace(new_vec, stateNb, false).first;
	return &(*it);

#endif
}

/* computes the list of recurrence classesgiven the list of recurrent states.
The matrix is assumed to be idempotent. */
const Vector * Matrix::recurrence_classes(const Vector * recs) const
{
#if USE_SPARSE_MATRIX
	size_t * new_vec = (size_t *)malloc(stateNb * sizeof(size_t));
	size_t * new_vec_start = new_vec;
	memcpy(new_vec,recs,stateNb * sizeof(size_t));

	throw runtime_error("Unimplemented");

	unordered_set<Vector>::iterator it = vectors.emplace(new_vec_start, new_vec - new_vec_start, false).first;
	return &(*it);
#else
	uint s = (stateNb + 8 * sizeof(uint) - 1) / (8 * sizeof(uint));
	uint * new_vec = (uint *)malloc(s * sizeof(uint));
	memcpy(new_vec, recs->bits, recs->bitsNb * sizeof(uint));

	//for each recurrent state we remove all its successors from the list of recurrent states
	uint b = 1;
	for (int i = 0; i < stateNb; i++)
	{
		auto r = row_ones[i];
		if ((new_vec[i / (8 * sizeof(uint))] & b) != 0)
		{
			for (int j = 0; j < stateNb; j++)
			{
				if (i != j && r->contains(j))
				{
					//cout << "State " << j << " is in the same rec class than state " << i << ", deleting." << endl;
					new_vec[j / (8 * sizeof(uint))] &= ~b;
				}
			}
		}
		b *= 2;
	}

	unordered_set<Vector>::iterator it = vectors.emplace(new_vec, stateNb, false).first;
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

	unordered_set<Vector>::iterator it = vectors.emplace(new_vec, varg->entriesNb, false).first;
	return &(*it);
}
#endif

#if USE_SPARSE_MATRIX
#else
uint Matrix::countLeaks(const Vector * classes) const
{
	uint answer = 0;
	for (int i = 0; i < stateNb; i++)
	{
		if (classes->contains(i))
		{
//			cout << "Checking leaks from state " << i << endl;
			for (int j = 0; j < stateNb; j++)
			{
				//				cout << "Checking leaks from state " << i << " to state " << j << endl;
				if (i != j && classes->contains(j) && row_pluses[i]->contains(j))
				{
					answer++;
#if VERBOSE_MONOID_COMPUTATION
					cout << "Found leak from class " << i << " to class " << j << endl;
#endif
	//				print();
				}
			}
		}
	}
	return answer;
}
#endif

bool Matrix::check() const
{
	//at least one 1 per line
	for (int i = 0; i < stateNb; i++)
		if (row_ones[i] == Matrix::zero_vector)
			return false;
}

// Function computing the product of two matrices
Matrix Matrix::prod(const Matrix & mat1, const Matrix & mat2)
{
	uint n = mat1.stateNb;
    Matrix result(n);

	for (uint i = 0; i < n; i++)
	{
		result.row_ones[i] = sub_prod( mat1.row_ones[i], mat2.col_ones, n);
		result.row_pluses[i] = sub_prod( mat1.row_pluses[i], mat2.col_pluses, n);
		result.col_ones[i] = sub_prod( mat2.col_ones[i], mat1.row_ones, n);
		result.col_pluses[i] = sub_prod( mat2.col_pluses[i], mat1.row_pluses, n);
	}

	result.update_hash();
	return result;
}

// Function checking whether j is recurrent. Only works if this is idempotent
bool Matrix::recurrent (int j) const{
#if USE_SPARSE_MATRIX
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
#else
	auto r = row_ones[j];
	for (int i = 0; i < stateNb; i++)
		if (r->contains(i) && !row_ones[i]->contains(j))
			return false;
#endif
	return true;
}


// Function computing the stabilization. mat is assumed to be idempotent
#if USE_SPARSE_MATRIX
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
		result.row_ones[i] = purge(mat.row_ones[i], tabrec);
    }
#else
Matrix Matrix::stab(const Matrix & mat, const Vector * recs)
{
	uint n = mat.stateNb;
	Matrix result(n);

	for (uint i = 0; i < mat.stateNb; i++)
	{
		result.row_ones[i] = purge(mat.row_ones[i], recs);
		result.row_pluses[i] = mat.row_pluses[i];
	}

	for (uint j = 0; j < mat.stateNb; j++)
	{
		if (recs->contains(j))
			result.col_ones[j] = mat.col_ones[j];
		else
			result.col_ones[j] = Matrix::zero_vector;
		result.col_pluses[j] = mat.col_pluses[j];
	}

#endif
	result.update_hash();
	return result;
}

bool Matrix::isIdempotent() const
{
	return (* this == Matrix::prod(* this,* this));
}
