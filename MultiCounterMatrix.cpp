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
    memset(rows, 0, stnb * sizeof(void *));
    memset(cols, 0, stnb * sizeof(void *));
#if USE_REDUNDANCE_HEURISTIC
    row_red = (uint *) malloc(stnb * sizeof(int));
    col_red = (uint *) malloc(stnb * sizeof(int));
    memset(row_red, 0, stnb * sizeof(int));
    memset(col_red, 0, stnb* sizeof(int));
#endif
    is_idempotent = -1;
}

MultiCounterMatrix::~MultiCounterMatrix() {
    free(rows); free(cols);
    rows = NULL;
    cols = NULL;
#if USE_REDUNDANCE_HEURISTIC
    free(row_red); free(col_red);
    row_red = NULL;
    col_red = NULL;
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

void MultiCounterMatrix::copy_from(const MultiCounterMatrix & other) {
    init();
    auto stnb = VectorInt::GetStateNb();
    memcpy(rows, other.rows, stnb * sizeof(void *));
    memcpy(cols, other.cols, stnb  * sizeof(void *));
#if USE_REDUNDANCE_HEURISTIC
    memcpy(row_red, other.row_red, stnb * sizeof(int));
    memcpy(col_red, other.col_red, stnb  * sizeof(int));
#endif
    update_hash();
}

//Constructor for another copy
MultiCounterMatrix::MultiCounterMatrix(const MultiCounterMatrix & other) {
    copy_from(other);
}

// Second constructor: copy constructor
MultiCounterMatrix & MultiCounterMatrix::operator=(const MultiCounterMatrix & other) {
    copy_from(other);
    return *this;
}


//Constructor from Explicit Matrix
MultiCounterMatrix::MultiCounterMatrix(const ExplicitMatrix & explMatrix)
{
    init();
    
    auto stnb = VectorInt::GetStateNb();
    
    if(stnb != explMatrix.stateNb)
        throw runtime_error("States number mismatch please call set_counter_and_states_number first");
    
    unsigned char * row = (unsigned char *) malloc(stnb * sizeof(char));
    unsigned char * col = (unsigned char *) malloc(stnb  * sizeof(char));
    for (uint i = 0; i < stnb; i++)
    {
        for (uint j = 0; j < stnb; j++)
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
        for (uint j = 0; j < stnb; j++)
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
    
    for(int i = 0; i < stnb; i++) {
        row_red[i] = 0;
        for(int j = 0; j < i; j++) {
            if(rows[i] == rows[j]) {
                row_red[i] = j + 1;
                break;
            }
        }
    }
    for(int i = 0; i < stnb; i++) {
        col_red[i] = 0;
        for(int j = 0; j < i; j++) {
            if(cols[i] == cols[j]) {
                col_red[i] = j + 1;
                break;
            }
        }
    }
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
    auto stnb = VectorInt::GetStateNb();
    for (uint i = 0; i < stnb; i++){
        os << ( state_names.size() > i ? state_names[i] : to_string(i)) << ":" << " ";
        for (uint j = 0; j < stnb; j++)
        {
            //			os << " " << (int)((rows[i]->coefs[j] == 6) ? 6 : rows[i]->coefs[j]);
            string result = "";
            auto element = rows[i]->coefs[j];
            COEF_TO_STRING
            os << result << " ";
        }
        os << endl;
    }
#if USE_REDUNDANCE_HEURISTIC
    /*
    cout << "red_row: ";
    for(int i = 0; i < stnb; i++) {
        cout << row_red[i] <<  " ";
        if(row_red[i] > stnb + 1)
            cout << "oups";
    }
    cout << endl;
    cout << "red_col: ";
    for(int i = 0; i < stnb; i++) {
        cout <<  col_red[i] << " ";
        if(col_red[i] > stnb + 1)
            cout << "oups";
    }
    cout << endl;
     */
#endif
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
                                                   const VectorInt * vec,
                                                   const VectorInt ** cols)
#endif
{
    //memset(new_vec, 0, (VectorInt::GetStateNb() * sizeof(char)));
    auto stnb = VectorInt::GetStateNb();

#if CACHE_VECTOR_PRODUCTS_HEUR
    long long v =  ((long long) vec) << 32;
#endif
    
    for (int j = 0; j < stnb; j++)
    {
        auto colj = cols[j];
#if CACHE_VECTOR_PRODUCTS_HEUR
        auto cached = cached_product_vectors.find(v | (long long) colj);
        if(cached != cached_product_vectors.end()) {
            mult_buffer[j] = cached->second;
        } else {
#endif
#if USE_REDUNDANCE_HEURISTIC
            //the redudnace heuristic avoids computing twice the same product
            auto a = red_vec[j];
            if(a > 0) {
                mult_buffer[j] = mult_buffer[ a - 1];
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
    
    const MultiCounterMatrix * mat1 = this;
    const MultiCounterMatrix * mat2 = dynamic_cast<const MultiCounterMatrix *>(pmat1);
    
    MultiCounterMatrix * result = new MultiCounterMatrix();
    
    auto stnb = VectorInt::GetStateNb();
    
#if USE_REDUNDANCE_HEURISTIC
    for (uint i = 0; i < stnb; i++){
        auto a = row_red[i];
        result->rows[i] =
        (a > 0)
        ? result->rows[a - 1]
        : sub_prod_int(mat1->rows[i], mat2->cols, mat2->col_red);
        
        auto b = mat2->col_red[i];
        result->cols[i] =
        (b > 0)
        ? result->cols[b - 1]
        : sub_prod_int(mat2->cols[i], mat1->rows, mat1->row_red);
        if(result->cols[i] == NULL)
            cout << "show" << endl;
    }
#else
    for (uint i = 0; i < stnb; i++){
        //if(i % 1024 == 256) cout << "Computing MultiCounterMatrix product line " << i << endl;
        result->rows[i] = sub_prod_int(mat1->rows[i], mat2->cols);
        result->cols[i] = sub_prod_int(mat2->cols[i], mat1->rows);
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
    
    const MultiCounterMatrix * emat = new MultiCounterMatrix(*this);
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
