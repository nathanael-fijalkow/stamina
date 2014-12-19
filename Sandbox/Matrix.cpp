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


// Class OneCounterMatrix
void OneCounterMatrix::allocate()
{
	rows=(const Vector ***)malloc(4*sizeof(const Vector **));
	cols=(const Vector ***)malloc(4*sizeof(const Vector **));
	for(char act=0;act<4;act++){
	rows[act] = (const Vector **) malloc(stateNb * sizeof(const Vector *));
	cols[act] = (const Vector **) malloc(stateNb * sizeof(const Vector *));
	}
};

//Constructor from Explicit Matrix
OneCounterMatrix::OneCounterMatrix(const ExplicitMatrix & explMatrix) : Matrix(explMatrix.stateNb)
{
	//CAUTION: Sparse_Matrix not defined
	allocate();
	for (char act=0;act<4;act++){
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
void OneCounterMatrix::print(ostream & os) const 
{
	// CAUTION: sparse matrix not implemented
	string actions="REIO";
	//cout << "Row description " << endl;
	for (uint i = 0; i < stateNb; i++){
	
		os << i << ":" << " ";
		
		for(uint j = 0; j < stateNb; j++){
		//find the char to print as the minimal one
			bool search=true;
			for(char act=0;act<4;act++){
				const Vector & row = *rows[act][i];
				if(row.contains(j)){
						os << actions[act]; //print the first (best) action found
						search=false;
						break;
				}
			}
			if(search) os<<'_';

		}
		os << endl;
	}
}

bool OneCounterMatrix::operator==(const OneCounterMatrix & mat) const
{
	//only on rows
	if (mat.stateNb != stateNb) return false;
	if (mat._hash != _hash) return false;

	for(char act=0;act<4;act++){
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
	cout<<"\nPROD\n";
	mat1.print();
	cout<<"\n";
	mat2.print();

	uint n = mat1.stateNb;
	OneCounterMatrix * result = new OneCounterMatrix(n);
	for (char act=0;act<4;act++){
	for (uint i = 0; i < n; i++)
	{
		result->rows[act][i] = sub_prod( mat1.rows[act][i], mat2.cols[act], n);
		result->cols[act][i] = sub_prod( mat2.cols[act][i], mat1.rows[act], n);
	}
	}
	result->update_hash();
	cout<<"\n Result: \n";
	result->print();
	system("pause");
	return result;
}

//works only on idempotents
Matrix * OneCounterMatrix::stab() const
{
	cout <<"\nSTAB\n";
	print();
	uint n = stateNb;
	OneCounterMatrix * result = new OneCounterMatrix(n);

	uint * diags[4]; //sharp of the diagonal, for now on one uint
	
	
	//a peaufiner, pour l'instant 1 a priori, on ne prend que celui du premier vecteur.
	uint bitsN=rows[0][0]->bitsNb;

for(char act=0;act<4;act++){
		uint *diag=(uint *) malloc(bitsN * sizeof(uint));;
	for(int b=0;b<bitsN;b++){ //initialisation de la diagonale
			diag[b]=0; 
		}
		//compute the diagonal 
	cout<<" act:"<<(int)act<<"\n";
	for (int i = 0; i <n ; i++)
	{
		bool d=rows[act][i]->contains(i);
		cout<<d;
		if (d) diag[i / (8 * sizeof(uint))] = (diag[i / (8 * sizeof(uint))]) | ( 1 << (i % (sizeof(uint) * 8))) ;
		
	}
	cout<<"\n";
	diags[act]=diag;

	
	for(uint i=0;i<n;i++){//initialize vectors to 0
		result->rows[act][i]=zero_vector;
		result->cols[act][i]=zero_vector;
	}
}
diags[INC]=diags[EPS]; //IC impossible, restriction to E

bool t=0;//temporary result for coef i,j

for(char act=0;act<4;act++){
	for (uint i =0; i <n; i++){
	for(uint j=0; j<n;j++){
		
		//look for a possible path
		for(int b=0;b<bitsN;b++){t = t || ((rows[act][i]->bits[b] & diags[act][b] & cols[act][j]->bits[b])!=0);}
			//cout << "success: " << i<< " "<<j <<"\n";
		if(t) { //put 1 in result(i,j)
			//a verifier
			result->rows[act][i]->bits[j / (8 * sizeof(uint))] = result->rows[act][i]->bits[j / (8 * sizeof(uint))] | ( 1 << (j % (sizeof(uint) * 8)));
			result->cols[act][j]->bits[i / (8 * sizeof(uint))] = result->cols[act][j]->bits[i / (8 * sizeof(uint))] | ( 1 << (i % (sizeof(uint) * 8)));
		}
			
		
	}
	}
}

	result->update_hash();
	cout<<"\nStabResult:\n";
	result->print();
	return result;
}

bool OneCounterMatrix::isIdempotent() const
{
	return (*this == *(OneCounterMatrix *)(this->OneCounterMatrix::prod(this)));
};
