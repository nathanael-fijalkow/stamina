#include "Automata.hpp"

using namespace std;

//Classical Automata
void ClassicAut::init(char Nletters,uint Nstates){
	NbLetters=Nletters;
	NbStates=Nstates;
	
	initialstate=(bool *)malloc(Nstates*sizeof(bool));
	memset(initialstate, false, Nstates);
	
	finalstate=(bool *)malloc(Nstates*sizeof(bool));
	memset(finalstate, false, Nstates);
	
	trans=(bool***)malloc(Nletters*sizeof(bool **));
	
	for(int a=0;a<Nletters;a++){
		trans[a]=(bool**)malloc(Nstates*sizeof(bool *));
		for(int i=0;i<Nstates;i++){
			trans[a][i]=(bool*)malloc(Nstates*sizeof(bool));
			memset(trans[a][i], false, Nstates);
		}
	}
}

//Constructor
ClassicAut::ClassicAut(char Nletters,uint Nstates){
	init(Nletters,Nstates);
	
}




//classical Automata with Epsilon-transitions
ClassicEpsAut::ClassicEpsAut(char Nletters,uint Nstates) : ClassicAut(Nletters,Nstates)
{
trans_eps=(bool**)malloc(Nstates*sizeof(bool *));
		for(int i=0;i<Nstates;i++){
			trans_eps[i]=(bool*)malloc(Nstates*sizeof(bool));
			memset(trans_eps[i], false, Nstates);
		}
		
transdet=(uint**)malloc(Nletters*sizeof(uint *));
		for(char a=0;a<Nletters;a++){
			transdet[a]=(uint*)malloc(Nstates*sizeof(uint));
			memset(transdet[a], 0, Nstates);
		}
}

//compute x^p
uint myPow(uint x, uint p)
{
  if (p == 0) return 1;
  if (p == 1) return x;

  int tmp = myPow(x, p/2);
  if (p%2 == 0) return tmp * tmp;
  else return x * tmp * tmp;
}

uint TwoPow(uint n){ return (1 << n);} //compute 2^n

//kth bit of n
bool bit(uint n,uint k){ return (n >> k) & 1;}


//build a subset automaton, by taking the powerset automaton and adding epsilon-transitions.
//We assume that we come from a deterministic automaton.
//It also works if a nondeterminstic automaton for the complement language is given, with final states reversed.
ClassicEpsAut* toSubsetAut(ClassicAut *aut){
	uint n=aut->NbStates;
	uint nspow=TwoPow(n); //new number of states
	
	ClassicEpsAut* Subaut=new ClassicEpsAut(aut->NbLetters,nspow);
	//each new state is a subset of Q, represented by the binary representation of uint 0,1,2,..., nspow-1
	
	//the only initial new state is {i} where i initial. The function works if there are several initial states
	for(uint i=0;i<n;i++){
		if(aut->initialstate[i]) Subaut->initialstate[myPow(2,i-1)]=true;
	}
	
	//final states are subsets where all states are final state. This is because we actually powerset the automaton for the complement.
	for(uint i=0;i<nspow;i++){
		uint k=0;
		bool fin=true;
		while(k<n and fin) {
			fin=aut->finalstate[bit(i,k)];
		}
		Subaut->finalstate[i]=fin;
	}
	
	
	
	//transitions are as usually in powerset
	
	bool* tabj=(bool *)malloc(n*sizeof(bool));
	for(char a=0;a<aut->NbLetters;a++){
		for(uint i=0;i<nspow;i++){
			//compute the j such that i--a-->j
			memset(tabj,false,n);
			for(uint k=0;k<n;k++){
				if(bit(i,k)){ 
					//put the destination(s) of k in tabj;
					for(uint x=0;x<n;x++){ if (aut->trans[a][k][x]) tabj[x]=true;}
				}				
			}
			//compute j from tabj
			uint j=0;
			for(uint k=0;k<n;k++){
				if(tabj[k]) j+= (1<<k);
			}
			
			//put transition (i,a,j), in both tables (nondeterministic and deterministic)
			Subaut->trans[a][i][j]=true;
			Subaut->transdet[a][i]=j;
		}
	}
	
	
	//we add epsilon-transitions witnessing the inclusion relation: P->Q if P is contained in Q
	for(uint i=0;i<nspow;i++){
		for(uint j=0;j<nspow;j++){
			if(i|j == j) Subaut->trans_eps[i][j]=true; //test for bitwise inclusion
		}
	}
		
	return Subaut;
	
}



//Multi-counter Automata
void MultiCounterAut::init(char Nletters,uint Nstates, char Ncounters){
	NbLetters=Nletters;
	NbStates=Nstates;
	NbCounters=Ncounters;
	
	initialstate=(bool *)malloc(Nstates*sizeof(bool));
	memset(initialstate, false, Nstates);
	
	finalstate=(bool *)malloc(Nstates*sizeof(bool));
	memset(finalstate, false, Nstates);
	
	trans=(char***)malloc(Nletters*sizeof(char **));
	
	for(int a=0;a<Nletters;a++){
		trans[a]=(char**)malloc(Nstates*sizeof(char *));
		for(int i=0;i<Nstates;i++){
			trans[a][i]=(char*)malloc(Nstates*sizeof(char));
			memset(trans[a][i], 2*Ncounters+2, Nstates);
		}
	}
}

//Constructor
MultiCounterAut::MultiCounterAut(char Nletters,uint Nstates, char Ncounters){
	init(Nletters,Nstates,Ncounters);
	
}




//classical Automata with Epsilon-transitions
MultiCounterEpsAut::MultiCounterEpsAut(char Nletters,uint Nstates, char Ncounters) : MultiCounterAut(Nletters,Nstates,Ncounters)
{
trans_eps=(char**)malloc(Nstates*sizeof(char *));
		for(int i=0;i<Nstates;i++){
			trans_eps[i]=(char*)malloc(Nstates*sizeof(char));
			memset(trans_eps[i], 2*Ncounters+2, Nstates);
		}
		
		
transdet_state=(uint**)malloc(Nletters*sizeof(uint *));
		for(char a=0;a<Nletters;a++){
			transdet_state[a]=(uint*)malloc(Nstates*sizeof(uint));
			memset(transdet_state[a], 0, Nstates);
		}

transdet_action=(char**)malloc(Nletters*sizeof(char *));
		for(char a=0;a<Nletters;a++){
			transdet_action[a]=(char*)malloc(Nstates*sizeof(char));
			memset(transdet_action[a], 2*Ncounters+2, Nstates);
		}

			
}

//Epsilon removal in MultiCounter Automata
//We assume the letters are deterministic in epsaut
MultiCounterAut* EpsRemoval(MultiCounterEpsAut *epsaut){
	uint ns=epsaut->NbStates;
	char nl=epsaut->NbLetters;
	char nc=epsaut->NbCounters;
	
	MultiCounterAut *aut=new MultiCounterAut(nl,ns,nc);
	
	//TODO
	
	return aut;
}
