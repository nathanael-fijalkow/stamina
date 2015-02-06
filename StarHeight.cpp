#include "StarHeight.hpp"




		

MultiCounterAut* toNestedBaut(ClassicAut *aut, char k){
	//We start by computing the subset automaton of aut
	//It has deterministic letters
	ClassicEpsAut* Subsetaut=toSubsetAut(aut);
	

	uint ns=Subsetaut->NbStates;
	char nl=Subsetaut->NbLetters;
	
#if VERBOSE_AUTOMATA_COMPUTATION
	printf("Subset Automaton Builded, %d states\n\n",ns);
	Subsetaut->print();
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
	
	uint N= (ns > 1) ? (myPow(ns,k+2)-ns)/(ns-1) : k+1;
	MultiCounterEpsAut* EpsBaut=new MultiCounterEpsAut(nl,N,k+1);
	
#if VERBOSE_AUTOMATA_COMPUTATION
	cout << "MultiCounterEpsAut " << endl;
	EpsBaut->print();
#endif

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
	

	uint l,bound;

	l=1;//length of the sequence
	bound=ns;//next time the length increases
	for(uint i=0;i<N;i++){ 
		if(i==bound){l++; bound=bound*ns+ns;}
		p = i % ns;//current state
		w = i-p;
		//Deterministic Letter transitions in the same component
		for(char a=0;a<nl;a++){
			
			if(Subsetaut->transdet[a][p]<ns) EpsBaut->transdet_state[a][i]=w+Subsetaut->transdet[a][p]; else EpsBaut->transdet_state[a][i]=N;
			//the action is increment of level l, i.e. (k+1)+l
			EpsBaut->transdet_action[a][i]=k+1+l;
		}
		//epsilon transitions in the same component
		for(uint q=0;q<ns;q++){
			if(Subsetaut->trans_eps[p][q]) EpsBaut->trans_eps[i][w+q]=k+1; //we can put action epsilon on epsilon-transitions			
		}
		
		/*
		//epsilon-transition wp->wpp with reset of level l (notice that operation 0 i.e. all reset is never performed)
		if(l<k+1) EpsBaut->trans_eps[i][i*ns+p]=l;
		
		//epsilon-transition wqp->wp with reset of level l-1
		if(l>1) EpsBaut->trans_eps[i][(w/ns)+p]=l-1;
		*/

		//epsilon-transition wp->wpp with reset of level l (notice that operation 0 i.e. all reset is never performed)
		int nouv = (ns>1) ? i*ns + p : i + 1;
		if (l<k + 1) EpsBaut->trans_eps[i][nouv] = l;

		//epsilon-transition wqp->wp with reset of level l-1
		nouv = (ns>1) ? (w / ns) - ((w / ns) % ns) + p : i - 1;
		if (l>1) EpsBaut->trans_eps[i][nouv] = l - 1;
	}
	

	auto epsremoved = EpsRemoval(EpsBaut);

#if VERBOSE_AUTOMATA_COMPUTATION
	cout << "Epsilon removed" << endl;
	epsremoved->print();
#endif

	return epsremoved;
}
