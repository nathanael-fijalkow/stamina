
#ifndef STARHEIGHT_HPP
#define STARHEIGHT_HPP

#include "RegExp.hpp"
#include "MultiMonoid.hpp"
#include <stack>
#include <list>

//graph structure, without letters
class GraphAut{
	public:
	// Constructor
	GraphAut(ClassicAut *aut);

	// number of states, numbered 0,1,2,...
	uint NbStates;
	
	//transition table: for each set i, trans[i] is a uint representing the set of successors
	vector<uint> trans;	
	
	//ordered list of states to be given to the Brzozowski Mccluskey algorithm, giving an expression of star-height LC(A) 
	list<uint> order;	
};

//Computing the Loop Complexity of a Classic automaton. 
//Upper bound for star-height, obtainable via standard Automata->Expressions algorithms
pair<char,list<uint>> LoopComplexity(ClassicAut *aut);



// B-automaton testing if L(aut) has star-height at most k.
//aut must be either deterministic, or more generally can be the dual (wrt accepting state) of a non-deterministic automaton for the complement.
MultiCounterAut* toNestedBaut(
                              ClassicEpsAut *SubsetAut,
                              char k,
                              bool debug = true,
                              bool output_file = true,
                              string filepref = ""
);


/* Compute the star height and return monoid and witness*/
int computeStarHeight(ClassicAut & aut,
                      UnstableMultiMonoid * & monoid,
                      const ExtendedExpression * & witness,
                      int & loopComplexity,
                      bool filelogs,
                      bool verbose,
                      string filepref = ""
);

ExtendedExpression *
checkLoopComplexitySuggestions(
                UnstableMultiMonoid * monoid,
                const MultiCounterAut & Baut,
                const list<ExtendedExpression*> & suggestions
                );



#endif



