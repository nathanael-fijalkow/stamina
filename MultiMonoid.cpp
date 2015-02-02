

#include <iostream>
#include "Monoid.hpp"
#include <sstream>

#include "MultiMonoid.hpp"

UnstableMultiMonoid::UnstableMultiMonoid(uint dim, uint counter_number) : UnstableMonoid(dim)
{
	VectorInt::SetSize(dim);
	MultiCounterMatrix::init_act_prod(counter_number);
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

