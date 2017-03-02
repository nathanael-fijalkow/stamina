
/* INCLUDES */
#ifndef STAB_MONOID_HPP
#define STAB_MONOID_HPP

#include "Monoid.hpp"
#include "MultiCounterMatrix.hpp"

class UnstableStabMonoid : public UnstableMonoid
{
public:
	// Creates zero vector
	UnstableStabMonoid(uint dim);

	// The set containing the known small matrices
	unordered_set <OneCounterSmallMatrix> small_matrices;
	unordered_set <OneCounterLargeMatrix> large_matrices;

protected:
	pair <Matrix *, bool> addMatrix(const Matrix * mat);

	/* converts an explicit matrix */
	virtual const Matrix * convertExplicitMatrix(const ExplicitMatrix & mat) const = 0;

};


#endif
