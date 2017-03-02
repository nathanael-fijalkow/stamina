#include "MultiCounterMatrix.hpp"
#include <stdlib.h>

//Constructor
void MultiCounterMatrix::init()
{
	rows = (const VectorInt **)malloc(VectorInt::GetStateNb() * sizeof(VectorInt*));
	cols = (const VectorInt **)malloc(VectorInt::GetStateNb() * sizeof(VectorInt*));
}

MultiCounterMatrix::MultiCounterMatrix()
{
	init();
}

const MultiCounterMatrix * MultiCounterMatrix::operator*(const MultiCounterMatrix & other) const
{
    return (const MultiCounterMatrix *) prod(&other);
}

void MultiCounterMatrix::set_counter_and_states_number(char N, uint NbSt)
{
    
    if(MultiCounterMatrix::N == N && VectorInt::GetStateNb() == NbSt && act_prod)
        return;

    VectorInt::SetSize(NbSt);
    
    MultiCounterMatrix::N = N;

    if(act_prod) {
        free(act_prod);
        act_prod = NULL;
    }
    
	act_prod = (unsigned char **) malloc((2 * N + 3)  *  sizeof(char*));
	for (uint i = 0; i < (2 * N + 3); i++){
		act_prod[i] = (unsigned char *)malloc((2 * N + 3)  *  sizeof(char));
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

/*	for (uint i = 0; i < (2 * N + 3); i++){
		for (uint j = 0; j < (2 * N + 3); j++){
			cout << (int)  act_prod[i][j] << " ";
		}
		cout << endl;
	}
	*/


}

MultiCounterMatrix::MultiCounterMatrix(const MultiCounterMatrix *mat){
	init();
	for(int i=0;i<VectorInt::GetStateNb();i++){
		rows[i]=mat->rows[i];
		cols[i]=mat->cols[i];
	}
	update_hash();
}

//Constructor from Explicit Matrix
MultiCounterMatrix::MultiCounterMatrix(const ExplicitMatrix & explMatrix, char N)
{
	init();

    if(VectorInt::GetStateNb() != explMatrix.stateNb)
        throw runtime_error("States number mismatch please call set_counter_and_states_number first");
    
	this->N = N;
	for (uint i = 0; i < VectorInt::GetStateNb(); i++)
	{
		vector<char> row(VectorInt::GetStateNb());
		vector<char> col(VectorInt::GetStateNb());

		for (uint j = 0; j < VectorInt::GetStateNb(); j++)
		{
			row[j] = explMatrix.coefficients[i][j];
			col[j] = explMatrix.coefficients[j][i];
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

unsigned char ** MultiCounterMatrix::act_prod = NULL;

// This is the constant vector with only zero entries
const VectorInt * MultiCounterMatrix::zero_int_vector = NULL;

int MultiCounterMatrix::get(int i, int j) const
{
        return (rows[i]->coefs[j]);
}

//Print MultiCounterMatrix
void MultiCounterMatrix::print(std::ostream & os, vector<string> state_names) const
{
	for (uint i = 0; i < VectorInt::GetStateNb(); i++){
		os << ( state_names.size() > i ? state_names[i] : to_string(i)) << ":" << " ";
		for (uint j = 0; j < VectorInt::GetStateNb(); j++)
		{
//			os << " " << (int)((rows[i]->coefs[j] == 6) ? 6 : rows[i]->coefs[j]);
				string result = "";
				auto element = rows[i]->coefs[j];
				if (element == 2 * N + 2) result = "_ ";
				else if (element == 2 * N + 1) result = "O ";
				else if (element == N) result = "E ";
				else if (element < N) result = "r" + to_string(element);
				else result = "i" + to_string(element - N - 1);
				if (result.size() <= 1) result.push_back(' ');
				os << result << " ";
		}
		os << endl;
	}
}

//Print MultiCounterMatrix according to columns (transpose the matrix)
void MultiCounterMatrix::print_col(std::ostream & os, vector<string> state_names) const
{
	for (uint i = 0; i < VectorInt::GetStateNb(); i++){
		os << ( state_names.size() > i ? state_names[i] : to_string(i)) << ":" << " ";
		for (uint j = 0; j < VectorInt::GetStateNb(); j++)
		{
//			os << " " << (int)((rows[i]->coefs[j] == 6) ? 6 : rows[i]->coefs[j]);
				string result = "";
				auto element = cols[i]->coefs[j];
				if (element == 2 * N + 2) result = "_ ";
				else if (element == 2 * N + 1) result = "O ";
				else if (element == N) result = "E ";
				else if (element < N) result = "r" + to_string(element);
				else result = "i" + to_string(element - N - 1);
				if (result.size() <= 1) result.push_back(' ');
				os << result << " ";
		}
		os << endl;
	}
}


ExplicitMatrix* MultiCounterMatrix::toExplicitMatrix() const
{
    print();
        ExplicitMatrix* ret = new ExplicitMatrix(VectorInt::GetStateNb());
        for (uint i = 0; i < VectorInt::GetStateNb(); i++){
            for (uint j = 0; j < VectorInt::GetStateNb(); j++)
                ret->coefficients[i][j]=rows[i]->coefs[j];
	}
	return ret;
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
	unsigned char * new_vec = (unsigned char *)malloc(VectorInt::GetStateNb() * sizeof(char));
	//memset(new_vec, 0, (VectorInt::GetStateNb() * sizeof(char)));

	for (int j = VectorInt::GetStateNb() - 1; j >= 0; j--)
	{
		unsigned char min_curr = 2 * N + 2;
        for (uint i = 0; i < VectorInt::GetStateNb(); i++) {
            auto a = vec->coefs[i];
            auto b = mat_cols[j]->coefs[i];
			min_curr = min(min_curr, act_prod[ a ][ b ] );
        }
		new_vec[j] = min_curr;
	}

	auto it = int_vectors.emplace(new_vec).first;
	free(new_vec);
	return &(*it);
}

const MultiCounterMatrix * MultiCounterMatrix::prod(const Matrix * pmat1) const
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

bool MultiCounterMatrix::isUnlimitedWitness(const vector<int> & initial_states, const vector<int> & final_states) const
{
	bool found = false;
	for (auto ini : initial_states)
	{
		auto row = rows[ini];
		for (auto fin : final_states)
		{
			if (row->coefs[fin] < 2 * N + 1)
				return false;
			if (row->coefs[fin] == 2 * N + 1)
				found = true;
		}
	}
	//if(found) print();
	return found;
}

const MultiCounterMatrix * MultiCounterMatrix::stab() const
{
	//start by reaching idempotent power.
	
	MultiCounterMatrix *emat=new MultiCounterMatrix(this);
	
	while(! emat->isIdempotent()){
		emat = (MultiCounterMatrix *)emat->prod(this);
	}
	
	uint n = VectorInt::GetStateNb();
	MultiCounterMatrix * result = new MultiCounterMatrix();
	
	
	unsigned char * diags; //sharp of the diagonal
	diags=(unsigned char *)malloc(n*sizeof(char));

	unsigned char *new_row = (unsigned char *)malloc(n * sizeof(char));
	unsigned char *new_col = (unsigned char *)malloc(n * sizeof(char));

	unsigned char cd;
	//compute the diagonal 
	//cout << " act:" << (int)act << "\n";
	for (uint i = 0; i <n; i++){
			cd=emat->rows[i]->coefs[i];
			diags[i] = (cd <= N) ? cd :((cd <= 2*N+1) ? 2*N+1 : 2*N+2);
	}
	//system("pause");

		for (uint i = 0; i <n; i++){
			memset(new_row, 2*N+2, n*sizeof(char));
			memset(new_col, 2*N+2, n*sizeof(char));
			for (uint j = 0; j<n; j++){
				//look for a possible path
				unsigned char t=2*N+2;
				for (uint b = 0; b<n; b++){ 
					 t = min(t, act_prod[emat->rows[i]->coefs[b]][act_prod[diags[b]][emat->cols[j]->coefs[b]]]); 
					 }
				new_row[j] = t;
				
				t=2*N+2;
				for (uint b = 0; b<n; b++){ 
					t = min(t, act_prod[emat->rows[j]->coefs[b]][act_prod[diags[b]][emat->cols[i]->coefs[b]]]);
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
	free(emat);
	result->update_hash();
	
	return result;
}
