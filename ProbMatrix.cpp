#include <ProbMatrix.hpp>

using namespace std;

// Class Matrix
void ProbMatrix::allocate()
{
	row_pluses = (const Vector **)malloc(Vector::GetStateNb() * sizeof(void *));
	row_ones = (const Vector **)malloc(Vector::GetStateNb() * sizeof(void *));
	col_pluses = (const Vector **)malloc(Vector::GetStateNb() * sizeof(void *));
	col_ones = (const Vector **)malloc(Vector::GetStateNb() * sizeof(void *));
}


// Convert an explicit matrix into a matrix
ProbMatrix::ProbMatrix(const ExplicitMatrix & explMatrix)
{
	allocate();

	for (uint i = 0; i < Vector::GetStateNb(); i++)
	{
#if USE_SPARSE_MATRIX
		vector<size_t> r_pluses;
		vector<size_t> r_ones;
		vector<size_t> c_pluses;
		vector<size_t> c_ones;
#else
		vector<bool> r_pluses(Vector::GetStateNb());
		vector<bool> r_ones(Vector::GetStateNb());
		vector<bool> c_pluses(Vector::GetStateNb());
		vector<bool> c_ones(Vector::GetStateNb());
#endif

#if USE_SPARSE_MATRIX
		for (uint j = 0; j < Vector::GetStateNb(); j++)
		{
			char c1 = explMatrix.coefficients[i * Vector::GetStateNb() + j];
			if (c1 >= 1)
				r_pluses.push_back(j);
			if (c1 == 2)
				r_ones.push_back(j);

			char c2 = explMatrix.coefficients[j * Vector::GetStateNb() + i];
			if (c2 >= 1)
				c_pluses.push_back(j);
			if (c2 == 2)
				c_ones.push_back(j);
		}
#else
		for (uint j = 0; j < Vector::GetStateNb(); j++)
		{
			char c1 = explMatrix.coefficients[i * Vector::GetStateNb() + j];
			r_pluses[j] = (c1 >= 1);
			r_ones[j] = (c1 >= 2);

			char c2 = explMatrix.coefficients[j * Vector::GetStateNb() + i];
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
void ProbMatrix::print(std::ostream & os) const
{
	//cout << "Row description " << endl;
	for (uint i = 0; i < Vector::GetStateNb(); i++)
	{
		os << i << ":" << " ";
		const Vector & ones = *row_ones[i];
		const Vector & pluses = *row_pluses[i];

		for (uint j = 0; j < Vector::GetStateNb(); j++)
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
	for (uint i = 0; i < Vector::GetStateNb(); i++)
	{
	os << i << ":" << " ";
	const Vector & ones = *col_ones[i];
	const Vector & pluses = *col_pluses[i];

	uint jones = 0, jpluses = 0;
	for (uint j = 0; j < Vector::GetStateNb(); j++)
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
	for (uint i = 0; i < Vector::GetStateNb(); i++)
	{
	os << "row " << i << ":" << " ";
	os << "h" << row_ones[i]->Hash() << " #" << row_ones[i] << " h" << row_pluses[i]->Hash() << " #" << row_pluses[i] << " ";
	os << endl;
	}
	for (uint i = 0; i < Vector::GetStateNb(); i++)
	{
	os << "col " << i << ":" << " ";
	os << "h" << col_ones[i]->Hash() << " #" << col_ones[i] << " h" << col_pluses[i]->Hash() << " #" << col_pluses[i] << " ";
	os << endl;
	}
	*/
}


bool ProbMatrix::operator==(const ProbMatrix & mat) const
{
	if (mat._hash != _hash) return false;

	const Vector ** row = row_ones;
	const Vector ** row1 = mat.row_ones;
	for (; row != row_ones + Vector::GetStateNb(); row++, row1++)
	{
		if (*row != *row1) return false;
	}

	row = row_pluses;
	row1 = mat.row_pluses;
	for (; row != row_pluses + Vector::GetStateNb(); row++, row1++)
	{
		if (*row != *row1) return false;
	}

	return true;
};

// computes the list of recurrent states.
const Vector * ProbMatrix::recurrent_states() const
{
#if USE_SPARSE_MATRIX
	size_t * new_vec = (size_t *)malloc(Vector::GetStateNb() * sizeof(size_t));
	size_t * new_vec_start = new_vec;

	for (int i = 0; i < Vector::GetStateNb(); i++)
		if (recurrent(i))
			*new_vec++ = i;

	unordered_set<Vector>::iterator it = vectors.emplace(new_vec_start, new_vec - new_vec_start, false).first;
	return &(*it);

#else
	size_t s = (Vector::GetStateNb() + 8 * sizeof(uint) - 1) / (8 * sizeof(uint));

	size_t * new_vec = (size_t *)malloc(s * sizeof(size_t));
	memset((void *)new_vec, (int)0, (size_t)(s * sizeof(size_t)));

	for (int i = Vector::GetStateNb() - 1; i >= 0; i--)
		new_vec[i / (8 * sizeof(uint))] = (new_vec[i / (8 * sizeof(uint))] << 1) | (recurrent(i) ? 1 : 0);

	auto it = vectors.emplace(new_vec, false).first;
	return &(*it);

#endif
}

/* computes the list of recurrence classesgiven the list of recurrent states.
The matrix is assumed to be idempotent. */
const Vector * ProbMatrix::recurrence_classes(const Vector * recs) const
{
#if USE_SPARSE_MATRIX
	size_t * new_vec = (size_t *)malloc(Vector::GetStateNb() * sizeof(size_t));
	size_t * new_vec_start = new_vec;
	memcpy(new_vec, recs, Vector::GetStateNb() * sizeof(size_t));

	throw runtime_error("Unimplemented");

	unordered_set<Vector>::iterator it = vectors.emplace(new_vec_start, new_vec - new_vec_start, false).first;
	return &(*it);
#else
	uint s = (Vector::GetStateNb() + 8 * sizeof(uint) - 1) / (8 * sizeof(uint));
	size_t * new_vec = (size_t *)malloc(s * sizeof(size_t));
	memcpy(new_vec, recs->bits, Vector::GetBitSize() * sizeof(size_t));

	//for each recurrent state we remove all its successors from the list of recurrent states
	uint b = 1;
	for (uint i = 0; i < Vector::GetStateNb(); i++)
	{
		auto r = row_ones[i];
		if ((new_vec[i / (8 * sizeof(uint))] & b) != 0)
		{
			for (uint j = 0; j < Vector::GetStateNb(); j++)
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

	auto it = vectors.emplace(new_vec, false).first;
	return &(*it);


#endif

}


#if USE_SPARSE_MATRIX
#else
uint ProbMatrix::countLeaks(const Vector * classes) const
{
	uint answer = 0;
	for (uint i = 0; i < Vector::GetStateNb(); i++)
	{
		if (classes->contains(i))
		{
			//			cout << "Checking leaks from state " << i << endl;
			for (uint j = 0; j < Vector::GetStateNb(); j++)
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

bool ProbMatrix::check() const
{
	//at least one 1 per line
	for (uint i = 0; i < Vector::GetStateNb(); i++)
		if (row_ones[i] == Matrix::zero_vector)
			return false;
	return true;
}

// Function computing the product of two matrices
Matrix * ProbMatrix::prod(const Matrix * pmat1) const
{
	const ProbMatrix & mat1 = *this;
	const ProbMatrix & mat2 = *(ProbMatrix *)pmat1;
	uint n = Vector::GetStateNb();
	ProbMatrix * result = new ProbMatrix(n);

	for (uint i = 0; i < n; i++)
	{
		result->row_ones[i] = sub_prod(mat1.row_ones[i], mat2.col_ones);
		result->row_pluses[i] = sub_prod(mat1.row_pluses[i], mat2.col_pluses);
		result->col_ones[i] = sub_prod(mat2.col_ones[i], mat1.row_ones);
		result->col_pluses[i] = sub_prod(mat2.col_pluses[i], mat1.row_pluses);
	}

	result->update_hash();
	return result;
}

// Function checking whether j is recurrent. Only works if this is idempotent
bool ProbMatrix::recurrent(int j) const{
#if USE_SPARSE_MATRIX
	size_t * vpred = this->col_ones[j]->entries;
	size_t * vmax = this->col_ones[j]->entries + this->col_ones[j]->entriesNb;
	size_t * vstart = this->row_ones[j]->entries;
	size_t * vend = vstart + this->row_ones[j]->entriesNb;

	for (size_t * vsucc = vstart; vsucc != vend; vsucc++)
	{
		while (*vpred<*vsucc) { vpred++; if (vpred == vmax) return false; }
		if (*vpred != *vsucc) return false;
		else vpred++;
	}
#else
	auto r = row_ones[j];
	for (uint i = 0; i < Vector::GetStateNb(); i++)
		if (r->contains(i) && !row_ones[i]->contains(j))
			return false;
#endif
	return true;
}

// Function computing the stabilization. mat is assumed to be idempotent
#if USE_SPARSE_MATRIX
Matrix Matrix::stab(const Matrix & mat)
{
	uint n = mat.Vector::GetStateNb();
	Matrix result(n);

	// Precompute recurrent elements, and columns
	bool *tabrec = (bool *)malloc(n*sizeof(bool));
	for (uint j = 0; j < n; j++){
		tabrec[j] = mat.recurrent(j);
		if (tabrec[j]) result.col_ones[j] = mat.col_ones[j];
		else result.col_ones[j] = Matrix::zero_vector;

		result.col_pluses[j] = mat.col_pluses[j];
	}
	for (uint i = 0; i < n; i++){
		result.row_pluses[i] = mat.row_pluses[i];
		result.row_ones[i] = purge(mat.row_ones[i], tabrec);
	}
#else
Matrix * ProbMatrix::stab() const
{
	const Vector * recs = recurrent_states();
	uint n = Vector::GetStateNb();
	ProbMatrix * result = new ProbMatrix(n);

	for (uint i = 0; i < n; i++)
	{
		result->row_ones[i] = purge(row_ones[i], recs);
		result->row_pluses[i] = row_pluses[i];
	}

	for (uint j = 0; j < n; j++)
	{
		if (recs->contains(j))
			result->col_ones[j] = col_ones[j];
		else
			result->col_ones[j] = Matrix::zero_vector;
		result->col_pluses[j] = col_pluses[j];
	}

#endif
	result->update_hash();
	return result;
}

bool ProbMatrix::isIdempotent() const
{
	return (*this == *(ProbMatrix *)(this->ProbMatrix::prod(this)));
}
