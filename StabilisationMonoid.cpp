

#include <iostream>
#include "Monoid.hpp"
#include <sstream>

#include "StabilisationMonoid.hpp"

UnstableStabMonoid::UnstableStabMonoid(uint dim) : UnstableMonoid(dim)
{

}

Matrix * UnstableStabMonoid::convertExplicitMatrix(const ExplicitMatrix & mat) const
{
	if (Matrix::UseCentralizedVectorStorage())
		return new OneCounterLargeMatrix(mat);
	else
		return new OneCounterSmallMatrix(mat);
}

pair <Matrix *, bool> UnstableStabMonoid::addMatrix(Matrix * mat)
{
	if (Matrix::UseCentralizedVectorStorage())
	{
		OneCounterLargeMatrix * mmat = (OneCounterLargeMatrix *)mat;
		auto it = large_matrices.emplace(*mmat);
		return pair<Matrix *, bool>((Matrix *)&(*it.first), it.second);
	}
	else
	{
		OneCounterSmallMatrix * mmat = (OneCounterSmallMatrix *)mat;
		auto it = small_matrices.emplace(*mmat);
		return pair<Matrix *, bool>((Matrix *)&(*it.first), it.second);
	}
}


#define VERBOSE_MONOID_COMPUTATION 0

