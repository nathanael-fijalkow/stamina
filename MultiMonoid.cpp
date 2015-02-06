

#include <iostream>
#include "Monoid.hpp"
#include <sstream>

#include "MultiMonoid.hpp"

UnstableMultiMonoid::UnstableMultiMonoid(uint dim, uint counter_number) : UnstableMonoid(dim)
{
	VectorInt::SetSize(dim);
	MultiCounterMatrix::init_act_prod(counter_number);
}

//Constructor from automa
UnstableMultiMonoid::UnstableMultiMonoid(const MultiCounterAut & automata) : UnstableMonoid(automata.NbStates)
{
	VectorInt::SetSize(automata.NbStates);
	MultiCounterMatrix::init_act_prod(automata.NbCounters);
	
	for (char letter = 0; letter < automata.NbLetters; letter++)
	{
		ExplicitMatrix mat(automata.NbStates);
		for (int i = 0; i < automata.NbStates; i++)
			memcpy(mat.coefficients + i * automata.NbStates, automata.trans[letter][i], automata.NbStates  * sizeof(char));
		addLetter('a' + letter, mat);
	}

	for (int i = 0; i < automata.NbStates; i++)
		if (automata.initialstate[i])
			initial_states.push_back(i);
	for (int i = 0; i < automata.NbStates; i++)
		if (automata.finalstate[i])
			final_states.push_back(i);
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

#define VERBOSE_MONOID_COMPUTATION 0

