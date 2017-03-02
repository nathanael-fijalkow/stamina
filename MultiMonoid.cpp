
#include <iostream>
#include "Monoid.hpp"
#include <sstream>

#include "MultiMonoid.hpp"

UnstableMultiMonoid::UnstableMultiMonoid(uint dim, uint counter_number) : UnstableMonoid(dim)
{
	initial_states.clear();
	final_states.clear();

	MultiCounterMatrix::set_counter_and_states_number(counter_number, dim);
}

//Constructor from automata
UnstableMultiMonoid::UnstableMultiMonoid(const MultiCounterAut & automata) : UnstableMonoid(automata.NbStates)
{
	initial_states.clear();
	final_states.clear();

    MultiCounterMatrix::set_counter_and_states_number(automata.NbCounters, automata.NbStates);
	
	for (unsigned char letter = 0; letter < automata.NbLetters; letter++)
	{
        /*
		ExplicitMatrix mat(automata.NbStates);
		mat.coefficients =
         */
        auto mat = automata.get_trans(letter).toExplicitMatrix();
		addLetter(letter, *mat);
        free(mat);
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



const MultiCounterMatrix * UnstableMultiMonoid::convertExplicitMatrix(const ExplicitMatrix & mat) const
{
	return new MultiCounterMatrix(mat,MultiCounterMatrix::counterNb());
}

pair <Matrix *, bool> UnstableMultiMonoid::addMatrix(const Matrix * mat)
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

const MultiCounterMatrix * UnstableMultiMonoid::ExtendedExpression2Matrix(const ExtendedExpression * expr,const MultiCounterAut & automata)
{
	const LetterExpr * lexpr = isLetterExpr(expr);
	const ConcatExpr * cexpr = isConcatExpr(expr);
	const SharpedExpr * sexpr = isSharpedExpr(expr);

	if(isLetterExpr(expr)){
		auto mat = automata.get_trans(lexpr->letter).toExplicitMatrix();
        auto res = addLetter(lexpr->letter, *mat);
        delete mat;
        return (const MultiCounterMatrix*) res;
	}
	else
	{
		if(isConcatExpr(expr))
		{
			const MultiCounterMatrix * mat = ExtendedExpression2Matrix(cexpr->sons[0], automata);
			
			for (uint i = 1; i < cexpr->sonsNb; i++) {
				mat = (*mat) * ExtendedExpression2Matrix(cexpr->sons[i], automata);
			}
			return mat;
		}
		else
		{
			if(!isSharpedExpr(expr)){
				cout<<"ERROR in UnstableMultiMonoid::ExtendedExpression2Matrix: Expression of unknown type"<<endl;
				return NULL;
			}
			const MultiCounterMatrix * mat = ExtendedExpression2Matrix(sexpr->son, automata);
			return mat->stab();
		}
	}
}

#define MONOID_COMPUTATION_VERBOSITY 0

