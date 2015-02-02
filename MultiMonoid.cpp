

#include <iostream>
#include "Monoid.hpp"
#include <sstream>

#include "MultiMonoid.hpp"

UnstableMultiMonoid::UnstableMultiMonoid(uint dim) : UnstableMonoid(dim)
{

}

Matrix * UnstableMultiMonoid::convertExplicitMatrix(const ExplicitMatrix & mat) const
{
	return new MultiCounterMatrix(mat);
}

pair <Matrix *, bool> UnstableMultiMonoid::addMatrix(Matrix * mat)
{
	MultiCounterMatrix * mmat = (MultiCounterMatrix *)mat;
	auto it = matrices.emplace(*mmat);
	return pair<Matrix *, bool>((Matrix *)&(*it.first), it.second);
}

#define VERBOSE_MONOID_COMPUTATION 0

