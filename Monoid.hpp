
/* INCLUDES */
#ifndef MONOID_HPP
#define MONOID_HPP

#include <mutex>

#include <map>
#include <set>

#include "Expressions.hpp"
#include "Matrix.hpp"

#include "ProbMatrix.hpp"
#include "OneCounterMatrix.hpp"
#include "MultiCounterMatrix.hpp"

using namespace std;

#define MAX_MONOID_SIZE 1000000

/* CLASS DEFINITIONS */
// This class describes a Markov Monoid
class Monoid
{
public:

	// Three sets containing the known expressions
	unordered_set<SharpedExpr> sharpExpressions;
	unordered_set<ConcatExpr> concatExpressions;
	unordered_set<LetterExpr> letterExpressions;

	// the two maps are inverses of one another
	map <const ExtendedExpression *, const Matrix *> expr_to_mat;
	map <const Matrix *, const ExtendedExpression *> mat_to_expr;

	// the map of rewrite rules
	map <const ExtendedExpression *, const ExtendedExpression *> rewriteRules;
	
	// Print
	void print(ostream& st = cout) const;
	void print_summary() const;
	void print_letters() const;
	string toString() const;

	//the set of canonical rewrite rules
	set<const ExtendedExpression *> canonicalRewriteRules;

	//the state names, default names if empty vector
	vector<string> state_names;

protected:

	// Constructor
	Monoid();
	virtual ~Monoid();

	/* static mutex, ensures at most one monoid is instatntiated at a time */
	static mutex singleton;
};

/* for printing to a file */
ostream& operator<<(ostream& os, const Monoid & monoid);

// This class describes Markov Monoid under construction, it extends the class Monoid
class UnstableMonoid : public Monoid
{
public:

	//Constructor. By defaut the constructor clears existing vectors list.
	UnstableMonoid(uint dim, bool clear_vectors = true);
	~UnstableMonoid();

	// Adds a new letter
	const Matrix * addLetter(char a, ExplicitMatrix & mat);

	// Adds a rewrite rule
	void addRewriteRule(const ExtendedExpression *, const ExtendedExpression *);

	// The vector of known elements
	vector<const ExtendedExpression *> elements;

	// The vector of elements added at the previous closure by product step
	vector<const ExtendedExpression *> new_elements;

	// The vector of elements added at this closure by product step (which are to be sharpified)
	vector<const ExtendedExpression *> to_be_sharpified;

	// Function closing the current monoid by concatenating all elements
	// stops the computation and returns a non NULL witness if found
	const ExtendedExpression * CloseByProduct();

	// Function closing the current monoid by stabilizing all idempotents elements
	// stops the computation and returns a non NULL witness if found
	const ExtendedExpression * CloseByStabilization();

	// Function computing the smallest Markov Monoid containing a given unstable Markov Monoid
	// stops the computation and returns a non NULL witness if found
	const ExtendedExpression * ComputeMonoid();

	//Sets the witness test which will interrupt computation as soon a matrix matching the condition is found
	void setWitnessTest(bool(*test)(const Matrix *));

	int sharp_height(){
		return _sharp_height;
	};

protected:

	/* the witness test */
	bool(*test)(const Matrix *);

	/* returns a pair with the Matrix inserted or an already known matrix and a bool indicating whether the matrix was already known */
	virtual pair <Matrix *, bool> addMatrix(Matrix * mat) = 0;

	/* converts an explicit matrix */
	virtual Matrix * convertExplicitMatrix(const ExplicitMatrix & mat) const = 0;

	UnstableMonoid() : _sharp_height(0), cnt(0){};

	// Function processing an expression, computing products, and returning witness if one is found.
	const ExtendedExpression * process_expression(const ExtendedExpression * elt_left, const ExtendedExpression * elt_right);

	// Function processing an expression, computing stabilization, and returning witness if one is found.
	const ExtendedExpression * sharpify_expression(const ExtendedExpression *);

	//check idempotence
	bool is_idempotent(const Matrix * mat);

	// (not)idempotent matrices
	unordered_set<const Matrix *> idempotent;
	unordered_set<const Matrix *> notidempotent;

	int cnt;
	void check_size(int i)
	{
		cout << expr_to_mat.size() << " expressions ";
		cout << " and " << rewriteRules.size() << " rules and " << Matrix::vectors.size() << " vectors and " << i << " expressions to process." << endl;
		cnt = MAX_MONOID_SIZE / 1000;

		if (new_elements.size() > MAX_MONOID_SIZE)
				throw std::runtime_error("Monoid too large");
	}

	int _sharp_height;
};




#endif
