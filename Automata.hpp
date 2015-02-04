#ifndef AUTOMATA_HPP
#define AUTOMATA_HPP

#include "Matrix.hpp"


class ClassicEpsAut;

class ClassicAut
{
public:

	// Constructor
	ClassicAut(char Nletters, uint Nstates);

	// number of letters in alphabet, numbered 0,1,2,...
	char NbLetters;

	// number of states, numbered 0,1,2,...
	uint NbStates;

	//initial states
	bool* initialstate;
	
	//final states
	bool* finalstate;
	
	//transition table: one boolean matrix for each letter
	bool*** trans;

protected:
	//initialization
	void init(char nbletters, uint nbstates);

};


//Classic automata with Epsilon-transition, in particular Subset Automata are of this type
class ClassicEpsAut : public ClassicAut
{
public:

	// Constructor
	ClassicEpsAut(char Nletters, uint Nstates);

	//matrix for epsilon-transitions
	bool** trans_eps;
	
	//deterministic transition table (if we know letters are deterministic, to avoid useless loops)
	uint** transdet;
};


//turn a deterministic automaton into a subset automaton with epsilon-transitions
ClassicEpsAut* toSubsetAut(ClassicAut *aut);

#endif
