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


Matrix * UnstableMarkovMonoid::convertExplicitMatrix(const ExplicitMatrix & mat) const
{
	return new ProbMatrix(mat);
}

pair <Matrix *, bool> UnstableMarkovMonoid::addMatrix(Matrix * mat)
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

