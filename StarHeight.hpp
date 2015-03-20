#ifndef STARHEIGHT_HPP
#define STARHEIGHT_HPP

#include "Automata.hpp"
#include "MultiMonoid.hpp"


//Computing the Loop Complexity of a Classic automaton. 
//Upper bound for star-height, obtainable via standard Automata->Expressions algorithms
char LoopComplexity(ClassicAut *aut);


// B-automaton testing if L(aut) has star-height at most k.
//aut must be either deterministic, or more generally can be the dual (wrt accepting state) of a non-deterministic automaton for the complement.
MultiCounterAut* toNestedBaut(ClassicAut *aut, char k);


#endif



