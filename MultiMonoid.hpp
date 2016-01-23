
/* INCLUDES */
#ifndef MULTI_MONOID_HPP
#define MULTI_MONOID_HPP

#include "Monoid.hpp"
#include "MultiCounterMatrix.hpp"
#include "Automata.hpp"


class UnstableMultiMonoid : public UnstableMonoid
{
public:
	// Constructor
	UnstableMultiMonoid(uint dim, uint counter_number);

	//Constructor from automa
	UnstableMultiMonoid(const MultiCounterAut & automata);

	// The set containing the known small matrices
	unordered_set <MultiCounterMatrix> matrices;

	//Returns null if no witness, otherwise a witness
	const ExtendedExpression * containsUnlimitedWitness();

	static bool IsUnlimitedWitness(const Matrix * matrix);

	/* inital states of the automaton used for construction */
	static vector<int> initial_states;
	static vector<int> final_states;

	//Converts a given ExtendedExpression into the corresponding Matrix
	const Matrix * ExtendedExpression2Matrix(const ExtendedExpression * expr, const MultiCounterAut & automata);

protected:
	pair <Matrix *, bool> addMatrix(Matrix * mat);

	/* converts an explicit matrix */
	Matrix * convertExplicitMatrix(const ExplicitMatrix & mat) const;

};

/* for printing to a file */
ostream& operator<<(ostream& os, const UnstableMultiMonoid & monoid);


#endif
