

#include <iostream>
#include "Monoid.hpp"
#include <sstream>

#include "MultiMonoid.hpp"

UnstableMultiMonoid::UnstableMultiMonoid(uint dim, uint counter_number) : UnstableMonoid(dim)
{
	initial_states.clear();
	final_states.clear();

	VectorInt::SetSize(dim);
	MultiCounterMatrix::set_counter_number(counter_number);
}

//Constructor from automa
UnstableMultiMonoid::UnstableMultiMonoid(const MultiCounterAut & automata) : UnstableMonoid(automata.NbStates)
{
	initial_states.clear();
	final_states.clear();

	VectorInt::SetSize(automata.NbStates);
	MultiCounterMatrix::set_counter_number(automata.NbCounters);
	
	for (char letter = 0; letter < automata.NbLetters; letter++)
	{
		ExplicitMatrix mat(automata.NbStates);
		mat.coefficients = automata.trans.at(letter);
		addLetter('a' + letter, mat);
	}

	for (int i = 0; i < automata.NbStates; i++)
		if (automata.initialstate[i])
			initial_states.push_back(i);

	for (int i = 0; i < automata.NbStates; i++)
		if (automata.finalstate[i])
			final_states.push_back(i);

	state_names.resize(automata.NbStates);
	for (int i = 0; i < automata.NbStates; i++)
		state_names[i] = state_index_to_tuple(i, automata.NbStates);

}

const ExtendedExpression * UnstableMultiMonoid::containsUnlimitedWitness()
{
	setWitnessTest(IsUnlimitedWitness);
	return ComputeMonoid();
}

vector<int> UnstableMultiMonoid::initial_states;
vector<int> UnstableMultiMonoid::final_states;

bool UnstableMultiMonoid::IsUnlimitedWitness(const Matrix * matrix)
{
	auto mat = (const MultiCounterMatrix *)matrix;
	return mat->isUnlimitedWitness(initial_states, final_states);
}



Matrix * UnstableMultiMonoid::convertExplicitMatrix(const ExplicitMatrix & mat) const
{
	return new MultiCounterMatrix(mat,MultiCounterMatrix::N);
}

pair <Matrix *, bool> UnstableMultiMonoid::addMatrix(Matrix * mat)
{
	MultiCounterMatrix * mmat = (MultiCounterMatrix *)mat;
	auto it = matrices.emplace(*mmat);
	return pair<Matrix *, bool>((Matrix *)&(*it.first), it.second);
}

ostream& operator<<(ostream& st, const UnstableMultiMonoid & monoid)
{
	auto & names = monoid.state_names;
	st << "***************************************" << endl;
	st << "Initial states (" << monoid.initial_states.size() << ")"<< endl;
	st << "***************************************" << endl;
	for (auto i : monoid.initial_states)
		st << (names.size() > i ? names[i] : to_string(i)) << " ";
	st << endl;

	st << "***************************************" << endl;
	st << "Final states (" << monoid.final_states.size() << ")" << endl;
	st << "***************************************" << endl;
	for (auto i : monoid.final_states)
		st << (names.size() > i ? names[i] : to_string(i)) << " ";
	st << endl;

	st << *(const Monoid *)(&monoid);
	return st;
}

#define MONOID_COMPUTATION_VERBOSITY 0

