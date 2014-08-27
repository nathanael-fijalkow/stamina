#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <functional>
#include <unordered_set>

#include "AcmeTypes.hpp"


SharpedExpr::SharpedExpr(ExtendedExpression * son) : son(son)
{
	_hash = hash_value(son->Hash());
}

//constructor
ConcatExpr::ConcatExpr(const LetterExpr * expr, uint maxSonsNb)
{
	sons = (const ExtendedExpression **)malloc(maxSonsNb * sizeof(void *));

	sons[0] = expr;
	sonsNb = 1;

	update_hash();
};

ConcatExpr::ConcatExpr(const ConcatExpr & other) : sonsNb(other.sonsNb)
{
	_hash = other._hash;
	sons = (const ExtendedExpression **) malloc( sonsNb * sizeof(void *));
	memcpy(sons,other.sons,sonsNb * sizeof(void *));
}

ConcatExpr::~ConcatExpr()
{
	free(sons);
	sons = NULL;
}

void ConcatExpr::addLeftSon(const ExtendedExpression * new_son)
{
	*(sons + sonsNb) = new_son;
	sonsNb++;
	_hash ^= hash_value(new_son->Hash()) + 0x9e3779b9 + (_hash << 6) + (_hash >> 2);
}

LetterExpr::LetterExpr(char letter) : letter(letter)
{
	_hash = hash_value(letter);
};


bool ExtendedExpression::operator==(const ExtendedExpression & exp) const
{
	const ExtendedExpression * pexp = &exp;
	if (typeid(this) != typeid(pexp))
		return false;
	const SharpedExpr * sexpr = isSharpedExpr(this);
	if (sexpr != NULL)
		return (*sexpr == *(SharpedExpr *)pexp);
	else
	{
		const ConcatExpr * cexpr = isConcatExpr(this);
		if (cexpr != NULL)
			return (*cexpr == *(ConcatExpr *)pexp);
		else
			return *(LetterExpr *)cexpr == *(LetterExpr *)pexp;
	}
}

#if USE_SPARSE_VECTOR
Vector::Vector(uint size) : entriesNb(size)
{
	entries = (size_t *)malloc(entriesNb
		* sizeof(size_t));
}
#endif

#if USE_SPARSE_VECTOR
Vector::Vector(vector<size_t> data) : entriesNb(data.size())
{
	entries = (size_t *)malloc(entriesNb * sizeof(size_t));
	size_t * p = entries;
	for (vector<size_t>::iterator it = data.begin(); it != data.end(); it++)
		*p++ = *it;
	update_hash();
}
#endif

#if USE_SPARSE_VECTOR
Vector::Vector(size_t * data, size_t data_size) : entriesNb(data_size)
{
	entries = (size_t *)malloc(entriesNb * sizeof(size_t));
	memcpy(entries, data, entriesNb * sizeof(size_t));
	update_hash();
};
#endif

#if USE_SPARSE_VECTOR

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


#else
void Vector::print() const
{
	for (uint i = 0; i < entries.size(); i++)
		cout << entries[i] ? "1" : "_";
	cout << endl;
}
#endif

#if USE_SPARSE_VECTOR
Vector::~Vector()
{
	free(entries);
	entries = NULL;
}
#endif

#if USE_SPARSE_VECTOR
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
#endif


/* known vectors */
unordered_set<Vector> Matrix::vectors;
const Vector * Matrix::zero_vector = NULL;

UnstableMarkovMonoid::UnstableMarkovMonoid(uint dim) : dim(dim)
{
	Matrix::vectors.clear();

#if USE_SPARSE_VECTOR
//	Matrix::zero_vector = &*Matrix::vectors.emplace(0).first;
	Matrix::zero_vector = &*Matrix::vectors.insert(0).first;
#else
	Matrix::zero_vector = &*Matrix::vectors.emplace(dim).first;
#endif
};

/* Cleans up knowns vectors */
UnstableMarkovMonoid::~UnstableMarkovMonoid()
{
	Matrix::vectors.clear();
	Matrix::zero_vector = NULL;;
};


void Matrix::allocate()
{
	row_pluses = (const Vector **)malloc(stateNb * sizeof(void *));
	row_ones = (const Vector **)malloc(stateNb * sizeof(void *));
	col_pluses = (const Vector **)malloc(stateNb * sizeof(void *));
	col_ones = (const Vector **)malloc(stateNb * sizeof(void *));
}

Matrix::Matrix(uint stateNb) : _hash(0), stateNb(stateNb)
{
	allocate();
};

Matrix::Matrix(const ExplicitMatrix & explMatrix) : stateNb(explMatrix.stateNb)
{
	allocate();
	for (uint i = 0; i < stateNb; i++)
	{
#if USE_SPARSE_VECTOR
		vector<size_t> r_pluses;
		vector<size_t> r_ones;
		vector<size_t> c_pluses;
		vector<size_t> c_ones;
		for (uint j = 0; j < stateNb; j++)
		{
			char c1 = explMatrix.coefficients[i * stateNb + j];
			if (c1 >= 1) //modified: the + are now the union of 1 and 2.
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
		vector<bool> r_ones(stateNb);
		vector<bool> r_pluses(stateNb);
		vector<bool> c_ones(stateNb);
		vector<bool> c_pluses(stateNb);
		for (uint j = 0; j < stateNb; j++)
		{
			char c1 = explMatrix.coefficients[i * stateNb + j];
			r_pluses[j] = (c1 >= 1);
			r_ones[j] = (c1 == 2);

			char c2 = explMatrix.coefficients[j * stateNb + i];
			c_pluses[j] = (c2 >= 1);
			c_ones[j] = (c2 == 2);
		}
#endif
//		unordered_set<Vector>::iterator it = vectors.emplace(r_ones).first;
		unordered_set<Vector>::iterator it = vectors.insert(r_ones).first;
		row_ones[i] = &(*it);

//		it = vectors.emplace(r_pluses).first;
		it = vectors.insert(r_pluses).first;
		row_pluses[i] = &(*it);

//		it = vectors.emplace(c_ones).first;
		it = vectors.insert(c_ones).first;
		col_ones[i] = &(*it);

//		it = vectors.emplace(c_pluses).first;
		it = vectors.insert(c_pluses).first;
		col_pluses[i] = &(*it);
	}
	update_hash();
}

void Matrix::print() const
{
	for (uint i = 0; i < stateNb; i++)
	{
		cout << i << ":" << " ";
		const Vector & ones = *row_ones[i];
		const Vector & pluses = *row_pluses[i];

#if USE_SPARSE_VECTOR
		uint jones = 0, jpluses = 0;
		for(uint j = 0; j < stateNb; j++)
		{
			if (ones.entriesNb > jones && ones.entries[jones] == j)
			{
				cout << "1 ";
				jones++;
				jpluses++;//added
			}
			else if (pluses.entriesNb > jpluses && pluses.entries[jpluses] == j)
			{
				cout << "+ ";
				jpluses++;
			}
			else
				cout << "_ ";
		}
#else
		for (uint j = 0; j < stateNb; j++)
		{
			if (ones.entries[j])
				cout << "1 ";
			else if (pluses.entries[j])
				cout << "+ ";
			else
				cout << "_ ";
		}
#endif
		cout << endl;
	}
}

void Matrix::print_col() const
{
	for (uint i = 0; i < stateNb; i++)
	{
		cout << i << ":" << " ";
		const Vector & ones = *col_ones[i];
		const Vector & pluses = *col_pluses[i];

#if USE_SPARSE_VECTOR
		uint jones = 0, jpluses = 0;
		for(uint j = 0; j < stateNb; j++)
		{
			if (ones.entriesNb > jones && ones.entries[jones] == j)
			{
				cout << "1 ";
				jones++;
				jpluses++;//added
			}
			else if (pluses.entriesNb > jpluses && pluses.entries[jpluses] == j)
			{
				cout << "+ ";
				jpluses++;
			}
			else
				cout << "_ ";
		}
#else
		for (uint j = 0; j < stateNb; j++)
		{
			if (ones.entries[j])
				cout << "1 ";
			else if (pluses.entries[j])
				cout << "+ ";
			else
				cout << "_ ";
		}
#endif
		cout << endl;
	}
}



const SharpedExpr * isSharpedExpr(const ExtendedExpression * expr) { return dynamic_cast<const SharpedExpr *>(expr); }
const ConcatExpr * isConcatExpr(const ExtendedExpression * expr) { return dynamic_cast<const ConcatExpr *>(expr); }
const LetterExpr * isLetterExpr(const ExtendedExpression * expr){ return dynamic_cast<const LetterExpr *>(expr); }


const Vector * Matrix::sub_prod(
	const Vector * vec,
	const Vector ** mat,
	size_t stateNb
	)
{
	if (vec == Matrix::zero_vector)	return Matrix::zero_vector;

#if USE_SPARSE_VECTOR

	size_t * new_vec = (size_t *)malloc(stateNb * sizeof(size_t));
	size_t * new_vec_start = new_vec;

	for (uint j = 0; j < stateNb; j++)
	{
		if (mat[j] == Matrix::zero_vector) continue;

		const size_t * vec_entries = vec->entries;
		const size_t * vec_entries_end = vec->end();

		const size_t * mat_entries_j = mat[j]->entries;
		const size_t * mat_entries_j_end = mat[j]->end();
/*
		if (vec == Matrix::zero_vector)
			vec_entries = vec_entries_end;
		if (mat[j] == Matrix::zero_vector)
			mat_entries_j = mat_entries_j_end;
*/
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

#else

vector<bool> new_ones(stateNb);
vector<bool> new_pluses(stateNb);

	for (uint j = 0; j < stateNb; j++)
	{
		bool found_one = false, found_plus = false;

		const vector<bool> & ones1 = vecones1->entries;
		const vector<bool> &  pluses1 = vecplus1->entries;

		const vector<bool> &  ones2 = matones2[j]->entries;
		const vector<bool> &  pluses2 = matpluses2[j]->entries;

		vector<bool>::const_iterator it1 = ones1.begin();
		vector<bool>::const_iterator it2 = ones2.begin();
		vector<bool>::const_iterator end1 = ones1.end();

		vector<bool>::const_iterator it1p = pluses1.begin();
		vector<bool>::const_iterator it2p = pluses2.begin();

		for (; it1 != end1; it1++, it2++, it1p++, it2p++)
		{
			if (*it1 && *it2)
			{
				found_one = true;
				break;
			}
			else if (!found_plus && (*it1 || *it1p) && (*it2 || *it2p))
			{
				found_plus = true;
			}
		}

		new_ones[j] = found_one;
		new_pluses[j] = found_plus && !found_one;
	}

	unordered_set<Vector>::iterator it_one = vectors.emplace(new_ones).first;
	unordered_set<Vector>::iterator it_plus = vectors.emplace(new_pluses).first;

#endif

	return &(*it);
}

Matrix::Matrix(const Matrix & mat1, const Matrix & mat2) : stateNb(mat1.stateNb)
{
	allocate();

	for (uint i = 0; i < stateNb; i++)
	{
		row_ones[i] = sub_prod(mat1.row_ones[i],mat2.col_ones,stateNb);
		row_pluses[i] = sub_prod(mat1.row_pluses[i],mat2.col_pluses,stateNb);
		col_ones[i] = sub_prod(mat2.col_ones[i],mat1.row_ones,stateNb);
		col_pluses[i] = sub_prod(mat2.col_pluses[i],mat1.row_pluses,stateNb);
	}

	update_hash();
}


void Vector::update_hash()
{
#if USE_SPARSE_VECTOR
	_hash = 0;
	for (size_t * index = entries; index != entries + entriesNb; index++)
		_hash ^= hash_value(*index) + 0x9e3779b9 + (_hash << 6) + (_hash >> 2);
#else
	_hash = this->hash_val(entries);
#endif
}

void Matrix::update_hash()
{
	_hash = 0;
	for (const Vector ** p = col_ones; p != col_ones + stateNb; p++)
		_hash ^= hash_value((size_t) *p) + 0x9e3779b9 + (_hash << 6) + (_hash >> 2);
	for (const Vector ** p = col_pluses; p != col_pluses + stateNb; p++)
		_hash ^= hash_value((size_t) *p) + 0x9e3779b9 + (_hash << 6) + (_hash >> 2);
	for (const Vector ** p = row_ones; p != row_ones + stateNb; p++)
		_hash ^= hash_value((size_t) *p) + 0x9e3779b9 + (_hash << 6) + (_hash >> 2);
	for (const Vector ** p = row_pluses; p != row_pluses + stateNb; p++)
		_hash ^= hash_value((size_t) *p) + 0x9e3779b9 + (_hash << 6) + (_hash >> 2);
}

Matrix Matrix::prod(const Matrix & mat1, const Matrix & mat2)
{
	Matrix result(mat1, mat2);
	return result;
}

bool Matrix::recurrent(int j) const{// works only on idempotent
    size_t * vpred=this->col_ones[j]->entries;
    size_t * vmax=this->col_ones[j]->entries+this->col_ones[j]->entriesNb;
    size_t * vdebut=this->row_ones[j]->entries;
    size_t * vfin=vdebut+ this->row_ones[j]->entriesNb;

    for (size_t * vsucc = vdebut; vsucc != vfin ; vsucc++)
	{
            while(*vpred<*vsucc) {vpred++; if (vpred==vmax) return false;}
            if(*vpred!=*vsucc) return false;
            else vpred++;
	}
//cout<<j<<" recurrent"<<endl;
	return true;
}

/*Matrix Matrix::stab(const Matrix & mat){
    Matrix result(mat.stateNb);
    return result;
} //always call on an idempotent
*/

Vector purge(const Vector *varg,bool * tab){//create a new vector, keep only coordinates of v that are true in tab
    int nbtab=0;//size of purged vector to precompute
        for (size_t * ent = varg->entries; ent != varg->entries + varg->entriesNb; ent++)
	{
        if(tab[*ent]) nbtab++;
    }//end of precomputation
    size_t *data=(size_t*) malloc(nbtab*(sizeof(size_t)));
    size_t *datastart=data;
    for (size_t * ent = varg->entries; ent != varg->entries + varg->entriesNb; ent++)//purge
	{
        if(tab[*ent]) *data++=*ent;
    }
    Vector result(datastart,nbtab);
	return result;
}

Matrix Matrix::stab(const Matrix & mat) //always call on an idempotent
{

    uint n=mat.stateNb;
    Matrix result(n);

    //precompute recurrent elements, and columns
    bool *tabrec=(bool *)malloc (n*sizeof(bool));
        for(uint j=0; j<n;j++){
        tabrec[j]=mat.recurrent(j);
        if(tabrec[j]) {
                result.col_ones[j]=mat.col_ones[j];
        }else{
            result.col_ones[j]=Matrix::zero_vector;
        }
       result.col_pluses[j]=mat.col_pluses[j];
        }
    for(uint i=0; i<n;i++){
        result.row_pluses[i]=mat.row_pluses[i];
        Vector v=purge(mat.row_ones[i],tabrec);
        unordered_set<Vector>::iterator it = Matrix::vectors.emplace(v).first;
        result.row_ones[i]=&(*it);
    }
    //#endif // USE_SPARSE_VECTOR
    result.update_hash();
    return result;
}

bool Matrix::isIdempotent() const
{
	return false;
}


/* adds a pair of expression and matrix in the monoid*/
void UnstableMarkovMonoid::addElement(const ExtendedExpression * new_expr, const Matrix * new_mat)
{
#ifdef VERBOSE
	cout << "Adding element ";
	new_expr->print();
	cout << endl;
#endif
	expr_to_mat[new_expr] = new_mat;
	mat_to_expr[new_mat] = new_expr;
	if (expr_to_mat.size() % 10000 == 0)
		cout << expr_to_mat.size() << " elements and " << rewriteRules.size() << " rewrite rules  and " << Matrix::vectors.size() << " vectors and " << matrices.size() << " matrices " << endl;
}

/* adds a rewrite rule */
void UnstableMarkovMonoid::addRewriteRule(const ExtendedExpression * pattern, const ExtendedExpression * rewritten)
{
#ifdef VERBOSE
	cout << "Adding rewrite rule ";
	pattern->print();
	cout << endl << " to " << endl;
	rewritten->print();
	cout << endl;
#endif
	const ConcatExpr * cc = isConcatExpr(pattern);
	if (cc != NULL)
	{
		if (concatExpressions.find(*cc) == concatExpressions.end())
		{
			cout << "Arrrgh ..." << endl;
		}
	}
	rewriteRules[pattern] = rewritten;
}


void UnstableMarkovMonoid::addLetter(char a, ExplicitMatrix & mat)
{
/*	if (mat.stateNb != dim)
		throw runtime_error("Wrong size of matrix in addLetter");
*/

	unordered_set<LetterExpr>::const_iterator it
		= letterExpressions.emplace(a).first;

	unordered_set<Matrix>::const_iterator it2
		= matrices.emplace(mat).first;

	const ExtendedExpression * new_expr = &(*it);
	const Matrix * new_mat = &(*it2);
	addElement(new_expr, new_mat);

	new_elements.push_back(new_expr);
}

void  UnstableMarkovMonoid::process_expression(const ExtendedExpression * elt, vector<const ExtendedExpression *> & added_elts)
{
#ifdef VERBOSE
	cout << endl << "Processing expression '"; elt->print(); cout << "'" << endl;
#endif

	/* for each letter */
	for (unordered_set<LetterExpr>::iterator it = letterExpressions.begin(); it != letterExpressions.end(); it++)
	{
		const LetterExpr * letterExpr = &(*it);

#ifdef VERBOSE
		cout << endl << "Trying to append letter '" << letterExpr->letter << "' to expr '"; elt->print(); cout << "'" << endl;
#endif

		const ConcatExpr * concatExpr = isConcatExpr(elt);
		if (concatExpr != NULL)
		{
			ConcatExpr new_concat_expr(letterExpr, concatExpr->sonsNb + 1);

			bool rewrite_rule_found = false;


			/* for each strict suffix, we test if the expression (which is necessarilly known) is reducible */
			for (uint i = 0; i < concatExpr->sonsNb - 1; i++)
			{
				const ExtendedExpression * new_e = concatExpr->sons[i];
				new_concat_expr.addLeftSon(new_e);

#ifdef VERBOSE
				cout << "Checking strict suffix '"; new_concat_expr.print(); cout << "'" << endl;
#endif

				unordered_set<ConcatExpr>::iterator ok = concatExpressions.find(new_concat_expr);
				if (ok != concatExpressions.end())
				{
					const ConcatExpr * uuid = &(*ok);
					map<const ExtendedExpression *, const ExtendedExpression *>::iterator rewrite_rule = rewriteRules.find(uuid);
					/* skip to next letter if a rewrite rule exists */
					if (rewrite_rule != rewriteRules.end())
					{
						rewrite_rule_found = true;
#ifdef VERBOSE
						cout << "Found strict suffix rewrite rule to "; rewrite_rule->second->print(); cout << endl;
#endif
						break;
					}
				}
				else
				{

#ifdef VERBOSE
					cout << "Unknown concat expression !##!??##!!" << endl;
#endif
				}
			}

			if (!rewrite_rule_found)
			{
#ifdef VERBOSE
				cout << "No rewrite rule found for strict suffixes " << endl;
#endif
				/* we compute the matrix for the largest suffix */
				const Matrix * mat = expr_to_mat[elt];
				pair< unordered_set<Matrix>::iterator, bool> result = matrices.emplace(*mat, *expr_to_mat[letterExpr]);

				new_concat_expr.addLeftSon(concatExpr->sons[concatExpr->sonsNb - 1]);
				unordered_set<ConcatExpr>::iterator it = concatExpressions.emplace(new_concat_expr).first;

				const ConcatExpr * new_expr = &*it;

#ifdef VERBOSE
				cout << "Looking for a rewrite rule for expression "; new_expr->print(); cout << endl;
#endif

				if (result.second)
				{
#ifdef VERBOSE
					cout << "No equivalent matrix found to matrix " << endl;
					(*result.first).print();
					cout << "Vectors are " << endl;
					for (unordered_set<Vector>::iterator it = Matrix::vectors.begin(); it != Matrix::vectors.end(); it++)
					{
						(*it).print();
					}
#endif
					addElement(new_expr, &(*result.first));
					added_elts.push_back(new_expr);
				}
				else
				{
					const ExtendedExpression * rewriten = mat_to_expr[&(*result.first)];
#ifdef VERBOSE
					cout << "Found equivalent expression " << endl; rewriten->print(); cout << endl;
#endif
					addRewriteRule(new_expr, rewriten);
				}
			}
		}

		const LetterExpr * leftLetterExpr = isLetterExpr(elt);
		if (leftLetterExpr != NULL)
		{
			/* we compute the matrix for the largest suffix */
			const Matrix * mat = expr_to_mat[elt];
			pair< unordered_set<Matrix>::iterator, bool> result = matrices.emplace(*mat, *expr_to_mat[letterExpr]);

			ConcatExpr new_concat_expr(letterExpr, 2);
			new_concat_expr.addLeftSon(leftLetterExpr);

			unordered_set<ConcatExpr>::iterator it = concatExpressions.emplace(new_concat_expr).first;
			const ConcatExpr * new_expr = &*it;

			if (result.second)
			{
				addElement(new_expr, &(*result.first));
				added_elts.push_back(new_expr);
			}
			else
			{
				addRewriteRule(new_expr, mat_to_expr[&(*result.first)]);
			}
		}
	}
}
	/* todo diff\E9rents teste de r\E9ducibilit\E9 (cf tableau) */



bool UnstableMarkovMonoid::CloseByProduct(uint max_elements_nb)
{
	/* iterer sur tous les newbies */
	/* pour chaque newbie,
		pour chaque lettre,
		pour chaque suffixe,
		on teste si le suffixe est d\E9j\E0 r\E9ductible
			si oui-> on fait rien
			si non -> on calcule la matrice, on teste si elle existe d\E9j\E0a
				si oui-> on ajoute la nouvelle r\E8gle de r\E9\E9criture
				si non -> on ajoute le nouvel \E9l\E9ment
				*/

	/* elements added at this closure step */
	vector<const ExtendedExpression *> very_new_elts;

	size_t cur_index = 0;

	while (cur_index < new_elements.size())
	{
			const ExtendedExpression *expr = new_elements[cur_index];
			int size_before = very_new_elts.size();
			process_expression(expr, very_new_elts);
			new_elements.insert(
				new_elements.end(),
				very_new_elts.begin() + size_before,
				very_new_elts.end()
				);
			cur_index++;
/*
 			if (max_elements_nb > 0 && cur_index > max_elements_nb)
				throw runtime_error("Too many elements in the monoid");
*/
	}

	new_elements = very_new_elts;
	return (very_new_elts.size() != 0);
}

bool UnstableMarkovMonoid::CloseByStabilization()
{
	return true;
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


void MarkovMonoid::print()
{
	cout << "***************************************" << endl;
	cout << "Elements " << endl;
	cout << "***************************************" << endl;
	for (map<const ExtendedExpression *, const Matrix *>::iterator it = expr_to_mat.begin(); it != expr_to_mat.end(); it++)
	{
			it->first->print();
			cout << endl;
			it->second->print();
			cout << endl;
	}

	cout << "***************************************" << endl << endl;
	cout << "Rewrite rules " << endl;
	cout << "***************************************" << endl;
	for (map<const ExtendedExpression *, const ExtendedExpression *>::iterator it = rewriteRules.begin(); it != rewriteRules.end(); it++)
	{
		it->first->print();
		cout << "  ->  ";
		it->second->print();
		cout << endl;
	}

	/* todo */
}


void SharpedExpr::print() const
{
	if (isLetterExpr(son))
	{
		son->print();
		cout << "#";
	}
	else
	{
		cout << "(";
		son->print();
		cout << ")^#";
	}
}


void ConcatExpr::print() const
{
	for (uint i = sonsNb ; i > 0; i--)
		sons[i -1]->print();
}

void LetterExpr::print() const
{
	cout << letter;
}


ExplicitMatrix::ExplicitMatrix(uint stateNb) : stateNb(stateNb)
{
	coefficients = (char *)malloc(stateNb * stateNb * sizeof(char));
};

ExplicitMatrix::~ExplicitMatrix()
{
	free(coefficients);
	coefficients = NULL;
};
