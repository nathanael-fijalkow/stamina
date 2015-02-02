#include "MultiCounterMatrix.hpp"
#include <stdlib.h>

//Constructor
MultiCounterMatrix::MultiCounterMatrix()
{
	rows = (const VectorInt **)malloc(VectorInt::GetStateNb() * sizeof(VectorInt*));
	cols = (const VectorInt **)malloc(VectorInt::GetStateNb() * sizeof(VectorInt*));
}

void MultiCounterMatrix::init_act_prod(char N)
{
	MultiCounterMatrix::N = N;
	act_prod = (char **)malloc((2 * N + 3)  *  sizeof(char*));
	for (uint i = 0; i < (2 * N + 3); i++){
		act_prod[i] = (char *)malloc((2 * N + 3)  *  sizeof(char));
		for (uint j = 0; j < (2 * N + 3); j++){
			act_prod[i][j] =
				(i == 2 * N + 2 || j == 2 * N + 2) ? 2 * N + 2
				: (i == 2 * N + 1 || j == 2 * N + 1) ? 2 * N + 1
				: ((i <= N & j <= N) || (N < i & N < j)) ? ( i< j ? i : j)
				: (i <= N & N < j & i < j - N) ? i
				: (i <= N & N < j & i >= j - N) ? j
				: (j <= N & N < i & j < i - N) ? j
				: /*(j <= N & N < i & j > i - N) ?*/ i;
		}
	}
	for (uint i = 0; i < (2 * N + 3); i++){
		for (uint j = 0; j < (2 * N + 3); j++){
			cout << (int)  act_prod[i][j] << " ";
		}
		cout << endl;
	}


}

//Constructor from Explicit Matrix
MultiCounterMatrix::MultiCounterMatrix(const ExplicitMatrix & explMatrix, char N) : MultiCounterMatrix()
{
	VectorInt::SetSize(explMatrix.stateNb);
	this->N = N;
	for (uint i = 0; i < VectorInt::GetStateNb(); i++)
	{
		vector<char> row(VectorInt::GetStateNb());
		vector<char> col(VectorInt::GetStateNb());

		for (uint j = 0; j < VectorInt::GetStateNb(); j++)
		{
			row[j] = explMatrix.coefficients[i * VectorInt::GetStateNb() + j];
			col[j] = explMatrix.coefficients[j * VectorInt::GetStateNb() + i];
		}
		auto it1 = int_vectors.emplace(row);
		auto it = it1.first;
		rows[i] = &(*it);
		it = int_vectors.emplace(col).first;
		cols[i] = &(*it);
	}
	update_hash();
}

char MultiCounterMatrix::N = 0;

char ** MultiCounterMatrix::act_prod = NULL;

/* the set of all vectors */
std::unordered_set<VectorInt> MultiCounterMatrix::int_vectors;

// This is the constant vector with only zero entries
const VectorInt * MultiCounterMatrix::zero_int_vector = NULL;


//Print MultiCounterMatrix
void MultiCounterMatrix::print(std::ostream & os) const
{
	for (uint i = 0; i < VectorInt::GetStateNb(); i++){
		os << i << ":" << " ";
		for (uint j = 0; j < VectorInt::GetStateNb(); j++)
		{

			os << " " << (int)((rows[i]->coefs[j] == 6) ? 6 : rows[i]->coefs[j]);
		}
		os << endl;
	}
}

bool MultiCounterMatrix::operator==(const MultiCounterMatrix & mat) const
{
	//only on rows
	if (mat._hash != _hash) return false;

	const VectorInt ** rows1 = rows;
	const VectorInt ** rows2 = mat.rows;
	for (; rows1 != rows + VectorInt::GetStateNb(); rows1++, rows2++)
		if (*rows1 != *rows2) return false;
	return true;
};

const VectorInt * MultiCounterMatrix::sub_prod_int(const VectorInt * vec, const VectorInt ** mat_cols)
{
	char * new_vec = (char *)malloc(VectorInt::GetStateNb() * sizeof(char));
	memset(new_vec, 0, (char)(VectorInt::GetStateNb() * sizeof(char)));

	for (int j = VectorInt::GetStateNb() - 1; j >= 0; j--)
	{
		char min_curr = 2 * N + 2;
		for (uint i = 0; i < VectorInt::GetStateNb(); i++)
			min_curr = MIN_ACME(min_curr, act_prod[ vec->coefs[i] ][ mat_cols[j]->coefs[i] ] );
		new_vec[j] = min_curr;
	}

	auto it = int_vectors.emplace(new_vec).first;
	free(new_vec);
	return &(*it);
}

Matrix * MultiCounterMatrix::prod(const Matrix * pmat1) const
{
	const MultiCounterMatrix & mat1 = *this;
	const MultiCounterMatrix & mat2 = *(MultiCounterMatrix *)pmat1;

	MultiCounterMatrix * result = new MultiCounterMatrix();

	for (uint i = 0; i < VectorInt::GetStateNb(); i++){
		result->rows[i] = sub_prod_int(mat1.rows[i], mat2.cols);
		result->cols[i] = sub_prod_int(mat2.cols[i], mat1.rows);
	}
	result->update_hash();
	return result;
}

// Construct a vector obtained by multiplying the line vec by all columns of mat.

bool MultiCounterMatrix::isIdempotent() const
{
	return (*this == *(MultiCounterMatrix *)(this->MultiCounterMatrix::prod(this)));
};

//works only on idempotents
Matrix * MultiCounterMatrix::stab() const
{
	uint n = VectorInt::GetStateNb();
	MultiCounterMatrix * result = new MultiCounterMatrix();

	
	char * diags; //sharp of the diagonal
	diags=(char *)malloc(n*sizeof(char));

	char *new_row = (char *)malloc(n * sizeof(char));
	char *new_col = (char *)malloc(n * sizeof(char));

	char cd;
	//compute the diagonal 
	//cout << " act:" << (int)act << "\n";
	for (uint i = 0; i <n; i++){
			cd=rows[i]->coefs[i];
			diags[i] = (cd <= n) ? cd :((cd <= 2*N+1) ? 2*N+1 : 2*N+2);
			
	}
	//system("pause");

		for (uint i = 0; i <n; i++){
			memset(new_row, 2*N+2, n*sizeof(char));
			memset(new_col, 2*N+2, n*sizeof(char));
			for (uint j = 0; j<n; j++){
				//look for a possible path
				char t=2*N+2;
				for (uint b = 0; b<n; b++){ 
					 t = MIN_ACME(t, act_prod[rows[i]->coefs[b]][act_prod[diags[b]][cols[j]->coefs[b]]]); 
					 }
				new_row[j] = t;
				
				t=2*N+2;
				for (uint b = 0; b<n; b++){ 
					t = MIN_ACME(t, act_prod[rows[j]->coefs[b]][act_prod[diags[b]][cols[i]->coefs[b]]]);
					 }
				new_col[j] = t;
			}

			auto it = int_vectors.emplace(new_row).first;
			result->rows[i] = &(*it);
			it = int_vectors.emplace(new_col).first;
			result->cols[i] = &(*it);

		}

	free(new_row);
	free(new_col);

	free(diags);
	
	result->update_hash();
	
	return result;
}
