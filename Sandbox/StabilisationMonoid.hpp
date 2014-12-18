/* INCLUDES */
#ifndef STAB_MONOID_HPP
#define STAB_MONOID_HPP

#include "Monoid.hpp"

class UnstableStabMonoid : public UnstableMonoid
{
public:
	// The set containing the known matrices
	unordered_set <OneCounterMatrix> matrices;

};

#endif
