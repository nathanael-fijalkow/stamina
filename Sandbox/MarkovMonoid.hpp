/* INCLUDES */
#ifndef MARKOV_MONOID_HPP
#define MARKOV_MONOID_HPP

#include <map>
#include <set>

#include "Matrix.hpp"
#include "Expressions.hpp"

using namespace std;

/* CLASS DEFINITIONS */
// This class describes a Markov Monoid
class MarkovMonoid
{
public:

	// the two maps are inverses of one another
	map <const ExtendedExpression *, const Matrix *> expr_to_mat;
	map <const Matrix *, const ExtendedExpression *> mat_to_expr;

	// the map of rewrite rules
	map <const ExtendedExpression *, const ExtendedExpression *> rewriteRules;
	
	// Print
	void print();

protected:

	// Constructor
	MarkovMonoid(){};

	//the set of canonical rewrite rules
	set<const ExtendedExpression *> canonicalRewriteRules;


};

// This class describes Markov Monoid under construction, it extends the class MarkovMonoid
class UnstableMarkovMonoid : public MarkovMonoid
{
public:

	// Creates zero vector
	UnstableMarkovMonoid(uint dim);

	// Free knowns vectors
	~UnstableMarkovMonoid();

	// Adds a new letter
	void addLetter(char a, ExplicitMatrix & mat);

	// Adds a rewrite rule
	void addRewriteRule(const ExtendedExpression *, const ExtendedExpression *);

	// Three sets containing the known expressions
	unordered_set<SharpedExpr> sharpExpressions;
	unordered_set<ConcatExpr> concatExpressions;
	unordered_set<LetterExpr> letterExpressions;

	// The set containing the known matrices
	unordered_set <Matrix> matrices;

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
	void ComputeMarkovMonoid(UnstableMarkovMonoid * monoid);

protected:

	// Function processing an expression, computing products
	void process_expression(const ExtendedExpression * elt_left,const ExtendedExpression * elt_right);

	// Function processing an expression, computing stabilization
	void sharpify_expression(const ExtendedExpression *);

	// Number of states of the automaton
	uint dim;
};


#endif
