#include "MultiCounterMatrix.hpp"
#include <stdlib.h>

//Constructor
MultiCounterMatrix::MultiCounterMatrix()
{
	// Not implemented
	rows = (const VectorInt **)malloc(VectorInt::GetStateNb() * sizeof(VectorInt*));
	cols = (const VectorInt **)malloc(VectorInt::GetStateNb() * sizeof(VectorInt*));
}

void MultiCounterMatrix::init_act_prod(char N)
{
	this->N = N;
	act_prod = (char **)malloc((2 * N + 2)  *  sizeof(char*));
	for (uint i = 0; i < VectorInt::GetStateNb(); i++){
		act_prod[i] = (char *)malloc((2 * N + 2)  *  sizeof(char));
		for (uint j = 0; j < VectorInt::GetStateNb(); j++){
			act_prod[i][j] =
				(i == 2 * N + 2 | j == 2 * N + 2) ? 2 * N + 2
				: (i == 2 * N + 1 | j == 2 * N + 1) ? 2 * N + 1
				: ((i <= N & j <= N) | (N < i & N < j)) ? ( i< j ? i : j)
				: (i <= N & N < j & i <= j - N) ? i
				: (i <= N & N < j & i > j - N) ? j
				: (j <= N & N < i & j <= i - N) ? j
				: /*(j <= N & N < i & j > i - N) ?*/ i;
		}
	}
}

//Constructor from Explicit Matrix
MultiCounterMatrix::MultiCounterMatrix(const ExplicitMatrix & explMatrix)
{
	for (uint i = 0; i < VectorInt::GetStateNb(); i++)
	{
		vector<char> row(VectorInt::GetStateNb());
		vector<char> col(VectorInt::GetStateNb());

		for (uint j = 0; j < VectorInt::GetStateNb(); j++)
		{
			row[j] = explMatrix.coefficients[i * VectorInt::GetStateNb() + j];
			col[j] = explMatrix.coefficients[j * VectorInt::GetStateNb() + i];
		}
		unordered_set<VectorInt>::iterator it;
		it = int_vectors.insert(row).first;
		rows[i] = &(*it);
		it = int_vectors.insert(col).first;
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
		// WORK HERE
			os << " ";
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

	/*
	char * diags[2*N+2]; //sharp of the diagonal, for now on one uint

	//a peaufiner, pour l'instant 1 a priori, on ne prend que celui du premier vecteur.
	uint bitsN = VectorInt::GetBitSize();
	size_t *new_row = (size_t *)malloc(bitsN * sizeof(size_t));
	size_t *new_col = (size_t *)malloc(bitsN * sizeof(size_t));

	for (char act = 0; act<2*N+2; act++){
		if(act==INC) continue;
		diags[act]=(uint *)malloc(bitsN * sizeof(size_t));
		for (uint b = 0; b<bitsN; b++){ //initialisation de la diagonale
			diags[act][b] = 0;
		}
		//compute the diagonal 
		//cout << " act:" << (int)act << "\n";
		for (uint i = 0; i <n; i++)
			if (rows[act][i]->contains(i))
				diags[act][i / (8 * sizeof(size_t))] |= (1 << (i % (sizeof(size_t) * 8)));
	}
	//system("pause");
	diags[INC] = diags[EPS]; //IC impossible, restriction to E


	for (char act = 0; act<2*N+2; act++){
		for (uint i = 0; i <n; i++){
			memset(new_row, 0, bitsN*sizeof(size_t));
			memset(new_col, 0, bitsN*sizeof(size_t));
			for (uint j = 0; j<n; j++){
				bool t = false;//temporary result for coef i,j

				//look for a possible path
				for (uint b = 0; b<bitsN; b++){ t = t || ((rows[act][i]->bits[b] & diags[act][b] & cols[act][j]->bits[b]) != 0); }
				new_row[j / (8 * sizeof(size_t))] |= (t ? 1 : 0) << (j % (sizeof(size_t) * 8));

				t = false;
				for (uint b = 0; b<bitsN; b++){ t = t || ((rows[act][j]->bits[b] & diags[act][b] & cols[act][i]->bits[b]) != 0); }
				new_col[j / (8 * sizeof(size_t))] |= (t ? 1 : 0) << (j % (sizeof(size_t) * 8));

			}

			auto it = vectors.emplace(new_row).first;
			result->rows[act][i] = &(*it);
			it = vectors.emplace(new_col).first;
			result->cols[act][i] = &(*it);

		}
	}

	free(new_row);
	free(new_col);

	free(diags[RESET]);
	free(diags[EPS]);
	free(diags[OM]);

	result->update_hash();
	*/
	return result;
}
