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

#endif
