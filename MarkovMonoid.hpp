#ifndef MARKOV_MONOID_HPP
#define MARKOV_MONOID_HPP

#include <map>

#include "Matrix.hpp"
#include <Expressions.hpp>

using namespace std;

// This class describes a Markov Monoid
class MarkovMonoid
{
public:
	// the two maps are inverses of one another
	map<const ExtendedExpression *, const Matrix *> expr_to_mat;
	map<const Matrix *, const ExtendedExpression *> mat_to_expr;

	// the map of rewrite rules
	map<const ExtendedExpression *, const ExtendedExpression *> rewriteRules;

	// Print
	void print();

protected:
	// Constructor
	MarkovMonoid(){};
};

// This class describes Markov Monoid under construction, it extends the class MarkovMonoid
class UnstableMarkovMonoid : public MarkovMonoid
{
public:

	// creates zero vector
	UnstableMarkovMonoid(uint dim);

	// Cleans up knowns vectors
	~UnstableMarkovMonoid();

	// Adds a new letter
	void addLetter(char a, ExplicitMatrix & mat);

	// Adds a pair (expression, matrix) in the monoid
	void addElement(const ExtendedExpression *, const Matrix * mat);

	// Adds a rewrite rule
	void addRewriteRule(const ExtendedExpression *, const ExtendedExpression *);

	// Three sets containing the known expressions
	unordered_set<SharpedExpr> sharpExpressions;
	unordered_set<ConcatExpr> concatExpressions;
	unordered_set<LetterExpr> letterExpressions;

	// The set containing the known matrices
	unordered_set <Matrix> matrices;

	// The vector of elements added at last closure step
	vector<const ExtendedExpression *> new_elements;

	// Function closing the current monoid by concatenating all elements
	bool CloseByProduct();

	// Function closing the current monoid by stabilizing all idempotents elements
	void CloseByStabilization();

protected:

	// Function processing an expression, computing products
	void process_expression(const ExtendedExpression *, vector<const ExtendedExpression *> & added_elts);

	// Function processing an expression, computing stabilization
	void sharpify_expression(const ExtendedExpression *, vector<const ExtendedExpression *> & added_elts);

	// Number of states of the automaton
	uint dim;
};


#endif