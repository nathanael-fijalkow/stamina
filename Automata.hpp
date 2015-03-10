#ifndef AUTOMATA_HPP
#define AUTOMATA_HPP

#include "Matrix.hpp"
#include "VectorUInt.hpp"
#include <map>

#define VERBOSE_AUTOMATA_COMPUTATION 0
#define LOG_COMPUTATION_TO_FILE 1


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
	vector<bool> initialstate;
	
	//final states
	vector<bool> finalstate;
	
	//transition table: one boolean matrix for each letter
	map<char, vector<vector<bool>>> trans;

	virtual void print(ostream& st = cout);

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
	//if no transition, goes to state N+1.
	uint** transdet;
	
	//unique initial state
	uint initial;
	
	virtual void print(ostream& st = cout);

};


//turn a deterministic automaton into a subset automaton with epsilon-transitions
ClassicEpsAut* toSubsetAut(ClassicAut *aut);

//Minimisation of subset automata
ClassicEpsAut* SubMin(ClassicEpsAut *aut);

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

	//matrix product
	char** prod_mat(char **mat1, char **mat2);
	
	//det-matrix product
	char** prod_det_mat(uint *det_state, char *det_act, char** mat2 );

	//initialization
	void init(char nbletters, uint nbstates, char nbcounters);
	
	/* encoding of coefficients as integers */
	char inc(char counter){ return counter + NbCounters + 1; }
	char reset(char counter){ return counter; }
	char epsilon() { return NbCounters;}
	char omega() { return 2 * NbCounters + 1; }
	char bottom() { return 2 * NbCounters + 2; }

	/* returns -1 if code is not an increment and the corrsponding counter otherwise*/
	bool is_inc(char code) { return (code > NbStates && code < 2 * NbStates) ; }
	bool is_reset(char code){ return code < NbStates; }
	char get_inc_counter(char code) { return  is_inc(code) ?  code - NbStates - 1 : -1; }
	char get_reset_counter(char code){ return is_reset(code) ?  code : -1; }
	bool is_epsilon(char code){ return code == NbCounters; }
	bool is_omega(char code){ return code == 2 * NbCounters + 1; }
	bool is_bottom(char code){ return code == 2 * NbCounters + 2; }

	// This matrix act_prod is of size (2N+3)*(2N+3), it is computed once and for all.
	char ** act_prod;
	
	virtual void print(ostream& st = cout);
	string elementToString(char element);

protected:
	string state_index_to_string(int index);

};

string state_index_to_tuple(int index, int Nbstates);


//Classic automata with Epsilon-transition, in particular Subset Automata are of this type
class MultiCounterEpsAut : public MultiCounterAut
{
public:

	// Constructor
	MultiCounterEpsAut(char Nletters, uint Nstates, char Ncounters);

	//matrix for epsilon-transitions
	char** trans_eps;
	
	//deterministic transition table (if we know letters are deterministic, to avoid useless loops)
	//if no transition, goes to state N+1.
	uint** transdet_state;
	char** transdet_action;

	virtual void print(ostream& st = cout);

};

MultiCounterAut* EpsRemoval(MultiCounterEpsAut *aut);

uint myPow(uint x, uint p);

#endif
