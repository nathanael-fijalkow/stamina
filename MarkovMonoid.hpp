
/* INCLUDES */
#ifndef MARKOV_MONOID_HPP
#define MARKOV_MONOID_HPP

#include "ProbMatrix.hpp"
#include "Monoid.hpp"
#include "ExplicitAutomaton.hpp"



class UnstableMarkovMonoid : public UnstableMonoid
{
public:
	// Creates zero vector
	UnstableMarkovMonoid(uint dim);

    // Creates zero vector
    UnstableMarkovMonoid(const ExplicitAutomaton & aut);

    // Check whether the monoid has value 1
    const ExtendedExpression * hasValue1();

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

    //used by the value1Witness test
    static bool value1Test(const Matrix * m);
    static int initialState;
    static vector<bool> finalStates;
    static int autsize;
    
protected:
	pair <Matrix *, bool> addMatrix(const Matrix * mat);

	/* converts an explicit matrix */
	const Matrix * convertExplicitMatrix(const ExplicitMatrix & mat) const;

};



#endif
