#include "StarHeight.hpp"
#include <fstream>



//auxiliary function for acyclicity of automata induced by a subset of states
bool acyclic(ClassicAut *aut, uint subset){
	//subset uses the standard powerset encoding

	if(subset==0) return 0;
	//TODO
}

//auxiliary function for strong connectedness of automata induced by a subset of states
bool connected(ClassicAut *aut, uint subset){
	//TODO
	
}

//list the connected components
vector<uint> SCC(ClassicAut *aut, uint subset){
	//TODO
	
}

//recursive auxiliary function for Loop Complexity, on automaton induced by a subset
char RecLC(ClassicAut *aut, uint subset){
	if (acyclic(aut, subset)) return 0;
	if (connected(aut,subset)){
		//compute 1+min(lc(A-p))
		uint minloop=aut->NbStates;
		uint newmin;
		for(uint p=0;p<aut->NbStates;p++){
			if (bit(subset,p)){
				newmin=RecLC(aut, subset-TwoPow(p));
				if (newmin<minloop) minloop=newmin;
			}
		}
		return 1+minloop;
	}
	//else max(lc(SCC)) 
	
	vector<uint> vec=SCC(aut, subset);
	uint newmax, maxloop=0;
	for (vector<int>::iterator it = vec.begin() ; it != vec.end(); ++it){ 
		newmax=RecLC(aut, *it);
		if(newmax>maxloop) maxloop=newmax;
	}
	return maxloop;
	
}

//final function
char LoopComplexity(ClassicAut *aut){
	
	//for each set of states, we compute the loop complexity of the corresponding induced automaton.
	//This is done inductively according to the size of the set of states
	
	cout << "Computing the Loop Complexity..." << endl;
	uint n=aut->NbStates;	
	uint S=TwoPow(n); //number of subsets of the states. element S-1 represents the full automaton, element 0 the empty set.
	
	return RecLC(aut, S-1);
}
		

MultiCounterAut* toNestedBaut(ClassicAut *aut, char k){

	cout << "Computing the subset automaton..." << endl;
	//We start by computing the subset automaton of aut
	//It has deterministic letters
	ClassicEpsAut* Subsetaut=toSubsetAut(aut);
	

	uint ns=Subsetaut->NbStates;
	char nl=Subsetaut->NbLetters;
	
#if VERBOSE_AUTOMATA_COMPUTATION
	printf("Subset Automaton Builded, %d states\n\n",ns);
	Subsetaut->print();
#endif	
#if LOG_COMPUTATION_TO_FILE
	{
		ofstream file("subset_automaton.txt");
		Subsetaut->print(file);
	}
#endif
	
	/*
	Subsetaut=SubMin(Subsetaut);
	
	ns=Subsetaut->NbStates;
	
#if VERBOSE_AUTOMATA_COMPUTATION
	printf("Minimized Subset Automaton Computed, %d states\n\n", ns);
	Subsetaut->print();
#endif
	*/

	//states of the resulting automaton are words of Q* of length in [1,k+1]
	//there are n+n^2+...+n^(k+1)=(n^(k+2)-n)/(n-1)
	//there are k+1 counters
	
	cout << "Computing the multicounter epsilon automaton..." << endl;
	uint N = (ns > 1) ? (myPow(ns, k + 2) - ns) / (ns - 1) : k + 1;
	MultiCounterEpsAut* EpsBaut=new MultiCounterEpsAut(nl,N,k+1);
	


	//the last state on the pile is the reminder modulo ns

	//initial states are the ones from SubAut of length 1, i.e. the same
	for(uint i=0;i<ns;i++){ EpsBaut->initialstate[i]=(Subsetaut->initial==i); }
	
	//final states are the ones where the last state is final in Subsetaut
	uint p,w;
	for(uint i=0;i<N;i++){ 
		p = i % ns;
		if(Subsetaut->finalstate[p]) EpsBaut->finalstate[i]=true;
	}
	
	//TRANSITION table
	
	cout << "Removing epsilon transitions..." << endl;


	uint l,bound;

	l=1;//length of the sequence
	bound=ns;//next time the length increases
	for(uint i=0;i<N;i++){ 
		if(i==bound)
		{
			l++;
			bound=bound*ns+ns;
		}
		p = i % ns;//current state
		w = i-p;

		/* NEW DEBUG , better labels for edges */

		int x = w / ns - 1;  //état parent
		char action;

		//si state de la forme upp
		if (l>1 && (x%ns == p))
		{ //alors reset
			action = EpsBaut->reset(l-1);
		}
		else{ //sinon increment
			action = EpsBaut->inc(l-1);
		}

		for (char a = 0; a < nl; a++)
		{
			if (Subsetaut->transdet[a][p] < ns)
				EpsBaut->transdet_state[a][i] = w + Subsetaut->transdet[a][p];
			else
				EpsBaut->transdet_state[a][i] = N;
			EpsBaut->transdet_action[a][i] = action;
		}
		for (uint q = 0; q < ns; q++)
			if (Subsetaut->trans_eps[p][q])
				EpsBaut->trans_eps[i][w + q] = action;

		/*
		//Deterministic Letter transitions in the same component
		for(char a=0;a<nl;a++){
			
			if(Subsetaut->transdet[a][p]<ns) EpsBaut->transdet_state[a][i]=w+Subsetaut->transdet[a][p]; else EpsBaut->transdet_state[a][i]=N;
			//the action is increment of level l, i.e. (k+1)+l
			EpsBaut->transdet_action[a][i]=k+1+l;
		}
		//epsilon transitions in the same component
		for(uint q=0;q<ns;q++)
			if(Subsetaut->trans_eps[p][q])
				EpsBaut->trans_eps[i][w+q]=k+1; //we can put action epsilon on epsilon-transitions			
		*/
		/*
		//epsilon-transition wp->wpp with reset of level l (notice that operation 0 i.e. all reset is never performed)
		if(l<k+1) EpsBaut->trans_eps[i][i*ns+p]=l;
		
		//epsilon-transition wqp->wp with reset of level l-1
		if(l>1) EpsBaut->trans_eps[i][(w/ns)+p]=l-1;
		*/

		//epsilon-transition wp->wpp with reset of level l (notice that operation 0 i.e. all reset is never performed)
		/* BUGGY code
		int nouv = (ns>1) ? i*ns + p : i + 1;
		if (l<k + 1) EpsBaut->trans_eps[i][nouv] = l;
		*/

		int nouv = (ns > 1) ? (i + 1) * ns + p : i + 1;
		if (l < k + 1) EpsBaut->trans_eps[i][nouv] = action; /*  EpsBaut->reset(l); */

		//epsilon-transition wqp->wp with reset of level l-1
		/* BUGGY code
		nouv = (ns>1) ? (w / ns) - ((w / ns) % ns) + p : i - 1;
		if (l>1) EpsBaut->trans_eps[i][nouv] = l - 1;

		*/
		nouv = (ns > 1) ? x - (x % ns) + p : i - 1;
		//creates a reset 
		if (l > 1)
			EpsBaut->trans_eps[i][nouv] = action; /* l - 1; */

	}

#if VERBOSE_AUTOMATA_COMPUTATION
	cout << "****************************************" << endl << "MultiCounterEpsAut " << endl << "****************************************" << endl;
	EpsBaut->print();
#endif
#if LOG_COMPUTATION_TO_FILE
	{
		ofstream file("multicountereps_automaton.txt");
		EpsBaut->print(file);
	}
#endif

	

	auto epsremoved = EpsRemoval(EpsBaut);

#if VERBOSE_AUTOMATA_COMPUTATION
	cout << "****************************************" << endl << "Epsilon removed " << endl << "****************************************" << endl;
	epsremoved->print();
#endif
#if LOG_COMPUTATION_TO_FILE
	{
		ofstream file("epsremoved_automaton.txt");
		epsremoved->print(file);
	}
#endif

	return epsremoved;
}
