/* INCLUDES */
#ifndef MARKOV_MONOID_HPP
#define MARKOV_MONOID_HPP

#include <map>
#include <set>

#include "Matrix.hpp"
#include "Expressions.hpp"

using namespace std;

#define MAX_MONOID_SIZE 1000000

/* CLASS DEFINITIONS */
// This class describes a Markov Monoid
class Monoid
{
public:

	// the two maps are inverses of one another
	map <const ExtendedExpression *, const Matrix *> expr_to_mat;
	map <const Matrix *, const ExtendedExpression *> mat_to_expr;

	// the map of rewrite rules
	map <const ExtendedExpression *, const ExtendedExpression *> rewriteRules;
	
	// Print
	void print() const;
	string toString() const;

	//the set of canonical rewrite rules
	set<const ExtendedExpression *> canonicalRewriteRules;

protected:

	// Constructor
	Monoid(){};



};

/* for printing to a file */
ostream& operator<<(ostream& os, const Monoid & monoid);

// This class describes Markov Monoid under construction, it extends the class Monoid
class UnstableMonoid : public Monoid
{
public:

	UnstableMonoid(int dim);
	~UnstableMonoid();
	void init(int dim);

	// Adds a new letter
	const Matrix * addLetter(char a, ExplicitMatrix & mat);

	// Adds a rewrite rule
	void addRewriteRule(const ExtendedExpression *, const ExtendedExpression *);

	// Three sets containing the known expressions
	unordered_set<SharpedExpr> sharpExpressions;
	unordered_set<ConcatExpr> concatExpressions;
	unordered_set<LetterExpr> letterExpressions;

	// The vector of known elements
	vector<const ExtendedExpression *> elements;

	// The vector of elements added at the previous closure by product step
	vector<const ExtendedExpression *> new_elements;

	// The vector of elements added at this closure by product step (which are to be sharpified)
	vector<const ExtendedExpression *> to_be_sharpified;

	// Function closing the current monoid by concatenating all elements
	void CloseByProduct();

	// Function closing the current monoid by stabilizing all idempotents elements
	void CloseByStabilization();

	// Function computing the smallest Markov Monoid containing a given unstable Markov Monoid
	void ComputeMonoid();

	int sharp_height(){
		return _sharp_height;
	};

protected:

	/* returns a pair with the Matrix inserted or an already known matrix and a bool indicating whether the matrix was already known */
	virtual pair <Matrix *, bool> addMatrix(Matrix * mat) = 0;

	/* converts an explicit matrix */
	virtual Matrix * convertExplicitMatrix(const ExplicitMatrix & mat) const = 0;

	UnstableMonoid::UnstableMonoid() : _sharp_height(0), cnt(0){};

	// Function processing an expression, computing products
	void process_expression(const ExtendedExpression * elt_left,const ExtendedExpression * elt_right);

	// Function processing an expression, computing stabilization
	void sharpify_expression(const ExtendedExpression *);

	// Number of states of the automaton
	uint dim;

	//check idempotence
	bool is_idempotent(const Matrix * mat);

	// (not)idempotent matrices
	unordered_set<const Matrix *> idempotent;
	unordered_set<const Matrix *> notidempotent;

	int cnt;
	void check_size(int i)
	{
		cout << expr_to_mat.size() << " exprs ";
		cout << " and " << rewriteRules.size() << " rules " << i << " prods" << endl;
		cnt = MAX_MONOID_SIZE / 100;
		if (new_elements.size() > MAX_MONOID_SIZE)
				throw std::runtime_error("Monoid too large");
			if (i > 100* MAX_MONOID_SIZE)
				throw std::runtime_error("Too much work");
	}

	int _sharp_height;
};


class UnstableMarkovMonoid : public UnstableMonoid
{
public:
	// Creates zero vector
	UnstableMarkovMonoid(uint dim);

	// The set containing the known matrices
	unordered_set<ProbMatrix> matrices;

	//Computes the maximum number of leaks and the associated expreession 
	pair<int, const ExtendedExpression *> maxLeakNb();

	//get recurrent states
	const Vector * recurrent_states(const Matrix * mat);

	//get recurrence classes
	const Vector * recurrence_classes(const Matrix * mat);

#if CACHE_RECURRENT_STATES
	// maps from matrice to states which are recurrent
	map <const Matrix *, const Vector *> mat_to_recurrent_states;
	// maps from matrice to one recurrent state per class
	map <const Matrix *, const Vector *> mat_to_recurrence_classes;

#endif


protected:
	pair <Matrix *, bool> addMatrix(Matrix * mat);

	/* converts an explicit matrix */
	Matrix * convertExplicitMatrix(const ExplicitMatrix & mat) const;

};


class UnstableStabMonoid : public UnstableMonoid
{
public:
		// Creates zero vector
	UnstableStabMonoid(uint dim);

	// The set containing the known matrices
	unordered_set <OneCounterMatrix> matrices;

	protected:
	pair <Matrix *, bool> addMatrix(Matrix * mat);

	/* converts an explicit matrix */
	Matrix * convertExplicitMatrix(const ExplicitMatrix & mat) const;
};

#endif
