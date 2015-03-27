#ifndef STARHEIGHT_HPP
#define STARHEIGHT_HPP

#include "Automata.hpp"
#include "MultiMonoid.hpp"
#include <stack>

//graph structure, without letters
class GraphAut{
	public:
	// Constructor
	GraphAut(ClassicAut *aut);

	// number of states, numbered 0,1,2,...
	uint NbStates;

	
	//transition table: for each set i, trans[i] is a uint representing the set of successors
	vector<uint> trans;	
};

//Computing the Loop Complexity of a Classic automaton. 
//Upper bound for star-height, obtainable via standard Automata->Expressions algorithms
char LoopComplexity(ClassicAut *aut);


// B-automaton testing if L(aut) has star-height at most k.
//aut must be either deterministic, or more generally can be the dual (wrt accepting state) of a non-deterministic automaton for the complement.
MultiCounterAut* toNestedBaut(ClassicAut *aut, char k);


#endif



