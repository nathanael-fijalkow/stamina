/* INCLUDES */
#ifndef MARKOV_MONOID_HPP
#define MARKOV_MONOID_HPP

#include "ProbMatrix.hpp"
#include "Monoid.hpp"

class UnstableMarkovMonoid : public UnstableMonoid
{
public:
	// Creates zero vector
	UnstableMarkovMonoid(uint dim);

	// The set containing the known matrices
	unordered_set<ProbMatrix> matrices;

	//Computes the maximum number of leaks and the associated expression 
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


#endif
