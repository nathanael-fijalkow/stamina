#include "OneCounterMatrix.hpp"

// Class OneCounterMatrix
void OneCounterMatrix::allocate()
{
	rows = (const Vector ***)malloc(4 * sizeof(const Vector **));
	cols = (const Vector ***)malloc(4 * sizeof(const Vector **));
	for (char act = 0; act<4; act++){
		rows[act] = (const Vector **)malloc(stateNb * sizeof(const Vector *));
		cols[act] = (const Vector **)malloc(stateNb * sizeof(const Vector *));
	}
};


OneCounterMatrix::OneCounterMatrix(int stateNb) : Matrix(stateNb)
{
	allocate();
	update_hash();
}

//Constructor from Explicit Matrix
OneCounterMatrix::OneCounterMatrix(const ExplicitMatrix & explMatrix) : Matrix(explMatrix.stateNb)
{
	//CAUTION: Sparse_Matrix not defined
	allocate();
	for (char act = 0; act<4; act++){
		for (uint i = 0; i < stateNb; i++)
		{
			vector<bool> row(stateNb);
			vector<bool> col(stateNb);

			for (uint j = 0; j < stateNb; j++)
			{
				char c1 = explMatrix.coefficients[i * stateNb + j];
				char c2 = explMatrix.coefficients[j * stateNb + i];


				row[j] = (c1 <= act);
				col[j] = (c2 <= act);


			}

			unordered_set<Vector>::iterator it;
			//vector<bool> a la place de Vector ?
			it = vectors.insert(row).first;
			rows[act][i] = &(*it);
			it = vectors.insert(col).first;
			cols[act][i] = &(*it);
		}
	}

	update_hash();
}

//Print OneCounterMatrix
void OneCounterMatrix::print(std::ostream & os) const
{
	// CAUTION: sparse matrix not implemented
	string actions = "REIO";
	//cout << "Row description " << endl;
	for (uint i = 0; i < stateNb; i++){

		os << i << ":" << " ";

		for (uint j = 0; j < stateNb; j++){
			//find the char to print as the minimal one
			bool search = true;
			for (char act = 0; act<4; act++){
				const Vector & row = *rows[act][i];
				if (row.contains(j)){
					os << actions[act]; //print the first (best) action found
					search = false;
					break;
				}
			}
			if (search) os << '_';

		}
		os << endl;
	}
}

bool OneCounterMatrix::operator==(const OneCounterMatrix & mat) const
{
	//only on rows
	if (mat.stateNb != stateNb) return false;
	if (mat._hash != _hash) return false;

	for (char act = 0; act<4; act++){
		const Vector ** rows1 = rows[act];
		const Vector ** rows2 = mat.rows[act];
		for (; rows1 != rows[act] + stateNb; rows1++, rows2++)
		{
			if (*rows1 != *rows2) return false;
		}
	}
	return true;
};


Matrix * OneCounterMatrix::prod(const Matrix * pmat1) const
{
	const OneCounterMatrix & mat1 = *(OneCounterMatrix *)pmat1;
	const OneCounterMatrix & mat2 = *this;
	cout << "\nPROD\n";
	mat1.print();
	cout << "\n";
	mat2.print();

	uint n = mat1.stateNb;
	OneCounterMatrix * result = new OneCounterMatrix(n);


	for (uint i = 0; i < n; i++)//special case for the reset: reset on one side and increment on the other is enough
	{
	result->rows[RESET][i] = sub_prod2(mat1.rows[RESET][i], mat2.cols[INC],mat1.rows[INC][i], mat2.cols[RESET], n);
	result->cols[RESET][i] = sub_prod2(mat2.cols[INC][i], mat1.rows[RESET],mat2.cols[INC][i], mat1.rows[RESET], n);
	}
	for (char act = EPS; act<4; act++){
		for (uint i = 0; i < n; i++)
		{
			result->rows[act][i] = sub_prod(mat1.rows[act][i], mat2.cols[act], n);
			result->cols[act][i] = sub_prod(mat2.cols[act][i], mat1.rows[act], n);
		}
	}
	result->update_hash();
	cout << "\n Result: \n";
	result->print();
//	system("pause");
	return result;
}

//works only on idempotents
Matrix * OneCounterMatrix::stab() const
{
	cout << "\nSTAB\n";
	print();
	uint n = stateNb;
	OneCounterMatrix * result = new OneCounterMatrix(n);

	uint * diags[4]; //sharp of the diagonal, for now on one uint


	//a peaufiner, pour l'instant 1 a priori, on ne prend que celui du premier vecteur.
	uint bitsN = rows[0][0]->bitsNb;
	uint *new_row = (uint *)malloc(bitsN * sizeof(uint));
	uint *new_col = (uint *)malloc(bitsN * sizeof(uint));

	for (char act = 0; act<4; act++){
		if(act==INC) continue;
		diags[act]=(uint *)malloc(bitsN * sizeof(uint));
		for (int b = 0; b<bitsN; b++){ //initialisation de la diagonale
			diags[act][b] = 0;
		}
		//compute the diagonal 
		//cout << " act:" << (int)act << "\n";
		for (int i = 0; i <n; i++)
		{
			bool d = rows[act][i]->contains(i);
			cout << d;
			if (d) diags[act][i / (8 * sizeof(uint))] |= (1 << (i % (sizeof(uint) * 8)));

		}
	}
	//system("pause");
	diags[INC] = diags[EPS]; //IC impossible, restriction to E

	bool t = 0;//temporary result for coef i,j

	for (char act = 0; act<4; act++){
		for (uint i = 0; i <n; i++){
			memset(new_row, 0, bitsN*sizeof(uint));
			memset(new_col, 0, bitsN*sizeof(uint));
			for (uint j = 0; j<n; j++){

				//look for a possible path
				for (int b = 0; b<bitsN; b++){ t = t || ((rows[act][i]->bits[b] & diags[act][b] & cols[act][j]->bits[b]) != 0); }
				new_row[j / (8 * sizeof(uint))] |= (t ? 1 : 0) << (j % (sizeof(uint) * 8));
				for (int b = 0; b<bitsN; b++){ t = t || ((rows[act][j]->bits[b] & diags[act][b] & cols[act][i]->bits[b]) != 0); }
				new_col[j / (8 * sizeof(uint))] |= (t ? 1 : 0) << (j % (sizeof(uint) * 8));

			}

			auto it = vectors.emplace(new_row, n).first;
			result->rows[act][i] = &(*it);
			it = vectors.emplace(new_col, n).first;
			result->cols[act][i] = &(*it);

		}
	}

	free(new_row);
	free(new_col);

	free(diags[RESET]);free(diags[EPS]);free(diags[OM]);

	result->update_hash();
	cout << "\nStabResult:\n";
	result->print();
	return result;
}

bool OneCounterMatrix::isIdempotent() const
{
	return (*this == *(OneCounterMatrix *)(this->OneCounterMatrix::prod(this)));
};
