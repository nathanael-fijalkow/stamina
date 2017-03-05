
#include <iostream>
#include <sstream>

#include "MarkovMonoid.hpp"

#define MONOID_COMPUTATION_VERBOSITY 0


//Computes the maximum number of leaks of all sharped expression
pair<int, const ExtendedExpression *> UnstableMarkovMonoid::maxLeakNb()
{
	pair<int, const ExtendedExpression *> result(0, NULL);
	for (auto & gexpr_mat : expr_to_mat)
	{
		auto expr_mat = (ProbMatrix *)gexpr_mat.second;
		if ( is_idempotent(expr_mat))
		{
			int cl = expr_mat->countLeaks(recurrence_classes(expr_mat));
			if (cl > result.first)
			{
				result.first = cl;
				result.second = gexpr_mat.first;
			}
#if MONOID_COMPUTATION_VERBOSITY
			cout << "Checking whether "; expr_mat.first->print(); cout << " has a leak" << endl;
			if (cl > 0)
				cout << "Found " << cl << " leaks." << endl;
			//			cout << "Recurrence states" << endl; recurrent_states(expr_mat.second)->print(); cout << endl;
			//			cout << "Recurrence classes" << endl; recurrence_classes(expr_mat.second)->print(); cout << endl;
#endif
		}
	}
	return result;
}

// Constructor
UnstableMarkovMonoid::UnstableMarkovMonoid(uint dim) : UnstableMonoid(dim)
{
};

UnstableMarkovMonoid::UnstableMarkovMonoid(const ExplicitAutomaton & aut)
    : UnstableMonoid(aut.size)
{
    initialState = aut.initialState;
    finalStates.clear();
    finalStates.resize(aut.size, false);
    for(auto fs : aut.finalStates)
        finalStates[fs] = true;
    
    //	ret->setWitnessTest((bool(*)(const Matrix*))&test_witness);
    for(int i = 0 ; i < aut.alphabet.length();i++)
        addLetter(aut.alphabet[i],*(aut.matrices[i]));
    
}


// Check whether the monoid has value 1
const ExtendedExpression * UnstableMarkovMonoid::hasValue1() {

    setWitnessTest(&value1Test);
    
    auto exp = ComputeMonoid();
    if(exp) return exp;
    
    //in case the monod was already computed or half computed ...
    
    for(auto & mat : matrices) {
        if(value1Test(&mat)) {
            return mat_to_expr[&mat];
        }
    }
    return NULL;
}


const Matrix * UnstableMarkovMonoid::convertExplicitMatrix(const ExplicitMatrix & mat) const
{
	return new ProbMatrix(mat);
}

pair <Matrix *, bool> UnstableMarkovMonoid::addMatrix(const Matrix * mat)
{
	ProbMatrix * mmat = (ProbMatrix *)mat;
	auto it = matrices.emplace(*mmat);
	return pair<Matrix *, bool>((Matrix *)&(*it.first), it.second);
}


//get recurrent states
const Vector * UnstableMarkovMonoid::recurrent_states(const Matrix * mmat)
{
	ProbMatrix * mat = (ProbMatrix *) mmat;
#ifdef CACHE_RECURRENT_STATES
	auto recs = mat_to_recurrent_states.find(mat);
	if (recs == mat_to_recurrent_states.end())
	{
		auto vec = mat->recurrent_states();
		mat_to_recurrent_states[mat] = vec;
		return vec;
	}
	else
		return recs->second;
#else
	return mat->recurrent_states();
#endif
}

//get recurrence classes
const Vector * UnstableMarkovMonoid::recurrence_classes(const Matrix * mmat)
{
	ProbMatrix * mat = (ProbMatrix *)mmat;
#ifdef CACHE_RECURRENT_STATES
	auto recs = mat_to_recurrence_classes.find(mat);
	if (recs == mat_to_recurrence_classes.end())
	{
		auto vec = mat->recurrence_classes( recurrent_states(mat) );
		mat_to_recurrence_classes[mat] = vec;
		return vec;
	}
	else
		return recs->second;
#else
	return mat->recurrent_classes();
#endif
}

bool UnstableMarkovMonoid::value1Test(const Matrix * pm)
{
    auto m = (ProbMatrix *) pm;
    //  cout << "Call to test_witness for " << endl << *m;
    auto row = m->getRowOnes();
    auto ones = row[UnstableMarkovMonoid::initialState];
    
    
    /*	for (int i = 0; i < size; i++)
     cout << i << ": " << finalStates[i] << " " << ones->contains(i) << endl;
     */
    
    for (int i = 0; i < UnstableMarkovMonoid::autsize; i++)
        if( !UnstableMarkovMonoid::finalStates[i]  && ones->contains(i))
            return false;
    return true;
}

int UnstableMarkovMonoid::initialState = 0;
vector<bool> UnstableMarkovMonoid::finalStates;
int UnstableMarkovMonoid::autsize = 0;


