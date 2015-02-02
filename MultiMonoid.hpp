/* INCLUDES */
#ifndef MULTI_MONOID_HPP
#define MULTI_MONOID_HPP

#include "Monoid.hpp"
#include "MultiCounterMatrix.hpp"

class UnstableMultiMonoid : public UnstableMonoid
{
public:
	// Creates zero vector
	UnstableMultiMonoid(uint dim, uint counter_number);

	// The set containing the known small matrices
	unordered_set <MultiCounterMatrix> matrices;

protected:
	pair <Matrix *, bool> addMatrix(Matrix * mat);

	/* converts an explicit matrix */
	Matrix * convertExplicitMatrix(const ExplicitMatrix & mat) const;
};


#endif
