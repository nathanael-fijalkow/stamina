

#include <iostream>
#include "Monoid.hpp"
#include <sstream>

#include "StabilisationMonoid.hpp"

Matrix * UnstableStabMonoid::convertExplicitMatrix(const ExplicitMatrix & mat) const
{
	return new OneCounterMatrix(mat);
}

pair <Matrix *, bool> UnstableStabMonoid::addMatrix(Matrix * mat)
{
	OneCounterMatrix * mmat = (OneCounterMatrix *)mat;
	auto it = matrices.emplace(*mmat);
	return pair<Matrix *, bool>((Matrix *)&(*it.first), it.second);
}

// Constructor
UnstableStabMonoid::UnstableStabMonoid(uint dim)
{
	init(dim);
};

// Free known vectors
UnstableMonoid::~UnstableMonoid()
{
	Matrix::vectors.clear();
	Matrix::zero_vector = NULL;;
};

#define VERBOSE_MONOID_COMPUTATION 0

