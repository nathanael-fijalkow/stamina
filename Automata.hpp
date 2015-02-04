#ifndef AUTOMATA_HPP
#define AUTOMATA_HPP

#include "Matrix.hpp"



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


class MultiCounterAut
{
public:

	// Constructor
	MultiCounterAut(char Nletters, uint Nstates, char Ncounters);

	// number of letters in alphabet, numbered 0,1,2,...
	char NbLetters;

	// number of states, numbered 0,1,2,...
	uint NbStates;
	
	//number of counters
	char NbCounters;

	//initial states
	bool* initialstate;
	
	//final states
	bool* finalstate;
	
	//transition table: one char matrix for each letter
	char*** trans;

protected:
	//initialization
	void init(char nbletters, uint nbstates, char nbcounters);

};


//Classic automata with Epsilon-transition, in particular Subset Automata are of this type
class MultiCounterEpsAut : public MultiCounterAut
{
public:

	// Constructor
	MultiCounterEpsAut(char Nletters, uint Nstates, char Ncounters);

	//matrix for epsilon-transitions
	char** trans_eps;
	
	//deterministic transition table (if we know letters are deterministic, to avoid useless loops)
	uint** transdet_state;
	char** transdet_action;
	
};

MultiCounterAut* EpsRemoval(MultiCounterEpsAut *aut);

#endif
