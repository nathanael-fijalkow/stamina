
#ifndef AUTOMATA_HPP
#define AUTOMATA_HPP

#include "Matrix.hpp"
#include "MultiCounterMatrix.hpp"
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
	// transition table: one boolean
	// matrix for each letter
	map<char, vector<vector<bool>>> trans;

	void addLetter(char c, ExplicitMatrix m);
	
	//test wether the automaton is deterministic
	bool isdet();
	
	//test whether the automaton is complete
	bool iscomplete();
	
	//add a rejecting sink
	void addsink();
	
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

class charMat
{
    
};

class MultiCounterAut
{
public:

	// Constructor
	MultiCounterAut(char Nletters, uint Nstates, char Ncounters);
    virtual ~MultiCounterAut();

    // number of letters in alphabet, numbered 0,1,2,...
	char NbLetters;

	// number of states, numbered 0,1,2,...
	uint NbStates;
	
	//number of counters
	char NbCounters;

	//initial states
	vector<bool> initialstate;
	
	//final states
	vector<bool> finalstate;
	
	//matrix product
	//MultiCounterMatrix * prod_mat(MultiCounterMatrix * mat1, MultiCounterMatrix * mat2);

	//det-matrix product
	MultiCounterMatrix * prod_det_mat(uint * det_state, char * det_act, const MultiCounterMatrix * mat2);

	//initialization
	void init(char nbletters, uint nbstates, char nbcounters);
		
	virtual void print(ostream& st = cout);
	
    /* converts string EROI representation to char rrepresentation back end forth */
    string coef_to_string(char coef);
    char coef_to_char(string coef);

    static char coef_to_char(string coef, int NbCounters);

    void set_trans(char a, const MultiCounterMatrix & mat);

    const MultiCounterMatrix & get_trans(char a) const { return trans.at(a); };
    
protected:
	string state_index_to_string(int index);
    
    //transition table: one char matrix for each letter
    map<char, MultiCounterMatrix> trans;
    

};

string state_index_to_tuple(int index, int Nbstates);


//Classic automata with Epsilon-transition, in particular Subset Automata are of this type
class MultiCounterEpsAut : public MultiCounterAut
{
public:

	// Constructor
	MultiCounterEpsAut(char Nletters, uint Nstates, char Ncounters);
    virtual ~MultiCounterEpsAut();

	virtual void print(ostream& st = cout);

    //deterministic transition table (if we know letters are deterministic, to avoid useless loops)
    //if no transition, goes to state N+1.

    uint transdet_state(char letter, uint state) const;
    char transdet_action(char letter, uint state) const;
    uint * transdet_states(char letter) const;
    char * transdet_actions(char letter) const;
    void set_transdet_state(char letter, uint state, uint val);
    void set_transdet_action(char letter, uint state, char action);
    void set_trans_eps(const ExplicitMatrix & mat);
    const MultiCounterMatrix & get_trans_eps() const;

protected:

    //matrix for epsilon-transitions
    MultiCounterMatrix _trans_eps;
    
    //deterministic transition table (if we know letters are deterministic, to avoid useless loops)
    //if no transition, goes to state N+1.
    uint * _transdet_state; // Nletters * Nstates, initialisée à Nstates = no transition
    char * _transdet_action; // Nletters * Nstates, initialisée à 2 * Ncounters + 2

};

MultiCounterAut* EpsRemoval(MultiCounterEpsAut *aut);

uint myPow(uint x, uint p);

uint TwoPow(uint n);

bool bit(uint n,uint k);

#endif
