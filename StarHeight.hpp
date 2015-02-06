#ifndef STARHEIGHT_HPP
#define STARHEIGHT_HPP

#include "Automata.hpp"
#include "MultiMonoid.hpp"

// B-automaton testing if L(aut) has star-height at most k.
//aut must be either deterministic, or more generally can be the dual (wrt accepting state) of a non-deterministic automaton for the complement.
MultiCounterAut* toNestedBaut(ClassicAut *aut, char k);


#endif



