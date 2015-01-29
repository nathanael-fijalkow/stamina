#include "MultiCounterMatrix.hpp"

//Constructor
MultiCounterMatrix::MultiCounterMatrix()
{
	// Not implemented
}

//Constructor from Explicit Matrix
MultiCounterMatrix::MultiCounterMatrix(const ExplicitMatrix & explMatrix)
{
	Nb_state = VectorInt::GetStateNb();

	for (uint i = 0; i < Nb_state; i++){
		for (uint j = 0; j < Nb_state; j++){
			mat_prod[i][j] =
				if (i == 2*N+2 | j == 2*N+2) { 2*N+2 ; }
				else if (i == 2*N+1 | j == 2*N+1) { 2*N+1 ; }
				else if ((i <= N & j <= N) | (N < i & N < j)) { min(i,j) ; }
				else if (i <= N & N < j & i <= j-N) { i ; }
				else if (i <= N & N < j & i > j-N) { j ; }
				else if (j <= N & N < i & j <= i-N) { j ; }
				else if (j <= N & N < i & j > i-N) { i ; }
		}
	}
	
	for (uint i = 0; i < Nb_state; i++)
	{
		vector<int> row(Nb_state);
		vector<int> col(Nb_state);

		for (uint j = 0; j < Nb_state; j++)
		{
			row[j] = explMatrix.coefficients[i * Nb_state + j];
			col[j] = explMatrix.coefficients[j * Nb_state + i];
		}
		unordered_set<VectorInt>::iterator it;
		it = vectors.insert(row).first;
		rows[i] = &(*it);
		it = vectors.insert(col).first;
		cols[i] = &(*it);
	}
	update_hash();
}

//Print MultiCounterMatrix
void MultiCounterMatrix::print(std::ostream & os) const
{
	for (uint i = 0; i < Nb_state i++){
		os << i << ":" << " ";
		for (uint j = 0; j < Nb_state j++)
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
	for (; rows1 != rows + Nb_state; rows1++, rows2++)
	{
		if (*rows1 != *rows2) return false;
	}
	return true;
};

Matrix * MultiCounterMatrix::prod(const Matrix * pmat1) const
{
	const MultiCounterMatrix & mat1 = *this;
	const MultiCounterMatrix & mat2 = *(MultiCounterMatrix *)pmat1;

	MultiCounterMatrix * result = new MultiCounterMatrix();

 	for (uint i = 0; i < Nb_state; i++){
		result->rows[i] = sub_prod(mat1.rows[i], mat2.cols);
		result->cols[i] = sub_prod(mat2.cols[i], mat1.rows);
	}

	result->update_hash();
	return result;
}

// Construct a vector obtained by multiplying the line vec by all columns of mat.
const VectorInt * MultiCounterMatrix::sub_prod(const VectorInt * vec, const VectorInt ** mat_cols)
{
	size_t * new_vec = (size_t *)malloc(VectorInt::GetBitSize() * sizeof(size_t));
	memset(new_vec, 0, (size_t)(VectorInt::GetBitSize() * sizeof(size_t)));

	for (int j = VectorInt::GetStateNb() - 1; j >= 0; j--)
	{		
		int min_curr = 2*N+2;
		for (uint i = 0; i < VectorInt::GetBitSize(); i++)
		{
			min_curr = min min_curr mat_prod[vec[i]][mat_cols[j][i]];
		}
		new_vec[j] = min_curr;
	}

	auto it = vectors.emplace(new_vec).first;
	free(new_vec);
	return &(*it);
}

bool MultiCounterMatrix::isIdempotent() const
{
	return (*this == *(MultiCounterMatrix *)(this->MultiCounterMatrix::prod(this)));
};

//works only on idempotents
Matrix * MultiCounterMatrix::stab() const
{
	uint n = Nb_state
	MultiCounterMatrix * result = new MultiCounterMatrix();

	uint * diags[2*N+2]; //sharp of the diagonal, for now on one uint

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
	return result;
}
