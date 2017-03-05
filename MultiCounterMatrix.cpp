#include "MultiCounterMatrix.hpp"
#include <stdlib.h>
#include <chrono>
#include <unordered_map>

#define TIME_BENCHMARKS 0

//Constructor
void MultiCounterMatrix::init()
{
    auto stnb = VectorInt::GetStateNb();
    rows = (const VectorInt **)malloc(stnb * sizeof(void *));
    cols = (const VectorInt **)malloc(stnb * sizeof(void *));
    memset(rows, stnb, 0);
    memset(cols, stnb, 0);
#if USE_REDUNDANCE_HEURISTIC
    row_red = (uint *) malloc(stnb * sizeof(void *));
    col_red = (uint *) malloc(stnb * sizeof(void *));
    memset(row_red, stnb, 0);
    memset(col_red, stnb, 0);
#endif
    is_idempotent = -1;
}

MultiCounterMatrix::~MultiCounterMatrix() {
    free(rows); free(cols);
    rows = cols = NULL;
#if USE_REDUNDANCE_HEURISTIC
    free(row_red); free(col_red);
    row_red = col_red = NULL;
#endif
}

MultiCounterMatrix::MultiCounterMatrix()
{
    init();
}

#if CACHE_VECTOR_PRODUCTS_HEUR
std::unordered_map<long long, unsigned char> cached_product_vectors;
#endif

void MultiCounterMatrix::set_counter_and_states_number(char NbCounters, uint NbStates)
{
    if(MultiCounterMatrix::N == NbCounters && VectorInt::GetStateNb() == NbStates && act_prod)
        return;
    
#if CACHE_VECTOR_PRODUCTS_HEUR
    cached_product_vectors.clear();
#endif
    
    free(mult_buffer);
    mult_buffer = (unsigned char *)malloc( 2 * NbStates * sizeof(char));
    
    
    VectorInt::SetSize(NbStates);
    
    MultiCounterMatrix::N = NbCounters;
    
    if(act_prod) {
        free(act_prod);
        act_prod = NULL;
    }
    
    act_prod = (unsigned char **) malloc((1 + (long) bottom())  *  sizeof(char*));
    for (uint i = 0; i <= bottom(); i++){
        act_prod[i] = (unsigned char *) malloc( (1 + (long) bottom())  *  sizeof(char));
        for (uint j = 0; j <= bottom(); j++){
            act_prod[i][j] =
            (i == bottom() || j == bottom())
            ? bottom()
            : (i == omega() || j == omega())
            ? omega()
            : ((i <= epsilon() & j <= epsilon()) || (epsilon() < i & epsilon() < j))
            ? min(i,j)
            : (i <= epsilon() & epsilon() < j & i < j - epsilon() )
            ? i
            : (i <= epsilon() & epsilon() < j & i >= j - epsilon() )
            ? j
            : (j <= epsilon() & epsilon() < i & j < i - epsilon() )
            ? j
            :  i;
        }
    }
    
    //set_zero_vector
    vector<char> nullvec(NbStates, bottom());
    auto it = int_vectors.emplace(nullvec).first;
    zero_int_vector = &(*it);
    
}

MultiCounterMatrix::MultiCounterMatrix(const MultiCounterMatrix *mat){
    init();
    auto stnb = VectorInt::GetStateNb();
    for(int i=0;i< stnb;i++ ){
        rows[i]=mat->rows[i];
        cols[i]=mat->cols[i];
    }
#if USE_REDUNDANCE_HEURISTIC
    memcpy(row_red, mat->row_red, stnb);
    memcpy(col_red, mat->col_red, stnb);
#endif
    update_hash();
}

//Constructor from Explicit Matrix
MultiCounterMatrix::MultiCounterMatrix(const ExplicitMatrix & explMatrix, char N)
{
    init();
    
    auto stnb = VectorInt::GetStateNb();
    
    if(stnb != explMatrix.stateNb)
        throw runtime_error("States number mismatch please call set_counter_and_states_number first");
    
    unsigned char * row = (unsigned char *) malloc(stnb);
    unsigned char * col = (unsigned char *) malloc(stnb);
    for (uint i = 0; i < stnb; i++)
    {
        for (uint j = 0; j < VectorInt::GetStateNb(); j++)
        {
            row[j] = explMatrix.coefficients[i][j];
            col[j] = explMatrix.coefficients[j][i];
        }
        auto itr = int_vectors.emplace(row).first;
        auto itc = int_vectors.emplace(col).first;
        rows[i] = &(*itr);
        cols[i] = &(*itc);
    }
    free(row); free(col);
#if USE_REDUNDANCE_HEURISTIC
    update_red();
#endif
    update_hash();
}

MultiCounterMatrix::MultiCounterMatrix(unsigned char diag, unsigned char nondiag)
{

    init();
    auto stnb = VectorInt::GetStateNb();
    
    unsigned char * row = (unsigned char *) malloc(stnb);
    unsigned char * col = (unsigned char *) malloc(stnb);
    for (uint i = 0; i < stnb; i++)
    {
        for (uint j = 0; j < VectorInt::GetStateNb(); j++)
            row[j] = col[j] = (i == j) ? diag : nondiag;
        auto itr = int_vectors.emplace(row).first;
        auto itc = int_vectors.emplace(col).first;
        rows[i] = &(*itr);
        cols[i] = &(*itc);
    }
    free(row); free(col);
    
#if USE_REDUNDANCE_HEURISTIC
    update_red();
#endif
    update_hash();
}

#if USE_REDUNDANCE_HEURISTIC
void MultiCounterMatrix::update_red() {
    auto stnb = VectorInt::GetStateNb();
    
    for(int i = 0; i < stnb; i++)
        for(int j = 0; j < i; j++)
            row_red[i] = (rows[i] == rows[j]) ? j : -1;
    for(int i = 0; i < stnb; i++)
        for(int j = 0; j < i; j++)
            col_red[i] = (cols[i] == cols[j]) ? j : -1;
}
#endif


char MultiCounterMatrix::N = 0;

unsigned char ** MultiCounterMatrix::act_prod = NULL;

// This is the constant vector with only zero entries
const VectorInt * MultiCounterMatrix::zero_int_vector = NULL;

int MultiCounterMatrix::get(int i, int j) const
{
    return (rows[i]->coefs[j]);
}

#define COEF_TO_STRING \
if (element == bottom()) result = "_ ";\
else if (element == omega()) result = "O ";\
else if (element ==  epsilon()) result = "E ";\
else if (element <  epsilon()) result = "r" + to_string(element);\
else result = "i" + to_string(element - epsilon() - 1);\
if (result.size() <= 1) result.push_back(' ');

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
            COEF_TO_STRING
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



#if USE_REDUNDANCE_HEURISTIC
//third argument is the redudancy array of cols
const VectorInt * MultiCounterMatrix::sub_prod_int(
                                                   const VectorInt * vec,
                                                   const VectorInt ** cols,
                                                   const uint * red_vec)
#else
const VectorInt * MultiCounterMatrix::sub_prod_int(
                                                   const VectorInt *,
                                                   const VectorInt ** cols);
#endif
{
    if(vec == zero_int_vector) {
        return zero_int_vector;
    }
    
    //memset(new_vec, 0, (VectorInt::GetStateNb() * sizeof(char)));
    auto stnb = VectorInt::GetStateNb();
    
#if CACHE_VECTOR_PRODUCTS_HEUR
    long long v =  ((long long) vec) << 32;
#endif
    
    for (int j = 0; j < stnb; j++)
    {
        auto colj = cols[j];
        if(colj == zero_int_vector) {
            mult_buffer[j] = bottom();
        } else {
#if CACHE_VECTOR_PRODUCTS_HEUR
            auto cached = cached_product_vectors.find(v | (long long) colj);
            if(cached != cached_product_vectors.end()) {
                mult_buffer[j] = cached->second;
            } else {
#endif
#if USE_REDUNDANCE_HEURISTIC
                //the redudnace heuristic avoids computing twice the same product
                if(red_vec[j] != -1) {
                    mult_buffer[j] = mult_buffer[ red_vec[j]];
                } else {
#endif
                    auto min_curr = bottom();
                    //compute scalar product
                    auto ab = vec->coefs;
                    auto ae = vec->coefs + stnb;
                    auto bb = colj->coefs;
#if USE_MIN_HEURISTIC
                    //the min value heristic breaks the loop if the min value is reached
                    auto m = (colj->min <= vec->min) ? colj->min : vec->min;
#endif
                    for (; ab < ae; ) {
                        auto a = act_prod[ *ab++ ][ *bb++ ];
                        if(a < min_curr) min_curr = a;
#if USE_MIN_HEURISTIC
                        if(min_curr == m) break;
#else
                        if(min_curr == 0) break;
#endif
                    }
                    mult_buffer[j] = min_curr;
#if USE_REDUNDANCE_HEURISTIC
                }
#endif
#if CACHE_VECTOR_PRODUCTS_HEUR
                cached_product_vectors[ v | (long long) colj ] = mult_buffer[ j ];
            }
#endif
        }
    }
    //we avoid multiplicating vectors:
    //if this one was already known we use the one already known
    auto it = int_vectors.emplace(mult_buffer).first;

    return &(*it);
}

unsigned char * MultiCounterMatrix::mult_buffer = NULL;

const MultiCounterMatrix * MultiCounterMatrix::prod(const Matrix * pmat1) const
{
#if TIME_BENCHMARKS
    auto start = std::chrono::high_resolution_clock::now();
#endif
    
    const MultiCounterMatrix & mat1 = *this;
    const MultiCounterMatrix & mat2 = *(MultiCounterMatrix *)pmat1;
    
    MultiCounterMatrix * result = new MultiCounterMatrix();
    
    auto stnb = VectorInt::GetStateNb();
    
#if USE_REDUNDANCE_HEURISTIC
    for (uint i = 0; i < stnb; i++){
        result->rows[i] =
        (row_red[i] != -1)
        ? result->rows[row_red[i]]
        : sub_prod_int(mat1.rows[i], mat2.cols, col_red);
        
        result->cols[i] =
        (col_red[i] != -1)
        ? result->cols[col_red[i]]
        : sub_prod_int(mat1.cols[i], mat2.rows, row_red);
    }
#else
    for (uint i = 0; i < stnb; i++){
        //if(i % 1024 == 256) cout << "Computing MultiCounterMatrix product line " << i << endl;
        result->rows[i] = sub_prod_int(mat1.rows[i], mat2.cols);
        result->cols[i] = sub_prod_int(mat2.cols[i], mat1.rows);
    }
#endif
    result->update_hash();
    
#if USE_REDUNDANCE_HEURISTIC
    result->update_red();
#endif
    
#if TIME_BENCHMARKS
    auto end = std::chrono::high_resolution_clock::now();
    if(false && stnb > 40) {
        cout << "prod dim " << stnb << " in "
        << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << endl;
    }
#endif
    return result;
}

// Construct a vector obtained by multiplying the line vec by all columns of mat.

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
#if TIME_BENCHMARKS
    auto start = std::chrono::high_resolution_clock::now();
#endif
    
    const MultiCounterMatrix * emat = new MultiCounterMatrix(this);
    while(true){
        auto new_emat = (*emat) * (*emat);
        if(*new_emat == *emat)
        {
            delete new_emat;
            break;
        }
        auto to_delete = emat;
        emat = new_emat;
        delete to_delete;
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
        diags[i] = (cd <= epsilon()) ? cd : (cd <= omega()) ? omega() : bottom();
    }
    //system("pause");
    
    for (uint i = 0; i <n; i++){
        auto rowi = emat->rows[i]->coefs;
        memset(new_row, bottom(), n*sizeof(char));
        for (uint j = 0; j<n; j++){
            auto colj = emat->cols[j]->coefs;
            //look for a possible path
            auto t = act_prod
            [ rowi[0] ]
            [ act_prod[diags[0]][colj[0]] ]
            ;
            for (uint b = 1; b<n; b++){
                if(t == 0)
                    break;
                t = min(t,
                        act_prod
                        [ rowi[b] ]
                        [ act_prod[diags[b]][colj[b]] ]
                        );
            }
            new_row[j] = t;
        }
        auto it = int_vectors.emplace(new_row).first;
        result->rows[i] = &(*it);
        //if(i % 32 == 0) cout << "Stab row " << i << endl;
    }
    
    for (uint i = 0; i <n; i++){
        auto coli = emat->cols[i]->coefs;
        memset(new_col, bottom(), n*sizeof(char));
        for (uint j = 0; j<n; j++){
            auto rowj = emat->rows[j]->coefs;
            //look for a possible path
            
            auto t = act_prod[rowj[0]]
            [act_prod[diags[0]][coli[0]]];
            
            for (uint b = 1; b<n; b++){
                if(t == 0)
                    break;
                t = min(t,
                        act_prod[rowj[b]]
                        [act_prod[diags[b]][coli[b]]]
                        );
            }
            new_col[j] = t;
        }
        
        auto it = int_vectors.emplace(new_col).first;
        result->cols[i] = &(*it);
        //if(i % 32 == 0) cout << "Stab col " << i << endl;
    }
    free(new_row);
    free(new_col);
    
    free(diags);
    delete(emat);
    result->update_hash();
    
#if TIME_BENCHMARKS
    auto end = std::chrono::high_resolution_clock::now();
    if(n > 40) {
        cout << "stab dim " << n << " in "
        << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << endl;
    }
#endif
    
    return result;
}
